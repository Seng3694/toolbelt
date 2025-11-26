#include "common.h"
#include "../src/assert.h"

#define TLBT_T int
#define TLBT_COMPARE(left, right) ((left) - (right))
#define TLBT_STATIC
// min heap is default
#include "../src/heap.h"

#define TLBT_T int
#define TLBT_COMPARE(left, right) ((left) - (right))
#define TLBT_MAX_HEAP
#define TLBT_STATIC
#include "../src/heap.h"

int main(void) {
  TLBT_TEST_START();
  const int values[8] = {14, -20, 5, 33, 42, 9, 0, 5};
  const int min_order[8] = {-20, 0, 5, 5, 9, 14, 33, 42};
  const int max_order[8] = {42, 33, 14, 9, 5, 5, 0, -20};

  // min heap
  {
    int buffer[8] = {0};
    tlbt_min_heap_int h = {0};
    tlbt_min_heap_int_init(&h, 8, buffer);
    tlbt_assert_msg(h.count == 0, "count should be 0");
    tlbt_assert_msg(h.capacity == 8, "capacity should be 8");
    tlbt_assert_msg(h.data == buffer, "buffer should be the same");
    for (int i = 0; i < 8; ++i) {
      bool success = tlbt_min_heap_int_push(&h, values[i]);
      tlbt_assert_msg(success, "should have pushed successfully");
    }

    tlbt_assert_msg(h.count == 8, "count should be 8");
    {
      bool success = tlbt_min_heap_int_push(&h, 0);
      tlbt_assert_msg(!success, "should have failed pushing");
    }

    for (int i = 0; i < 8; ++i) {
      int value = 0;
      bool success = tlbt_min_heap_int_peek(&h, &value);
      tlbt_assert_msg(success, "should have peeked successfully");
      success = tlbt_min_heap_int_pop(&h);
      tlbt_assert_msg(success, "should have popped successfully");
      tlbt_assert_msg(value == min_order[i], "min heap sorted incorrectly");
    }
    tlbt_assert_msg(h.count == 0, "count should be 0");
    {
      int value = 0;
      bool success = tlbt_min_heap_int_peek(&h, &value);
      success = tlbt_min_heap_int_pop(&h);
      tlbt_assert_msg(!success, "should have failed peeking and popping");
    }

    // redo the heap and try building it from an existing buffer
    memcpy(buffer, values, sizeof(values));
    tlbt_min_heap_int_build(&h, 8, 8, buffer);
    tlbt_assert_msg(h.count == 8, "count should be 8");
    tlbt_assert_msg(h.capacity == 8, "capacity should be 8");
    tlbt_assert_msg(h.data == buffer, "buffer should be the same");

    // try copying first
    {
      int buffer2[8] = {0};
      tlbt_min_heap_int h2 = {0};
      tlbt_min_heap_int_init(&h2, 4, buffer2);
      bool success = tlbt_min_heap_int_copy(&h2, &h);
      tlbt_assert_msg(!success, "should have failed copying because dest capacity is smaller");
      tlbt_min_heap_int_init(&h2, 8, buffer2);
      success = tlbt_min_heap_int_copy(&h2, &h);
      tlbt_assert_msg(success, "should have copied successfully");
      tlbt_assert_msg(h.count == h2.count && memcmp(h.data, h2.data, sizeof(int) * 8) == 0, "they should be equal");
      tlbt_min_heap_int_clear(&h2);
      tlbt_assert_msg(h2.count == 0, "count should be 0");
    }

    // now check if the elements are in order when building the heap from a buffer with values
    for (int i = 0; i < 8; ++i) {
      int value = 0;
      bool success = tlbt_min_heap_int_peek(&h, &value);
      tlbt_assert_msg(success, "should have peeked successfully");
      success = tlbt_min_heap_int_pop(&h);
      tlbt_assert_msg(success, "should have popped successfully");
      tlbt_assert_msg(value == min_order[i], "min heap sorted incorrectly");
    }
    tlbt_assert_msg(h.count == 0, "count should be 0");
  }

  // max heap (only test the order. the rest is the same as the min heap)
  {
    int buffer[8] = {0};
    tlbt_max_heap_int h = {0};
    tlbt_max_heap_int_init(&h, 8, buffer);
    for (int i = 0; i < 8; ++i) {
      bool success = tlbt_max_heap_int_push(&h, values[i]);
      tlbt_assert_msg(success, "should have pushed successfully");
    }
    for (int i = 0; i < 8; ++i) {
      int value = 0;
      bool success = tlbt_max_heap_int_peek(&h, &value);
      tlbt_assert_msg(success, "should have peeked successfully");
      success = tlbt_max_heap_int_pop(&h);
      tlbt_assert_msg(success, "should have popped successfully");
      tlbt_assert_msg(value == max_order[i], "max heap sorted incorrectly");
    }
    tlbt_assert_msg(h.count == 0, "count should be 0");
  }

  TLBT_TEST_DONE();
}

