/*
types:
- tlbt_deque_TYPE             deque type. TYPE depends on your definition
- tlbt_deque_iterator_TYPE    deque iterator type

functions:
- tlbt_deque_TYPE_push_back        pushes an item to the back of the deque
- tlbt_deque_TYPE_push_front       pushes an item to the front of the deque
- tlbt_deque_TYPE_peek_back        peeks the item at the back of the deque
- tlbt_deque_TYPE_peek_front       peeks the item at the front of the deque
- tlbt_deque_TYPE_pop_back         pops the item at the back of the deque
- tlbt_deque_TYPE_pop_front        pops the item at the front of the deque
- tlbt_deque_TYPE_at               returns a pointer to the item at the given index (or NULL)
- tlbt_deque_TYPE_copy             copies the src deque to the dest deque
- tlbt_deque_TYPE_clear            resets the deque
- tlbt_deque_iterator_TYPE_init    initializes the iterator
- tlbt_deque_iterator_TYPE_reset   resets the iterator
- tlbt_deque_iterator_TYPE_iterate iterates the deque
if TLBT_DYNAMIC_MEMORY is not defined
- tlbt_deque_TYPE_init             initializes the deque with given buffer
if TLBT_DYNAMIC_MEMORY is defined
- tlbt_deque_TYPE_create           creates the deque with allocations
- tlbt_deque_TYPE_destroy          destroys the deque and frees memory
- tlbt_deque_TYPE_ensure_capacity  checks if resizing is necessary and resizes by factor 2 if it is

TLBT_DEFINITION      if you want to define the types and functions in a header file
TLBT_IMPLEMENTATION  for the corresponding implementation of the definitions in a separate source file
TLBT_STATIC          if you want to define and implement them statically in a source file

=== required definitions ===
TLBT_T                 the deque key type

=== optional definitions ===
TLBT_T_NAME            default is TLBT_T
TLBT_ASSERT            default is assert from <assert.h>
TLBT_BASE2_CAPACITY    will use bit operations instead of modulo
TLBT_SIZE_T            default is size_t from <stddef.h>
TLBT_DEQUE_NO_ITERATOR don't define an iterator struct and functions

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

#ifndef TLBT_T_NAME
#define TLBT_T_NAME TLBT_T
#endif

#ifdef TLBT_BASE2_CAPACITY
#define TLBT_MOD(a, b) ((a) & ((b) - 1))
#else
#define TLBT_MOD(a, b) ((a) % (b))
#endif

#define TLBT_DEQUE_INC_WRAP(n, s) (TLBT_MOD(((n) + 1), (s)))
#define TLBT_DEQUE_DEC_WRAP(n, s) ((n) == 0 ? (s) - 1 : (n) - 1)

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

#define TLBT_DEQUE_TYPE TLBT_COMBINE2(tlbt_deque_, TLBT_T_NAME)
#define TLBT_DEQUE_FUNC(name) TLBT_COMBINE2(TLBT_DEQUE_TYPE, TLBT_COMBINE2(_, name))

#ifndef TLBT_DEQUE_NO_ITERATOR
#define TLBT_DEQUE_ITERATOR_TYPE TLBT_COMBINE2(tlbt_deque_iterator_, TLBT_T_NAME)
#define TLBT_DEQUE_ITERATOR_FUNC(name) TLBT_COMBINE2(TLBT_DEQUE_ITERATOR_TYPE, TLBT_COMBINE2(_, name))
#endif

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

typedef struct TLBT_DEQUE_TYPE {
  TLBT_SIZE_T capacity;
  TLBT_SIZE_T count;
  TLBT_SIZE_T head;
  TLBT_SIZE_T tail;
  TLBT_T *data;
} TLBT_DEQUE_TYPE;

#ifndef TLBT_DEQUE_NO_ITERATOR
typedef struct TLBT_DEQUE_ITERATOR_TYPE {
  TLBT_DEQUE_TYPE *deque;
  TLBT_SIZE_T i;
} TLBT_DEQUE_ITERATOR_TYPE;
#endif

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_DEQUE_FUNC(create)(TLBT_DEQUE_TYPE *const d, TLBT_SIZE_T capacity);
TLBT_INLINE void TLBT_DEQUE_FUNC(destroy)(TLBT_DEQUE_TYPE *const d);
TLBT_INLINE void TLBT_DEQUE_FUNC(ensure_capacity)(TLBT_DEQUE_TYPE *const d, const TLBT_SIZE_T capacity);
TLBT_INLINE void TLBT_DEQUE_FUNC(push_front)(TLBT_DEQUE_TYPE *const d, TLBT_T item);
TLBT_INLINE void TLBT_DEQUE_FUNC(push_back)(TLBT_DEQUE_TYPE *const d, TLBT_T item);
TLBT_INLINE void TLBT_DEQUE_FUNC(copy)(TLBT_DEQUE_TYPE *const dest, const TLBT_DEQUE_TYPE *const src);
#else
TLBT_INLINE void TLBT_DEQUE_FUNC(init)(TLBT_DEQUE_TYPE *const d, TLBT_SIZE_T capacity, TLBT_T *buffer);
TLBT_INLINE bool TLBT_DEQUE_FUNC(push_front)(TLBT_DEQUE_TYPE *const d, TLBT_T item);
TLBT_INLINE bool TLBT_DEQUE_FUNC(push_back)(TLBT_DEQUE_TYPE *const d, TLBT_T item);
TLBT_INLINE bool TLBT_DEQUE_FUNC(copy)(TLBT_DEQUE_TYPE *const dest, const TLBT_DEQUE_TYPE *const src);
#endif

TLBT_INLINE bool TLBT_DEQUE_FUNC(pop_front)(TLBT_DEQUE_TYPE *const d);
TLBT_INLINE bool TLBT_DEQUE_FUNC(pop_back)(TLBT_DEQUE_TYPE *const d);

TLBT_INLINE TLBT_T *TLBT_DEQUE_FUNC(peek_front)(TLBT_DEQUE_TYPE *const d);
TLBT_INLINE TLBT_T *TLBT_DEQUE_FUNC(peek_back)(TLBT_DEQUE_TYPE *const d);
TLBT_INLINE TLBT_T *TLBT_DEQUE_FUNC(at)(TLBT_DEQUE_TYPE *const d, const TLBT_SIZE_T index);

static inline void TLBT_DEQUE_FUNC(clear)(TLBT_DEQUE_TYPE *const d) {
  d->count = 0;
  d->head = 0;
  d->tail = 0;
}

#ifndef TLBT_DEQUE_NO_ITERATOR

static inline void TLBT_DEQUE_ITERATOR_FUNC(init)(TLBT_DEQUE_ITERATOR_TYPE *const iter, TLBT_DEQUE_TYPE *const d) {
  iter->i = 0;
  iter->deque = d;
}

static inline void TLBT_DEQUE_ITERATOR_FUNC(reset)(TLBT_DEQUE_ITERATOR_TYPE *const iter) {
  iter->i = 0;
}

static inline bool TLBT_DEQUE_ITERATOR_FUNC(iterate)(TLBT_DEQUE_ITERATOR_TYPE *const iter, TLBT_T *out) {
  TLBT_DEQUE_TYPE *d = iter->deque;
  if (iter->i == d->count)
    return false;
  *out = *TLBT_DEQUE_FUNC(at)(d, iter->i);
  ++iter->i;
  return true;
}

#endif

#endif

#ifdef TLBT_IMPLEMENTATION

#ifdef TLBT_DYNAMIC_MEMORY

TLBT_INLINE void TLBT_DEQUE_FUNC(create)(TLBT_DEQUE_TYPE *const d, TLBT_SIZE_T capacity) {
  d->data = TLBT_MALLOC(sizeof(TLBT_T) * capacity);
  TLBT_ASSERT(d->data);
  d->capacity = capacity;
  d->count = 0;
  d->head = 0;
  d->tail = 0;
}

TLBT_INLINE void TLBT_DEQUE_FUNC(destroy)(TLBT_DEQUE_TYPE *const d) {
  TLBT_FREE(d->data);
}

TLBT_INLINE void TLBT_DEQUE_FUNC(ensure_capacity)(TLBT_DEQUE_TYPE *const d, const TLBT_SIZE_T capacity) {
  if (d->capacity < capacity) {
    TLBT_SIZE_T new_capacity = d->capacity;
    while (new_capacity < capacity)
      new_capacity *= 2;

    if (d->head >= d->tail) {
      // example:
      //                    T   H
      // old (size 5): [4] [5] [1] [2] [3]
      //                H               T
      // new (size 8): [1] [2] [3] [4] [5] [-] [-] [-]
      TLBT_T *new_data = TLBT_MALLOC(sizeof(TLBT_T) * new_capacity);
      // copy [1] [2] [3]
      memcpy(new_data, d->data + d->head, (d->capacity - d->head) * sizeof(TLBT_T));
      // copy [4] [5]
      memcpy(new_data + d->capacity - d->head, d->data, d->tail * sizeof(TLBT_T));
      d->tail = d->capacity - d->head + d->tail;
      d->head = 0;
      TLBT_FREE(d->data);
      d->data = new_data;
    } else {
      TLBT_T *new_data = TLBT_MALLOC(sizeof(TLBT_T) * new_capacity);
      memcpy(new_data, d->data, d->capacity * sizeof(TLBT_T));
      TLBT_FREE(d->data);
      d->data = new_data;
    }
    d->capacity = new_capacity;
  }
}

#else
TLBT_INLINE void TLBT_DEQUE_FUNC(init)(TLBT_DEQUE_TYPE *const d, TLBT_SIZE_T capacity, TLBT_T *buffer) {
  d->capacity = capacity;
  d->count = 0;
  d->head = 0;
  d->tail = 0;
  d->data = buffer;
#ifdef TLBT_BASE2_CAPACITY
  TLBT_ASSERT(capacity / 2.0f == (float)(int)(capacity / 2.0f));
#endif
}
#endif

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_DEQUE_FUNC(push_front)(TLBT_DEQUE_TYPE *const d, TLBT_T item) {
  TLBT_DEQUE_FUNC(ensure_capacity)(d, d->count + 1);
#else
TLBT_INLINE bool TLBT_DEQUE_FUNC(push_front)(TLBT_DEQUE_TYPE *const d, TLBT_T item) {
  if (d->count + 1 > d->capacity)
    return false;
#endif

  d->head = TLBT_DEQUE_DEC_WRAP(d->head, d->capacity);
  d->data[d->head] = item;
  ++d->count;

#ifndef TLBT_DYNAMIC_MEMORY
  return true;
#endif
}

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_DEQUE_FUNC(push_back)(TLBT_DEQUE_TYPE *const d, TLBT_T item) {
  TLBT_DEQUE_FUNC(ensure_capacity)(d, d->count + 1);
#else
TLBT_INLINE bool TLBT_DEQUE_FUNC(push_back)(TLBT_DEQUE_TYPE *const d, TLBT_T item) {
  if (d->count + 1 > d->capacity)
    return false;
#endif

  d->data[d->tail] = item;
  d->tail = TLBT_DEQUE_INC_WRAP(d->tail, d->capacity);
  ++d->count;

#ifndef TLBT_DYNAMIC_MEMORY
  return true;
#endif
}

TLBT_INLINE bool TLBT_DEQUE_FUNC(pop_front)(TLBT_DEQUE_TYPE *const d) {
  if (d->count == 0)
    return false;
  d->head = TLBT_DEQUE_INC_WRAP(d->head, d->capacity);
  --d->count;
  return true;
}

TLBT_INLINE bool TLBT_DEQUE_FUNC(pop_back)(TLBT_DEQUE_TYPE *const d) {
  if (d->count == 0)
    return false;
  d->tail = TLBT_DEQUE_DEC_WRAP(d->tail, d->capacity);
  --d->count;
  return true;
}

TLBT_INLINE TLBT_T *TLBT_DEQUE_FUNC(peek_front)(TLBT_DEQUE_TYPE *const d) {
  return d->count == 0 ? NULL : &d->data[d->head];
}

TLBT_INLINE TLBT_T *TLBT_DEQUE_FUNC(peek_back)(TLBT_DEQUE_TYPE *const d) {
  return d->count == 0 ? NULL : &d->data[TLBT_DEQUE_DEC_WRAP(d->tail, d->capacity)];
}

TLBT_INLINE TLBT_T *TLBT_DEQUE_FUNC(at)(TLBT_DEQUE_TYPE *const d, const TLBT_SIZE_T index) {
  return d->count == 0 ? NULL : &d->data[TLBT_MOD(d->head + index, d->capacity)];
}

#ifdef TLBT_DYNAMIC_MEMORY
TLBT_INLINE void TLBT_DEQUE_FUNC(copy)(TLBT_DEQUE_TYPE *const dest, const TLBT_DEQUE_TYPE *const src) {
  TLBT_DEQUE_FUNC(ensure_capacity)(dest, src->count);
#else
TLBT_INLINE bool TLBT_DEQUE_FUNC(copy)(TLBT_DEQUE_TYPE *const dest, const TLBT_DEQUE_TYPE *const src) {
  if (dest->capacity < src->capacity)
    return false;
#endif

  if (dest->capacity == src->capacity || (src->head < src->tail)) {
    memcpy(dest->data, src->data, src->capacity);
    dest->head = src->head;
    dest->tail = src->tail;
    dest->count = src->count;
  } else /* dest cap bigger than src cap and head bigger than tail*/ {
    // example
    //          T  H
    // src  [4][5][1][2][3]
    // dest [ ][ ][ ][ ][ ][ ][ ][ ][ ]
    // copying it as is would mean that the new 4 slots are also valid items
    // goal [1][2][3][4][5][ ][ ][ ][ ]
    // copy [1] [2] [3]
    memcpy(dest->data, src->data + src->head, (src->capacity - src->head) * sizeof(TLBT_T));
    // copy [4] [5]
    memcpy(dest->data + src->capacity - src->head, src->data, src->tail * sizeof(TLBT_T));
    dest->tail = src->capacity - src->head + src->tail;
    dest->head = 0;
    dest->count = src->count;
  }
#ifndef TLBT_DYNAMIC_MEMORY
  return true;
#endif
}

#endif

#undef TLBT_ASSERT
#undef TLBT_BASE2_CAPACITY
#undef TLBT_COMBINE
#undef TLBT_COMBINE2
#undef TLBT_DEFINITION
#undef TLBT_DEQUE_DEC_WRAP
#undef TLBT_DEQUE_FUNC
#undef TLBT_DEQUE_INC_WRAP
#undef TLBT_DEQUE_ITERATOR_FUNC
#undef TLBT_DEQUE_ITERATOR_TYPE
#undef TLBT_DEQUE_NO_ITERATOR
#undef TLBT_DEQUE_TYPE
#undef TLBT_DYNAMIC_MEMORY
#undef TLBT_FREE
#undef TLBT_IMPLEMENTATION
#undef TLBT_INLINE
#undef TLBT_MALLOC
#undef TLBT_MOD
#undef TLBT_SIZE_T
#undef TLBT_STATIC
#undef TLBT_T
#undef TLBT_T_NAME

