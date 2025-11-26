#include "common.h"
#include "../src/assert.h"

static bool internal_assert_triggered = false;
#define INTERNAL_ASSERT(cond)                                                                                          \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      /* this only works because the functions are statically defined */                                               \
      internal_assert_triggered = true;                                                                                \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

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

#define TLBT_KEY_T string_slice
#define TLBT_VALUE_T point
#define TLBT_HASH_FUNC(x) string_slice_hash(&x)
#define TLBT_EQUALS_FUNC(a, b) string_slice_equals(&a, &b)
#define TLBT_KEY_T_NAME str
#define TLBT_VALUE_T_NAME point
#define TLBT_MAX_LOAD_FACTOR 0.5 // makes no sense but easier for testing
#define TLBT_BASE2_CAPACITY
#define TLBT_ASSERT INTERNAL_ASSERT
#define TLBT_DYNAMIC_MEMORY
#define TLBT_MALLOC custom_alloc
#define TLBT_FREE custom_free
#define TLBT_STATIC
#include "../src/hashmap.h"

int main(void) {
  TLBT_TEST_START();
  tlbt_map_str_point m = {0};
  tlbt_map_str_point_create(&m, 16);

  tlbt_assert_msg(!internal_assert_triggered, "there should be no failed assertion");
  tlbt_assert_msg(m.capacity == 16, "capacity should be 16");
  tlbt_assert_msg(m.count == 0, "count should be 0");

  const char *test_strings[16] = {"hello",   "world", "!",      "these", "are",  "some", "unique", "test",
                                  "strings", "I",     "should", "not",   "need", "more", "than",   "sixteen"};

  const point test_values[16] = {
      {0, 15}, {1, 14}, {2, 13}, {3, 12}, {4, 11}, {5, 10}, {6, 9},  {7, 8},
      {8, 7},  {9, 6},  {10, 5}, {11, 4}, {12, 3}, {13, 2}, {14, 1}, {15, 0},
  };

  // insert test
  for (int i = 0; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool success = tlbt_map_str_point_insert(&m, key, test_values[i]);
    tlbt_assert_msg(success, "should have inserted successfully");
  }
  tlbt_assert_msg(m.capacity == 32, "capacity should be 32");
  tlbt_assert_msg(m.count == 16, "count should be 16");

  // contains test
  for (int i = 0; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool contains = tlbt_map_str_point_contains(&m, key);
    tlbt_assert_msg(contains, "should have contained element");
  }

  // remove test
  for (int i = 0; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool removed = tlbt_map_str_point_remove(&m, key);
    tlbt_assert_msg(removed, "should have removed element");
  }
  tlbt_assert_msg(m.capacity == 32, "capacity should be 32");
  tlbt_assert_msg(m.count == 0, "count should be 0 now");

  tlbt_map_str_point_destroy(&m);

  tlbt_assert_msg(allocations == frees, "there should be the same amount of allocations and frees");
  TLBT_TEST_DONE();
}

