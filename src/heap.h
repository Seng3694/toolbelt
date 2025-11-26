/*
types:
- tlbt_SORT_heap_TYPE          heap type. TYPE and SORT depend on your definition. SORT can be either `min` or `max`

functions:
- tlbt_SORT_heap_TYPE_push     pushes an item onto the heap
- tlbt_SORT_heap_TYPE_peek     peeks the first item of the heap
- tlbt_SORT_heap_TYPE_pop      pops the first item of the heap
- tlbt_SORT_heap_TYPE_copy     copies the src heap to the dest heap
- tlbt_SORT_heap_TYPE_clear    resets the heap
if TLBT_DYNAMIC_MEMORY is not defined
- tlbt_SORT_heap_TYPE_init     initializes the heap with the given buffer
- tlbt_SORT_heap_TYPE_build    initializes the heap with the given buffer and heap sorts the values in it
if TLBT_DYNAMIC_MEMORY is defined
- tlbt_SORT_heap_TYPE_create   creates the heap with allocations
- tlbt_SORT_heap_TYPE_destroy  destroys the heap and frees memory
- tlbt_SORT_heap_TYPE_build    creates the heap with allocations, copies the buffer and heap sorts the internal copy
- tlbt_SORT_heap_TYPE_move     initializes the heap and takes responsibility of the given buffer and heap sorts it

TLBT_DEFINITION      if you want to define the types and functions in a header file
TLBT_IMPLEMENTATION  for the corresponding implementation of the definitions in a separate source file
TLBT_STATIC          if you want to define and implement them statically in a source file

=== required definitions ===
TLBT_T               the heap type
TLBT_COMPARE         function for comparing two items of TYPE

=== optional definitions ===
TLBT_T_NAME      default is TLBT_T
TLBT_MAX_HEAP    default is a min heap (tlbt_min_heap_TYPE)
TLBT_ASSERT      default is assert from <assert.h>
TLBT_MEMCPY      default is memcpy from <string.h>
TLBT_SIZE_T      default is size_t from <stddef.h>

=== memory ===
if TLBT_DYNAMIC_MEMORY is not defined, you have to provide the buffers to the init function. resizing won't work
if TLBT_DYNAMIC_MEMORY is defined, then memory is managed by the implementation

TLBT_MALLOC  if TLBT_DYNAMIC_MEMORY is defined. default is malloc from <stdlib.h>
TLBT_FREE    if TLBT_DYNAMIC_MEMORY is defined. default is free from <stdlib.h>
*/

#define TLBT_COMBINE(a, b) a##b
#define TLBT_COMBINE2(a, b) TLBT_COMBINE(a, b)

#ifndef TLBT_T
#error "TLBT_T must be defined"
#endif

#ifndef TLBT_COMPARE
#error "TLBT_COMPARE must be defined"
#endif

#ifndef TLBT_T_NAME
#define TLBT_T_NAME TLBT_T
#endif

#ifdef TLBT_MIN_HEAP
#undef TLBT_MIN_HEAP /* it's default */
#endif

#ifdef TLBT_MAX_HEAP
#define TLBT_HEAP_SORT_TYPE max
#else /* default to min heap */
#define TLBT_HEAP_SORT_TYPE min
#endif

#define TLBT_HEAP_PARENT(i) (((i) - 1) / 2)
#define TLBT_HEAP_LEFT_CHILD(i) (2 * (i) + 1)
#define TLBT_HEAP_RIGHT_CHILD(i) (2 * (i) + 2)

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

#ifndef TLBT_SIZE_T
#include <stddef.h>
#define TLBT_SIZE_T size_t
#endif

#ifndef TLBT_MEMCPY
#include <string.h>
#define TLBT_MEMCPY memcpy
#endif

#define TLBT_HEAP_TYPE TLBT_COMBINE2(tlbt_, TLBT_COMBINE2(TLBT_HEAP_SORT_TYPE, TLBT_COMBINE2(_heap_, TLBT_T_NAME)))
#define TLBT_HEAP_FUNC(name) TLBT_COMBINE2(TLBT_HEAP_TYPE, TLBT_COMBINE2(_, name))
#define TLBT_HEAP_FUNC_INTERNAL(name) TLBT_COMBINE2(_, TLBT_HEAP_FUNC(name))

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

typedef struct TLBT_HEAP_TYPE {
  TLBT_SIZE_T capacity;
  TLBT_SIZE_T count;
  TLBT_T *data;
} TLBT_HEAP_TYPE;

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_HEAP_FUNC(create)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity);
TLBT_INLINE void TLBT_HEAP_FUNC(build)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T count, const TLBT_T *const buffer);
TLBT_INLINE void TLBT_HEAP_FUNC(move)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity, TLBT_SIZE_T count, TLBT_T *buffer);
TLBT_INLINE void TLBT_HEAP_FUNC(destroy)(TLBT_HEAP_TYPE *const h);
TLBT_INLINE void TLBT_HEAP_FUNC(push)(TLBT_HEAP_TYPE *const h, TLBT_T item);
TLBT_INLINE void TLBT_HEAP_FUNC(copy)(TLBT_HEAP_TYPE *const dest, const TLBT_HEAP_TYPE *const src);
TLBT_INLINE void TLBT_HEAP_FUNC_INTERNAL(ensure_capacity)(TLBT_HEAP_TYPE *const h, const TLBT_SIZE_T capacity);
#else
TLBT_INLINE void TLBT_HEAP_FUNC(init)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity, TLBT_T *buffer);
TLBT_INLINE void TLBT_HEAP_FUNC(build)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity, TLBT_SIZE_T count,
                                       TLBT_T *buffer);
TLBT_INLINE bool TLBT_HEAP_FUNC(push)(TLBT_HEAP_TYPE *const h, TLBT_T item);
TLBT_INLINE bool TLBT_HEAP_FUNC(copy)(TLBT_HEAP_TYPE *const dest, const TLBT_HEAP_TYPE *const src);
#endif

TLBT_INLINE bool TLBT_HEAP_FUNC(pop)(TLBT_HEAP_TYPE *const h);
TLBT_INLINE void TLBT_HEAP_FUNC_INTERNAL(bubble_up)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T i);
TLBT_INLINE void TLBT_HEAP_FUNC_INTERNAL(bubble_down)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T i);

static inline bool TLBT_HEAP_FUNC(peek)(TLBT_HEAP_TYPE *const h, TLBT_T *const out) {
  if (h->count == 0)
    return false;
  *out = h->data[0];
  return true;
}

static inline void TLBT_HEAP_FUNC_INTERNAL(swap)(TLBT_T *const a, TLBT_T *const b) {
  const TLBT_T tmp = *a;
  *a = *b;
  *b = tmp;
}

static inline void TLBT_HEAP_FUNC(clear)(TLBT_HEAP_TYPE *const h) {
  h->count = 0;
}

#endif

#ifdef TLBT_IMPLEMENTATION

#ifdef TLBT_DYNAMIC_MEMORY

TLBT_INLINE void TLBT_HEAP_FUNC(create)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity) {
  h->capacity = capacity;
  h->count = 0;
  h->data = TLBT_MALLOC(capacity * sizeof(TLBT_T));
  TLBT_ASSERT(h->data);
}

TLBT_INLINE void TLBT_HEAP_FUNC(build)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T count, const TLBT_T *const buffer) {
  h->count = count;
  h->capacity = count;
  h->data = TLBT_MALLOC(count * sizeof(TLBT_T));
  TLBT_MEMCPY(h->data, buffer, sizeof(TLBT_T) * count);
  for (long long int i = ((long long int)count / 2) - 1; i >= 0; --i) {
    TLBT_HEAP_FUNC_INTERNAL(bubble_down)(h, i);
  }
}

TLBT_INLINE void TLBT_HEAP_FUNC(move)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity, TLBT_SIZE_T count,
                                      TLBT_T *buffer) {
  h->capacity = capacity;
  h->count = count;
  h->data = buffer;
  TLBT_ASSERT(h->data);
  for (long long int i = ((long long int)count / 2) - 1; i >= 0; --i) {
    TLBT_HEAP_FUNC_INTERNAL(bubble_down)(h, i);
  }
}

TLBT_INLINE void TLBT_HEAP_FUNC(destroy)(TLBT_HEAP_TYPE *const h) {
  TLBT_ASSERT(h->data);
  TLBT_FREE(h->data);
}

TLBT_INLINE void TLBT_HEAP_FUNC_INTERNAL(ensure_capacity)(TLBT_HEAP_TYPE *const h, const TLBT_SIZE_T capacity) {
  if (h->capacity < capacity) {
    while (h->capacity < capacity)
      h->capacity *= 2;
    TLBT_T *new_data = TLBT_MALLOC(h->capacity * sizeof(TLBT_T));
    TLBT_ASSERT(new_data);
    TLBT_MEMCPY(new_data, h->data, h->count * sizeof(TLBT_T));
    TLBT_FREE(h->data);
    h->data = new_data;
  }
}

#else

TLBT_INLINE void TLBT_HEAP_FUNC(init)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity, TLBT_T *buffer) {
  h->count = 0;
  h->capacity = capacity;
  h->data = buffer;
}

TLBT_INLINE void TLBT_HEAP_FUNC(build)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T capacity, TLBT_SIZE_T count,
                                       TLBT_T *buffer) {
  h->count = count;
  h->capacity = capacity;
  h->data = buffer;
  for (long long int i = ((long long int)count / 2) - 1; i >= 0; --i) {
    TLBT_HEAP_FUNC_INTERNAL(bubble_down)(h, i);
  }
}

#endif

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_HEAP_FUNC(copy)(TLBT_HEAP_TYPE *const dest, const TLBT_HEAP_TYPE *const src) {
  TLBT_HEAP_FUNC_INTERNAL(ensure_capacity)(dest, src->count);
#else
TLBT_INLINE bool TLBT_HEAP_FUNC(copy)(TLBT_HEAP_TYPE *const dest, const TLBT_HEAP_TYPE *const src) {
  if (dest->capacity < src->count)
    return false;
#endif

  dest->count = src->count;
  TLBT_MEMCPY(dest->data, src->data, sizeof(TLBT_T) * src->count);

#ifndef TLBT_DYNAMIC_MEMORY
  return true;
#endif
}

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_HEAP_FUNC(push)(TLBT_HEAP_TYPE *const h, TLBT_T item) {
  TLBT_HEAP_FUNC_INTERNAL(ensure_capacity)(h, h->count + 1);
#else
TLBT_INLINE bool TLBT_HEAP_FUNC(push)(TLBT_HEAP_TYPE *const h, TLBT_T item) {
  if (h->count + 1 > h->capacity)
    return false;
#endif

  h->data[h->count++] = item;
  TLBT_HEAP_FUNC_INTERNAL(bubble_up)(h, h->count - 1);

#ifndef TLBT_DYNAMIC_MEMORY
  return true;
#endif
}

TLBT_INLINE bool TLBT_HEAP_FUNC(pop)(TLBT_HEAP_TYPE *const h) {
  if (h->count == 0)
    return false;

  --h->count;
  h->data[0] = h->data[h->count];
  TLBT_HEAP_FUNC_INTERNAL(bubble_down)(h, 0);

  return true;
}

TLBT_INLINE void TLBT_HEAP_FUNC_INTERNAL(bubble_up)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T i) {
  while (i > 0) {
    TLBT_SIZE_T parent = TLBT_HEAP_PARENT(i);

#ifdef TLBT_MAX_HEAP
    if (TLBT_COMPARE(h->data[parent], h->data[i]) >= 0) {
#else
    if (TLBT_COMPARE(h->data[parent], h->data[i]) <= 0) {
#endif
      break;
    }

    TLBT_HEAP_FUNC_INTERNAL(swap)(&h->data[parent], &h->data[i]);
    i = parent;
  }
}

TLBT_INLINE void TLBT_HEAP_FUNC_INTERNAL(bubble_down)(TLBT_HEAP_TYPE *const h, TLBT_SIZE_T i) {
  for (;;) {
    TLBT_SIZE_T left = TLBT_HEAP_LEFT_CHILD(i);
    TLBT_SIZE_T right = TLBT_HEAP_RIGHT_CHILD(i);
    TLBT_SIZE_T best = i;

#ifdef TLBT_MAX_HEAP
    if (left < h->count && TLBT_COMPARE(h->data[left], h->data[best]) > 0) {
      best = left;
    }
    if (right < h->count && TLBT_COMPARE(h->data[right], h->data[best]) > 0) {
      best = right;
    }
#else
    if (left < h->count && TLBT_COMPARE(h->data[left], h->data[best]) < 0) {
      best = left;
    }
    if (right < h->count && TLBT_COMPARE(h->data[right], h->data[best]) < 0) {
      best = right;
    }
#endif

    if (best == i) {
      break;
    }

    TLBT_HEAP_FUNC_INTERNAL(swap)(&h->data[best], &h->data[i]);
    i = best;
  }
}

#endif

#undef TLBT_ASSERT
#undef TLBT_COMBINE
#undef TLBT_COMBINE2
#undef TLBT_COMPARE
#undef TLBT_DEFINITION
#undef TLBT_DYNAMIC_MEMORY
#undef TLBT_FREE
#undef TLBT_HEAP_FUNC
#undef TLBT_HEAP_FUNC_INTERNAL
#undef TLBT_HEAP_LEFT_CHILD
#undef TLBT_HEAP_PARENT
#undef TLBT_HEAP_RIGHT_CHILD
#undef TLBT_HEAP_SORT_TYPE
#undef TLBT_HEAP_TYPE
#undef TLBT_IMPLEMENTATION
#undef TLBT_INLINE
#undef TLBT_MALLOC
#undef TLBT_MAX_HEAP
#undef TLBT_MEMCPY
#undef TLBT_MIN_HEAP
#undef TLBT_SIZE_T
#undef TLBT_STATIC
#undef TLBT_T
#undef TLBT_T_NAME

