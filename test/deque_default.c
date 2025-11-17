#include "common.h"

static bool internal_assert_triggered = false;
#define INTERNAL_ASSERT(cond)                                                                                          \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      /* this only works because the functions are statically defined */                                               \
      internal_assert_triggered = true;                                                                                \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

#define TLBT_T int
#define TLBT_STATIC
#define TLBT_BASE2_CAPACITY
#define TLBT_ASSERT INTERNAL_ASSERT
#include "../src/deque.h"

int main(void) {
  int buffer[16] = {0};
  tlbt_deque_int d = {0};
  tlbt_deque_int_init(&d, 13, buffer);
  TLBT_TEST_ASSERT(internal_assert_triggered, "internal assert should have triggered because of no base2 capacity");
  internal_assert_triggered = false;

  tlbt_deque_int_init(&d, 16, buffer);
  TLBT_TEST_ASSERT(!internal_assert_triggered, "there should be no failed assertion");
  TLBT_TEST_ASSERT(d.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(d.count == 0, "count should be 0");
  TLBT_TEST_ASSERT(d.data == buffer, "should use the same array for values");

  TLBT_TEST_ASSERT(!tlbt_deque_int_pop_back(&d), "should not be able to pop an element with an empty deque");
  TLBT_TEST_ASSERT(!tlbt_deque_int_pop_front(&d), "should not be able to pop an element with an empty deque");
  TLBT_TEST_ASSERT(tlbt_deque_int_peek_back(&d) == NULL, "peek return value should be NULL with an empty deque");
  TLBT_TEST_ASSERT(tlbt_deque_int_peek_front(&d) == NULL, "peek return value should be NULL with an empty deque");

  /* shuf -i 10-99 -n 16 | paste -sd ',' */
  int values[16] = {91, 19, 56, 37, 86, 95, 82, 12, 42, 11, 94, 89, 39, 14, 53, 48};

  for (int i = 0; i < 16; ++i) {
    bool success = tlbt_deque_int_push_back(&d, values[i]);
    TLBT_TEST_ASSERT(success, "should have successfully pushed to the back");
  }
  TLBT_TEST_ASSERT(d.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(d.count == 16, "count should be 16");
  {
    bool success = tlbt_deque_int_push_back(&d, 42);
    TLBT_TEST_ASSERT(!success, "should have failed pushing to the back");
  }

  TLBT_TEST_ASSERT(*tlbt_deque_int_peek_back(&d) == values[15], "wrong peek value");
  TLBT_TEST_ASSERT(*tlbt_deque_int_peek_front(&d) == values[0], "wrong peek value");

  for (int i = 0; i < 16; ++i) {
    TLBT_TEST_ASSERT(*tlbt_deque_int_at(&d, i) == values[i], "values should be the same");
  }

  {
    tlbt_deque_iterator_int iter = {0};
    tlbt_deque_iterator_int_init(&iter, &d);
    int i = 0, v = 0;
    while (tlbt_deque_iterator_int_iterate(&iter, &v)) {
      TLBT_TEST_ASSERT(*tlbt_deque_int_at(&d, i++) == v, "values should be the same");
    }
  }

  for (int i = 0; i < 4; ++i) {
    bool success = tlbt_deque_int_pop_back(&d);
    TLBT_TEST_ASSERT(success, "should have successfully popped back");
  }

  for (int i = 0; i < 4; ++i) {
    bool success = tlbt_deque_int_pop_front(&d);
    TLBT_TEST_ASSERT(success, "should have successfully popped front");
  }

  TLBT_TEST_ASSERT(d.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(d.count == 8, "count should be 8");
  TLBT_TEST_ASSERT(*tlbt_deque_int_peek_front(&d) == values[4], "wrong peek value");
  TLBT_TEST_ASSERT(*tlbt_deque_int_peek_back(&d) == values[11], "wrong peek value");

  for (int i = 0; i < 8; ++i) {
    bool success = tlbt_deque_int_push_front(&d, i);
    TLBT_TEST_ASSERT(success, "should have successfully pushed to the front");
  }
  TLBT_TEST_ASSERT(d.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(d.count == 16, "count should be 8");

  for (int i = 0; i < 8; ++i) {
    TLBT_TEST_ASSERT(*tlbt_deque_int_at(&d, i) == 7 - i, "wrong value");
  }
  for (int i = 8; i < 16; ++i) {
    TLBT_TEST_ASSERT(*tlbt_deque_int_at(&d, i) == values[4 + (i - 8)], "wrong value");
  }

  {
    tlbt_deque_int copy = {0};
    int copy_buffer[32] = {0};
    tlbt_deque_int_init(&copy, 32, copy_buffer);
    bool success = tlbt_deque_int_copy(&copy, &d);
    TLBT_TEST_ASSERT(success, "should have successfully copied");
    TLBT_TEST_ASSERT(copy.count == d.count, "count should be the same");
    for (int i = 0; i < copy.count; ++i) {
      TLBT_TEST_ASSERT(*tlbt_deque_int_at(&d, i) == *tlbt_deque_int_at(&copy, i), "copied elements should be the same");
    }
  }

  {
    tlbt_deque_int copy = {0};
    int copy_buffer[8] = {0};
    tlbt_deque_int_init(&copy, 8, copy_buffer);
    bool success = tlbt_deque_int_copy(&copy, &d);
    TLBT_TEST_ASSERT(!success, "should not have successfully copied");
  }

  TLBT_TEST_DONE();
}

