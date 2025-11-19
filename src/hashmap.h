/*
types:
- tlbt_map_KEY_VALUE           map type KEY and VALUE depend on your definitions
- tlbt_map_KEY_VALUE_key       key type wrapping KEY type. required for open addressing collision resolution
- tlbt_map_iterator_KEY_VALUE  iterator type

functions (_ph variants require you to provide the hash):
- tlbt_map_KEY_VALUE_get(_ph)         tries retrieving the value with a key
- tlbt_map_KEY_VALUE_remove(_ph)      tries removing the entry with a key
- tlbt_map_KEY_VALUE_insert(_ph)      tries inserting the entry with key and value
- tlbt_map_KEY_VALUE_contains(_ph)    checks if a value exists with its key
- tlbt_map_KEY_VALUE_clear            resets the map
- tlbt_map_KEY_VALUE_copy             tries copying the entries from one map to another
- tlbt_map_iterator_KEY_VALUE_init    initializes the iterator
- tlbt_map_iterator_KEY_VALUE_reset   resets the iterator
- tlbt_map_iterator_KEY_VALUE_iterate iterates the map
if TLBT_DYNAMIC_MEMORY is not defined
- tlbt_map_KEY_VALUE_init             initializes the map type (no allocations)
if TLBT_DYNAMIC_MEMORY is defined
- tlbt_map_KEY_VALUE_create           creates the map type (with allocations)
- tlbt_map_KEY_VALUE_destroy          destroys the map type (with deallocations)
- tlbt_map_KEY_VALUE_adjust_capacity  basically reallocates to the new capacity. downsizing doesn't work yet
- tlbt_map_KEY_VALUE_ensure_capacity  checks if adjusting is necessary and resizes by factor 2 if it is

TLBT_DEFINITION      if you want to define the types and functions in a header file
TLBT_IMPLEMENTATION  for the corresponding implementation of the definitions in a separate source file
TLBT_STATIC          if you want to define and implement them statically in a source file

=== required definitions ===
TLBT_KEY_T        the hashmap key type
TLBT_HASH_FUNC    the function for hashing the key
TLBT_EQUALS_FUNC  the function for checking key equality

=== optional definitions ===
TLBT_VALUE_T           the hashmap value type. when omitted it will be a hashset
TLBT_KEY_T_NAME        default is TLBT_KEY_T
TLBT_VALUE_T_NAME      default is TLBT_VALUE_T
TLBT_ASSERT            default is assert from <assert.h>
TLBT_BASE2_CAPACITY    will use bit operations instead of modulo
TLBT_MAX_LOAD_FACTOR   float ]0,1[, lower means less time spent on collision resolution but more space wasted
                       resolution but more space wasted. default 0.7
TLBT_SIZE_T            default is size_t from <stddef.h>
TLBT_UINT32_T          default is uint32_t from <stdint.h>
TLBT_MAP_NO_ITERATOR   don't define an iterator struct and functions

=== memory ===
if TLBT_DYNAMIC_MEMORY is not defined, you have to provide the buffers to the init function. resizing won't work
if TLBT_DYNAMIC_MEMORY is defined, then memory is managed by the implementation

TLBT_MALLOC  if TLBT_DYNAMIC_MEMORY is defined. default is malloc from <stdlib.h>
TLBT_FREE    if TLBT_DYNAMIC_MEMORY is defined. default is free from <stdlib.h>

=== notes ===
when omitting the TLBT_VALUE_T definition all types and functions have the prefix tlbt_set_KEY instead of
tlbt_map_KEY_VALUE

some functions might be different or don't even exist like "get" due to no more values

for usage examples please check the test files in the test directory
*/

#define TLBT_COMBINE(a, b) a##b
#define TLBT_COMBINE2(a, b) TLBT_COMBINE(a, b)

#ifndef TLBT_KEY_T
#error "TLBT_KEY_T must be defined"
#endif

#ifndef TLBT_KEY_T_NAME
#define TLBT_KEY_T_NAME TLBT_KEY_T
#endif

#ifndef TLBT_VALUE_T_NAME
#define TLBT_VALUE_T_NAME TLBT_VALUE_T
#endif

#ifdef TLBT_BASE2_CAPACITY
#define TLBT_MOD(a, b) ((a) & ((b) - 1))
#else
#define TLBT_MOD(a, b) ((a) % (b))
#endif

#ifndef TLBT_MAX_LOAD_FACTOR
#define TLBT_MAX_LOAD_FACTOR (0.70)
#endif

#ifdef TLBT_DYNAMIC_MEMORY
#ifndef TLBT_MALLOC
#include <stdlib.h>
#define TLBT_MALLOC malloc
// memory is managed by this implementation so redefine free just in case
#undef TLBT_FREE
#define TLBT_FREE free
#endif
// if malloc is defined but free is not, then do nothing because it's in the
// responsibility of the user. it could be an arena allocator
// where free doesn't mean a lot
#endif

#ifndef TLBT_ASSERT
#include <assert.h>
#define TLBT_ASSERT assert
#endif

#ifndef TLBT_UINT32_T
#include <stdint.h>
#define TLBT_UINT32_T uint32_t
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
// if it's not defined and the C standard is big enough to use static assertions, check the size of the provided type
_Static_assert(sizeof(TLBT_UINT32_T) == 4, "TLBT_UINT32_T has to have 4 bytes");
#endif

#ifndef TLBT_SIZE_T
#include <stddef.h>
#define TLBT_SIZE_T size_t
#endif

#ifdef TLBT_VALUE_T
#define TLBT_MAP_TYPE TLBT_COMBINE2(tlbt_map_, TLBT_COMBINE2(TLBT_KEY_T_NAME, TLBT_COMBINE2(_, TLBT_VALUE_T_NAME)))
#else
#define TLBT_MAP_TYPE TLBT_COMBINE2(tlbt_set_, TLBT_KEY_T_NAME)
#endif

#define TLBT_MAP_KEY_TYPE TLBT_COMBINE2(TLBT_MAP_TYPE, TLBT_COMBINE2(_, key))
#define TLBT_MAP_FUNC(name) TLBT_COMBINE2(TLBT_MAP_TYPE, TLBT_COMBINE2(_, name))
#define TLBT_MAP_FUNC_INTERNAL(name) TLBT_COMBINE2(_, TLBT_COMBINE2(TLBT_MAP_TYPE, TLBT_COMBINE2(_, name)))

#ifndef TLBT_MAP_NO_ITERATOR

#ifdef TLBT_VALUE_T
#define TLBT_MAP_ITERATOR_TYPE                                                                                         \
  TLBT_COMBINE2(tlbt_map_iterator_, TLBT_COMBINE2(TLBT_KEY_T_NAME, TLBT_COMBINE2(_, TLBT_VALUE_T_NAME)))
#else
#define TLBT_MAP_ITERATOR_TYPE TLBT_COMBINE2(tlbt_set_iterator_, TLBT_KEY_T_NAME)
#endif

#define TLBT_MAP_ITERATOR_FUNC(name) TLBT_COMBINE2(TLBT_MAP_ITERATOR_TYPE, TLBT_COMBINE2(_, name))
#endif

#define TLBT_OCCUPIED_BIT (1U << 31)
#define TLBT_DELETED_BIT (1U << 30)
#define TLBT_INDEX_MASK (TLBT_DELETED_BIT - 1)
#define TLBT_IS_DELETED(flags) (((flags) & (TLBT_DELETED_BIT)) != 0)
#define TLBT_IS_OCCUPIED(flags) (((flags) & (TLBT_OCCUPIED_BIT)) != 0)

#ifdef TLBT_STATIC
#undef TLBT_DEFINITION
#define TLBT_DEFINITION
#undef TLBT_IMPLEMENTATION
#define TLBT_IMPLEMENTATION
#define TLBT_INLINE static inline
#else
#define TLBT_INLINE
#endif

#ifdef TLBT_DEFINITION

#include <stdbool.h>

typedef struct TLBT_MAP_KEY_TYPE {
  TLBT_KEY_T key;
  TLBT_UINT32_T index; // upper two bits are used for occupied and deleted states
} TLBT_MAP_KEY_TYPE;

typedef struct TLBT_MAP_TYPE {
  TLBT_MAP_KEY_TYPE *keys;
#ifdef TLBT_VALUE_T
  TLBT_VALUE_T *values;
#endif
  TLBT_SIZE_T capacity;
  TLBT_SIZE_T count;
} TLBT_MAP_TYPE;

#ifndef TLBT_MAP_NO_ITERATOR
typedef struct TLBT_MAP_ITERATOR_TYPE {
  TLBT_MAP_TYPE *map;
  TLBT_SIZE_T i;
} TLBT_MAP_ITERATOR_TYPE;
#endif

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_MAP_FUNC(create)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity);
TLBT_INLINE void TLBT_MAP_FUNC(destroy)(TLBT_MAP_TYPE *const m);
TLBT_INLINE void TLBT_MAP_FUNC(ensure_capacity)(TLBT_MAP_TYPE *const m);
TLBT_INLINE void TLBT_MAP_FUNC(adjust_capacity)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity);
#else

#ifdef TLBT_VALUE_T
TLBT_INLINE void TLBT_MAP_FUNC(init)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity, TLBT_MAP_KEY_TYPE *key_buffer,
                                     TLBT_VALUE_T *value_buffer);
#else
TLBT_INLINE void TLBT_MAP_FUNC(init)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity, TLBT_MAP_KEY_TYPE *key_buffer);
#endif

#endif

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(insert_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T value,
                                          TLBT_UINT32_T hash);
#else
TLBT_INLINE bool TLBT_MAP_FUNC(insert_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_UINT32_T hash);
#endif

TLBT_INLINE bool TLBT_MAP_FUNC(remove_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_UINT32_T hash);
TLBT_INLINE bool TLBT_MAP_FUNC(contains_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_UINT32_T hash);

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(get_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T *out, TLBT_UINT32_T hash);
#endif

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(insert)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T value);
#else
TLBT_INLINE bool TLBT_MAP_FUNC(insert)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key);
#endif

TLBT_INLINE bool TLBT_MAP_FUNC(remove)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key);
TLBT_INLINE bool TLBT_MAP_FUNC(contains)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key);

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(get)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T *out);
#endif

TLBT_INLINE void TLBT_MAP_FUNC(clear)(TLBT_MAP_TYPE *const m);
TLBT_INLINE bool TLBT_MAP_FUNC(copy)(TLBT_MAP_TYPE *const dest, const TLBT_MAP_TYPE *const src);

#ifndef TLBT_MAP_NO_ITERATOR

static inline void TLBT_MAP_ITERATOR_FUNC(init)(TLBT_MAP_ITERATOR_TYPE *const iter, TLBT_MAP_TYPE *const m) {
  iter->i = 0;
  iter->map = m;
}

static inline void TLBT_MAP_ITERATOR_FUNC(reset)(TLBT_MAP_ITERATOR_TYPE *const iter) {
  iter->i = 0;
}

#ifdef TLBT_VALUE_T
static inline bool TLBT_MAP_ITERATOR_FUNC(iterate)(TLBT_MAP_ITERATOR_TYPE *const iter, TLBT_KEY_T **out_key,
                                                   TLBT_VALUE_T **out_value) {
#else
static inline bool TLBT_MAP_ITERATOR_FUNC(iterate)(TLBT_MAP_ITERATOR_TYPE *const iter, TLBT_KEY_T **out_key) {
#endif
  TLBT_MAP_TYPE *m = iter->map;

  // this function is iterating over the whole capacity even if there is only one element in the map because I don't
  // know where the elements are at. I could count the elements found inside the iterator and break early once I have
  // found the amount of elements specified by the map.count field but technically they should be evenly spread so while
  // searching for the elements you will probably go through almost all of the elements anyway. if iterating all
  // elements is a very common operation, then I should consider adding another field to every key specifying the gap to
  // the next element. this makes inserting slower because I have to search for the previous element but
  // it would speed up iterating a lot. this could even be controlled by a macro but for now this is fine.
  // most of the time I only iterate once before destroying/resetting the whole map. inserts happen more often so even
  // if there was a speed gain on the iteration, the speed loss on the insertions would accumulate to more than is
  // gained through the iteration speed for most cases. rambling over
  while (iter->i < m->capacity && !(m->keys[iter->i].index & TLBT_OCCUPIED_BIT))
    ++iter->i;

  if (iter->i >= m->capacity)
    return false;

  *out_key = &m->keys[iter->i].key;
#ifdef TLBT_VALUE_T
  *out_value = &m->values[iter->i];
#endif
  ++iter->i;

  return true;
}

#endif

#endif

#ifdef TLBT_IMPLEMENTATION

#ifndef TLBT_HASH_FUNC
#error "TLBT_HASH_FUNC must be defined"
#endif

#ifndef TLBT_EQUALS_FUNC
#error "TLBT_EQUALS_FUNC must be defined"
#endif

TLBT_INLINE bool TLBT_MAP_FUNC_INTERNAL(find_entry)(TLBT_MAP_KEY_TYPE *keys, TLBT_SIZE_T capacity, TLBT_KEY_T key,
                                                    TLBT_UINT32_T hash, TLBT_SIZE_T *out_index) {
  TLBT_SIZE_T i = TLBT_MOD(hash, capacity);
  const TLBT_SIZE_T start = i;
  for (;;) {
    TLBT_MAP_KEY_TYPE *entry = &keys[i];
    if (!TLBT_IS_OCCUPIED(entry->index) && !TLBT_IS_DELETED(entry->index)) {
      return false;
    } else if (TLBT_EQUALS_FUNC(entry->key, key)) {
      *out_index = i;
      return true;
    } else {
      // deleted or occupied and not the same
      // if an entry was deleted we still have to continue searching because it might have been inserted after that
      i = TLBT_MOD(i + 1, capacity);
      if (i == start)
        return false;
    }
  }
  // should never reach
  return false;
}

TLBT_INLINE TLBT_SIZE_T TLBT_MAP_FUNC_INTERNAL(find_empty)(TLBT_MAP_KEY_TYPE *keys, TLBT_SIZE_T capacity,
                                                           TLBT_UINT32_T hash) {
  TLBT_SIZE_T i = TLBT_MOD(hash, capacity);
  for (;;) {
    TLBT_MAP_KEY_TYPE *entry = &keys[i];
    if (!TLBT_IS_OCCUPIED(entry->index) || TLBT_IS_DELETED(entry->index)) {
      return i;
    }
    i = TLBT_MOD(i + 1, capacity);
  }
  // should never reach
  return 0;
}

#ifdef TLBT_DYNAMIC_MEMORY

TLBT_INLINE void TLBT_MAP_FUNC(create)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity) {
  m->capacity = capacity;
#ifdef TLBT_BASE2_CAPACITY
  TLBT_ASSERT(capacity / 2.0f == (float)(int)(capacity / 2.0f));
#endif
  m->keys = TLBT_MALLOC(sizeof(TLBT_MAP_KEY_TYPE) * capacity);
  memset(m->keys, 0, sizeof(TLBT_MAP_KEY_TYPE) * capacity);
#ifdef TLBT_VALUE_T
  m->values = TLBT_MALLOC(sizeof(TLBT_VALUE_T) * capacity);
#endif
  m->count = 0;
}

TLBT_INLINE void TLBT_MAP_FUNC(destroy)(TLBT_MAP_TYPE *const m) {
  TLBT_FREE(m->keys);
#ifdef TLBT_VALUE_T
  TLBT_FREE(m->values);
#endif
}

TLBT_INLINE void TLBT_MAP_FUNC(ensure_capacity)(TLBT_MAP_TYPE *const m) {
  if (m->count + 1 > (float)m->capacity * TLBT_MAX_LOAD_FACTOR) {
    const TLBT_SIZE_T capacity = m->capacity * 2;
    TLBT_MAP_FUNC(adjust_capacity)(m, capacity);
  }
}

TLBT_INLINE void TLBT_MAP_FUNC(adjust_capacity)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity) {
  TLBT_MAP_KEY_TYPE *keys = (TLBT_MAP_KEY_TYPE *)TLBT_MALLOC(sizeof(TLBT_MAP_KEY_TYPE) * capacity);
#ifdef TLBT_VALUE_T
  TLBT_VALUE_T *values = (TLBT_VALUE_T *)TLBT_MALLOC(sizeof(TLBT_VALUE_T) * capacity);
#endif

  for (TLBT_SIZE_T i = 0; i < capacity; ++i)
    keys[i].index = 0;

  TLBT_UINT32_T hash = 0;
  TLBT_SIZE_T j = 0;
  for (TLBT_SIZE_T i = 0; i < m->capacity; ++i) {
    if (TLBT_IS_OCCUPIED(m->keys[i].index)) {
      hash = TLBT_HASH_FUNC(m->keys[i].key);
      j = TLBT_MAP_FUNC_INTERNAL(find_empty)(keys, capacity, hash);
      keys[j].key = m->keys[i].key;
      keys[j].index = (m->keys[i].index & TLBT_INDEX_MASK) | TLBT_OCCUPIED_BIT;
#ifdef TLBT_VALUE_T
      values[j] = m->values[i];
#endif
    }
  }

  TLBT_FREE(m->keys);
  m->keys = keys;
#ifdef TLBT_VALUE_T
  TLBT_FREE(m->values);
  m->values = values;
#endif
  m->capacity = capacity;
}

#else

#ifdef TLBT_VALUE_T
TLBT_INLINE void TLBT_MAP_FUNC(init)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity, TLBT_MAP_KEY_TYPE *key_buffer,
                                     TLBT_VALUE_T *value_buffer) {
  m->values = value_buffer;
#else
TLBT_INLINE void TLBT_MAP_FUNC(init)(TLBT_MAP_TYPE *const m, TLBT_SIZE_T capacity, TLBT_MAP_KEY_TYPE *key_buffer) {
#endif
  m->keys = key_buffer;
  memset(m->keys, 0, sizeof(TLBT_MAP_KEY_TYPE) * capacity);
  m->capacity = capacity;
#ifdef TLBT_BASE2_CAPACITY
  TLBT_ASSERT(capacity / 2.0f == (float)(int)(capacity / 2.0f));
#endif
  m->count = 0;
}
#endif

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(insert_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T value,
                                          TLBT_UINT32_T hash) {
#else
TLBT_INLINE bool TLBT_MAP_FUNC(insert_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_UINT32_T hash) {
#endif
#ifdef TLBT_DYNAMIC_MEMORY
  TLBT_MAP_FUNC(ensure_capacity)(m);
#else
  if (m->count + 1 > (float)m->capacity * TLBT_MAX_LOAD_FACTOR)
    return false;
#endif

  // not finding an empty slot will not happen because the function will either auto resize
  // once the load factor is reached or it will return false before that happens
  // depending on the memory mode
  TLBT_SIZE_T i = TLBT_MAP_FUNC_INTERNAL(find_empty)(m->keys, m->capacity, hash);

  m->keys[i].key = key;
  m->keys[i].index = (i & TLBT_INDEX_MASK) | TLBT_OCCUPIED_BIT;
#ifdef TLBT_VALUE_T
  m->values[i] = value;
#endif
  ++m->count;
  return true;
}

TLBT_INLINE bool TLBT_MAP_FUNC(remove_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_UINT32_T hash) {
  if (m->count == 0)
    return false;

  TLBT_SIZE_T i = 0;
  if (TLBT_MAP_FUNC_INTERNAL(find_entry)(m->keys, m->capacity, key, hash, &i)) {
    m->keys[i].index = TLBT_DELETED_BIT;
    --m->count;
    return true;
  }
  return false;
}

TLBT_INLINE bool TLBT_MAP_FUNC(contains_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_UINT32_T hash) {
  if (m->count == 0)
    return false;
  TLBT_SIZE_T i = 0;
  return TLBT_MAP_FUNC_INTERNAL(find_entry)(m->keys, m->capacity, key, hash, &i);
}

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(get_ph)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T *out, TLBT_UINT32_T hash) {
  if (m->count == 0)
    return false;

  TLBT_SIZE_T i = 0;
  if (TLBT_MAP_FUNC_INTERNAL(find_entry)(m->keys, m->capacity, key, hash, &i)) {
    *out = m->values[i];
    return true;
  }
  return false;
}
#endif

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(insert)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T value) {
  return TLBT_MAP_FUNC(insert_ph)(m, key, value, TLBT_HASH_FUNC(key));
}
#else
TLBT_INLINE bool TLBT_MAP_FUNC(insert)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key) {
  return TLBT_MAP_FUNC(insert_ph)(m, key, TLBT_HASH_FUNC(key));
}
#endif

TLBT_INLINE bool TLBT_MAP_FUNC(remove)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key) {
  return TLBT_MAP_FUNC(remove_ph)(m, key, TLBT_HASH_FUNC(key));
}

TLBT_INLINE bool TLBT_MAP_FUNC(contains)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key) {
  return TLBT_MAP_FUNC(contains_ph)(m, key, TLBT_HASH_FUNC(key));
}

#ifdef TLBT_VALUE_T
TLBT_INLINE bool TLBT_MAP_FUNC(get)(TLBT_MAP_TYPE *const m, TLBT_KEY_T key, TLBT_VALUE_T *out) {
  return TLBT_MAP_FUNC(get_ph)(m, key, out, TLBT_HASH_FUNC(key));
}
#endif

TLBT_INLINE void TLBT_MAP_FUNC(clear)(TLBT_MAP_TYPE *const m) {
  m->count = 0;
  for (TLBT_SIZE_T i = 0; i < m->capacity; ++i)
    m->keys[i].index = 0;
}

TLBT_INLINE bool TLBT_MAP_FUNC(copy)(TLBT_MAP_TYPE *const dest, const TLBT_MAP_TYPE *const src) {
  if (dest->capacity != src->capacity) {
    if ((dest->capacity * TLBT_MAX_LOAD_FACTOR) <= src->count)
      return false;

    TLBT_MAP_FUNC(clear)(dest);
    for (TLBT_SIZE_T i = 0; i < src->capacity; ++i) {
      if (TLBT_IS_OCCUPIED(src->keys[i].index)) {
#ifdef TLBT_VALUE_T
        TLBT_MAP_FUNC(insert)(dest, src->keys[i].key, src->values[i]);
#else
        TLBT_MAP_FUNC(insert)(dest, src->keys[i].key);
#endif
      }
    }
  } else {
    for (TLBT_SIZE_T i = 0; i < src->capacity; ++i) {
      dest->keys[i] = src->keys[i];
#ifdef TLBT_VALUE_T
      dest->values[i] = src->values[i];
#endif
    }
  }
  dest->count = src->count;
  return true;
}

#endif

#undef TLBT_ASSERT
#undef TLBT_BASE2_CAPACITY
#undef TLBT_COMBINE
#undef TLBT_COMBINE2
#undef TLBT_DEFINITION
#undef TLBT_DELETED_BIT
#undef TLBT_DYNAMIC_MEMORY
#undef TLBT_EQUALS_FUNC
#undef TLBT_FREE
#undef TLBT_HASH_FUNC
#undef TLBT_IMPLEMENTATION
#undef TLBT_INDEX_MASK
#undef TLBT_INLINE
#undef TLBT_IS_DELETED
#undef TLBT_IS_OCCUPIED
#undef TLBT_KEY_T
#undef TLBT_KEY_T_NAME
#undef TLBT_MALLOC
#undef TLBT_MAP_FUNC
#undef TLBT_MAP_FUNC_INTERNAL
#undef TLBT_MAP_ITERATOR_FUNC
#undef TLBT_MAP_ITERATOR_TYPE
#undef TLBT_MAP_KEY_TYPE
#undef TLBT_MAP_NO_ITERATOR
#undef TLBT_MAP_TYPE
#undef TLBT_MAX_LOAD_FACTOR
#undef TLBT_MOD
#undef TLBT_OCCUPIED_BIT
#undef TLBT_SIZE_T
#undef TLBT_STATIC
#undef TLBT_UINT32_T
#undef TLBT_VALUE_T
#undef TLBT_VALUE_T_NAME

