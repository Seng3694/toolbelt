#include "common.h"
#include "../src/assert.h"

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

#define TLBT_T int
#define TLBT_COMPARE(left, right) ((left) - (right))
#define TLBT_DYNAMIC_MEMORY
#define TLBT_MALLOC custom_alloc
#define TLBT_FREE custom_free
#define TLBT_STATIC
#include "../src/heap.h"

int main(void) {
  TLBT_TEST_START();
  const int values[8] = {14, -20, 5, 33, 42, 9, 0, 5};
  const int min_order[8] = {-20, 0, 5, 5, 9, 14, 33, 42};

  tlbt_min_heap_int h = {0};
  tlbt_min_heap_int_create(&h, 3);
  tlbt_assert_msg(h.count == 0, "count should be 0");
  tlbt_assert_msg(h.capacity == 3, "capacity should be 3");
  tlbt_min_heap_int_push(&h, values[0]);
  tlbt_assert_msg(h.count == 1, "count should be 1");
  tlbt_assert_msg(h.capacity == 3, "capacity should be 3");

  for (int i = 1; i < 4; ++i) {
    tlbt_min_heap_int_push(&h, values[i]);
  }
  tlbt_assert_msg(h.count == 4, "count should be 4");
  tlbt_assert_msg(h.capacity == 6, "capacity should be 6");

  for (int i = 4; i < 8; ++i) {
    tlbt_min_heap_int_push(&h, values[i]);
  }
  tlbt_assert_msg(h.count == 8, "count should be 8");
  tlbt_assert_msg(h.capacity == 12, "capacity should be 12");

  // copy
  {
    tlbt_min_heap_int h2 = {0};
    tlbt_min_heap_int_create(&h2, 2);
    tlbt_min_heap_int_copy(&h2, &h);
    tlbt_assert_msg(h.count == h2.count, "count should be the same");
    tlbt_assert_msg(h2.capacity >= h2.count, "capacity should be bigger than count or equal");
    tlbt_assert_msg(memcmp(h.data, h2.data, h.count * sizeof(int)) == 0, "data should be the same");
    tlbt_min_heap_int_destroy(&h2);
  }

  for (int i = 0; i < 8; ++i) {
    int value = 0;
    bool success = tlbt_min_heap_int_peek(&h, &value);
    tlbt_assert_msg(success, "should have peeked successfully");
    success = tlbt_min_heap_int_pop(&h);
    tlbt_assert_msg(success, "should have popped successfully");
    tlbt_assert_msg(value == min_order[i], "min heap sorted incorrectly");
  }

  // build test
  {
    tlbt_min_heap_int h2 = {0};
    // this allocates memory and copies the values from the buffer without touching it
    tlbt_min_heap_int_build(&h2, 8, values);
    tlbt_assert_msg(h2.data != values, "buffer should be different");
    tlbt_assert_msg(h2.count == 8, "count should be 8");
    tlbt_assert_msg(h2.capacity == 8, "capacity should be 8");
    for (int i = 0; i < 8; ++i) {
      int value = 0;
      bool success = tlbt_min_heap_int_peek(&h2, &value);
      tlbt_assert_msg(success, "should have peeked successfully");
      success = tlbt_min_heap_int_pop(&h2);
      tlbt_assert_msg(success, "should have popped successfully");
      tlbt_assert_msg(value == min_order[i], "min heap sorted incorrectly");
    }
    tlbt_min_heap_int_destroy(&h2);
  }

  // move test (basically the same as build but with dynamic memory management)
  {
    // externally allocated buffer with a capacity of 16 ints
    int *buffer = custom_alloc(sizeof(int) * 16);
    memcpy(buffer, values, sizeof(values));
    tlbt_min_heap_int h2 = {0};
    // this moves the memory responsibility of the buffer to the heap implementation
    tlbt_min_heap_int_move(&h2, 16, 8, buffer);
    tlbt_assert_msg(h2.data == buffer, "buffer should be the same");
    tlbt_assert_msg(h2.count == 8, "count should be 8");
    tlbt_assert_msg(h2.capacity == 16, "capacity should be 16");
    for (int i = 0; i < 8; ++i) {
      int value = 0;
      bool success = tlbt_min_heap_int_peek(&h2, &value);
      tlbt_assert_msg(success, "should have peeked successfully");
      success = tlbt_min_heap_int_pop(&h2);
      tlbt_assert_msg(success, "should have popped successfully");
      tlbt_assert_msg(value == min_order[i], "min heap sorted incorrectly");
    }
    // buffer will be freed here
    tlbt_min_heap_int_destroy(&h2);
  }

  tlbt_min_heap_int_destroy(&h);
  tlbt_assert_msg(allocations == frees, "there should be the same amount of allocations and frees");
  TLBT_TEST_DONE();
}

