/*
types:
- tlbt_arena

functions:
- tlbt_arena_create                      creates the arena, allocates a buffer with the given capacity aligned to 8
- tlbt_arena_create_root_and_sub_arenas  creates a root arena and initializes sub arenas with that root arena
- tlbt_arena_malloc                      retrieve a pointer to the internal buffer. block is aligned to 8
- tlbt_arena_reset                       resets the arena
- tlbt_arena_destroy                     frees the allocated buffer

TLBT_IMPLEMENTATION  for the implementation of the definitions in a separate source file

=== optional definitions ===
TLBT_ASSERT  default is assert from <assert.h>
TLBT_MALLOC  default is malloc from <stdlib.h>
TLBT_FREE    default is free from <stdlib.h>
*/

#ifndef TLBT_ARENA_H
#define TLBT_ARENA_H

#ifndef TLBT_MALLOC
#include <stdlib.h>
#define TLBT_MALLOC malloc
// memory is managed by this implementation so redefine free just in case
#undef TLBT_FREE
#define TLBT_FREE free
#endif

#ifndef TLBT_ASSERT
#include <assert.h>
#define TLBT_ASSERT assert
#endif

#include <stddef.h>

#define TLBT_ALIGN_TO_8(x) (((x) + 7) & (~(size_t)7))

typedef struct tlbt_arena {
  size_t current;
  size_t capacity;
  unsigned char *memory;
} tlbt_arena;

void tlbt_arena_create(size_t capacity, tlbt_arena *const a);
void tlbt_arena_create_root_and_sub_arenas(tlbt_arena *const root, const size_t count, const size_t *const capacities,
                                           tlbt_arena *const sub_arenas);
void *tlbt_arena_malloc(size_t size, tlbt_arena *const a);

static inline void tlbt_arena_destroy(tlbt_arena *const a) {
  TLBT_ASSERT(a->memory);
  TLBT_FREE(a->memory);
}

static inline void tlbt_arena_reset(tlbt_arena *const a) {
  a->current = 0;
}

#endif

#ifdef TLBT_IMPLEMENTATION

void tlbt_arena_create(size_t capacity, tlbt_arena *const a) {
  a->capacity = TLBT_ALIGN_TO_8(capacity);
  a->current = 0;
  a->memory = TLBT_MALLOC(a->capacity);
  TLBT_ASSERT(a->memory);
}

void tlbt_arena_create_root_and_sub_arenas(tlbt_arena *const root, const size_t count, const size_t *const capacities,
                                           tlbt_arena *const sub_arenas) {
  size_t root_capacity = 0;
  for (size_t i = 0; i < count; ++i) {
    root_capacity += TLBT_ALIGN_TO_8(capacities[i]);
  }
  tlbt_arena_create(root_capacity, root);
  for (size_t i = 0; i < count; ++i) {
    sub_arenas[i].capacity = TLBT_ALIGN_TO_8(capacities[i]);
    sub_arenas[i].current = 0;
    sub_arenas[i].memory = tlbt_arena_malloc(capacities[i], root);
    TLBT_ASSERT(sub_arenas[i].memory);
  }
}

void *tlbt_arena_malloc(size_t size, tlbt_arena *const a) {
  size = TLBT_ALIGN_TO_8(size);
  if (a->current + size > a->capacity) {
    TLBT_ASSERT(a->current + size > a->capacity); // crash in debug builds
    return NULL;
  }
  void *ptr = a->memory + a->current;
  a->current += size;
  return ptr;
}

#endif

