#include "common.h"

static int allocations = 0;
static int frees = 0;

static void *custom_alloc(size_t size) {
  ++allocations;
  return malloc(size);
}

static void custom_free(void *ptr) {
  ++frees;
  free(ptr);
}

#define TLBT_MALLOC custom_alloc
#define TLBT_FREE custom_free
#define TLBT_STATIC
#include "../src/arena.h"

int main(void) {

  // case with allocating memory with only a single arena
  {
    tlbt_arena a = {0};
    tlbt_arena_create(1020, &a);
    TLBT_TEST_ASSERT(a.capacity == 1024, "arena should have aligned capacity with a multiple of 8");
    TLBT_TEST_ASSERT(tlbt_arena_malloc(1000, &a) != NULL, "allocation should have succeeded");
    TLBT_TEST_ASSERT(tlbt_arena_malloc(1000, &a) == NULL, "allocation should have failed");
    TLBT_TEST_ASSERT(a.current != 0, "arena should not be empty");
    tlbt_arena_reset(&a);
    TLBT_TEST_ASSERT(a.current == 0, "arena should be empty");
    tlbt_arena_destroy(&a);
    TLBT_TEST_ASSERT(allocations == 1 && frees == 1, "there should have been only one allocation");
  }

  allocations = 0;
  frees = 0;
  // case with allocating memory for only a single arena but using the arena for multiple sub arenas
  // the memory of the root arena is derived by the requirements of the sub arenas
  // this is useful for example for games where you have different systems with different memory requirement
  {
    tlbt_arena root = {0};
    tlbt_arena sub_arenas[5] = {0};
    const size_t sub_arena_capacities[5] = {23, 57, 93, 42, 81};
    tlbt_arena_create_root_and_sub_arenas(&root, 5, sub_arena_capacities, sub_arenas);
    TLBT_TEST_ASSERT(root.current != 0, "arena should not be empty");
    size_t sum_of_capacities = 0;
    for (size_t i = 0; i < 5; ++i) {
      sum_of_capacities += sub_arenas[i].capacity;
      // it can be bigger due to alignment
      TLBT_TEST_ASSERT(sub_arenas[i].capacity >= sub_arena_capacities[i], "capacity should be the same or bigger");
    }
    // it's probably the same but can be bigger due to alignment
    TLBT_TEST_ASSERT(root.capacity >= sum_of_capacities, "root capacity should be the same or bigger than sum");

    for (size_t i = 0; i < 5; ++i) {
      TLBT_TEST_ASSERT(tlbt_arena_malloc(8, &sub_arenas[i]) != NULL, "allocation should have succeeded");
      TLBT_TEST_ASSERT(tlbt_arena_malloc(1000, &sub_arenas[i]) == NULL, "allocation should have failed");
      TLBT_TEST_ASSERT(sub_arenas[i].current != 0, "arena should not be empty");
      tlbt_arena_reset(&sub_arenas[i]);
      TLBT_TEST_ASSERT(sub_arenas[i].current == 0, "arena should be empty");
    }

    // only the root has to be freed because the sub arenas used the memory from the root
    tlbt_arena_destroy(&root);
    TLBT_TEST_ASSERT(allocations == 1 && frees == 1, "there should have been only one allocation");
  }

  TLBT_TEST_DONE();
}

