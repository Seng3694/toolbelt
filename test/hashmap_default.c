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

#define TLBT_KEY_T string_slice
#define TLBT_VALUE_T point
#define TLBT_HASH_FUNC(x) string_slice_hash(&x)
#define TLBT_EQUALS_FUNC(a, b) string_slice_equals(&a, &b)
#define TLBT_KEY_T_NAME str
#define TLBT_VALUE_T_NAME point
#define TLBT_MAX_LOAD_FACTOR 0.5 // makes no sense but easier for testing
#define TLBT_BASE2_CAPACITY
#define TLBT_ASSERT INTERNAL_ASSERT
#define TLBT_STATIC
#include "../src/hashmap.h"

int main(void) {
  tlbt_map_str_point_key keys[16] = {0};
  point values[16] = {0};

  tlbt_map_str_point m = {0};
  tlbt_map_str_point_init(&m, 13, keys, values);
  TLBT_TEST_ASSERT(internal_assert_triggered, "internal assert should have triggered because of no base2 capacity");
  internal_assert_triggered = false;

  tlbt_map_str_point_init(&m, 16, keys, values);
  TLBT_TEST_ASSERT(!internal_assert_triggered, "there should be no failed assertion");
  TLBT_TEST_ASSERT(m.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(m.count == 0, "count should be 0");
  TLBT_TEST_ASSERT(m.keys == keys, "should use the same array for keys");
  TLBT_TEST_ASSERT(m.values == values, "should use the same array for values");

  const char *test_strings[16] = {"hello",   "world", "!",      "these", "are",  "some", "unique", "test",
                                  "strings", "I",     "should", "not",   "need", "more", "than",   "sixteen"};

  const point test_values[16] = {
      {0, 15}, {1, 14}, {2, 13}, {3, 12}, {4, 11}, {5, 10}, {6, 9},  {7, 8},
      {8, 7},  {9, 6},  {10, 5}, {11, 4}, {12, 3}, {13, 2}, {14, 1}, {15, 0},
  };

  // contains test with empty map
  for (int i = 0; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool contains = tlbt_map_str_point_contains(&m, key);
    TLBT_TEST_ASSERT(!contains, "shouldn't have found element");
  }

  // get test with empty map
  for (int i = 0; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    point value = {0};
    const bool found = tlbt_map_str_point_get(&m, key, &value);
    TLBT_TEST_ASSERT(!found, "shouldn't have found element");
  }

  // iterator test with empty map
  {
    string_slice *key = NULL;
    point *value = NULL;
    tlbt_map_iterator_str_point iter = {0};
    tlbt_map_iterator_str_point_init(&iter, &m);
    bool iterated = tlbt_map_iterator_str_point_iterate(&iter, &key, &value);
    TLBT_TEST_ASSERT(!iterated, "should not be able to iterate because there are no elements");
  }

  // insert test
  for (int i = 0; i < 8; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool success = tlbt_map_str_point_insert(&m, key, test_values[i]);
    TLBT_TEST_ASSERT(success, "should have inserted successfully");
  }
  for (int i = 8; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool success = tlbt_map_str_point_insert(&m, key, test_values[i]);
    TLBT_TEST_ASSERT(!success, "shouldn't have inserted successfully due to load factor");
  }
  TLBT_TEST_ASSERT(m.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(m.count == 8, "count should be 8");

  // contains test
  for (int i = 0; i < 8; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool contains = tlbt_map_str_point_contains(&m, key);
    TLBT_TEST_ASSERT(contains, "should have contained element");
  }
  for (int i = 8; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool contains = tlbt_map_str_point_contains(&m, key);
    TLBT_TEST_ASSERT(!contains, "shouldn't' have contained element");
  }

  // get test
  for (int i = 0; i < 8; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    point value = {0};
    const bool found = tlbt_map_str_point_get(&m, key, &value);
    TLBT_TEST_ASSERT(found, "should have found element");
    TLBT_TEST_ASSERT(value.x == test_values[i].x && value.y == test_values[i].y, "wrong value associated with key");
  }
  for (int i = 8; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    point value = {0};
    const bool found = tlbt_map_str_point_get(&m, key, &value);
    TLBT_TEST_ASSERT(!found, "shouldn't' have found element");
  }

  // iterator test
  {
    string_slice *key = NULL;
    point *value = NULL;
    tlbt_map_iterator_str_point iter = {0};
    tlbt_map_iterator_str_point_init(&iter, &m);
    int iterations = 0;
    while (tlbt_map_iterator_str_point_iterate(&iter, &key, &value)) {
      bool correct_pair = false;
      // try finding the pair in the source data
      for (int i = 0; i < 8; ++i) {
        if (strcmp(key->data, test_strings[i]) == 0) {
          if (value->x == test_values[i].x && value->y == test_values[i].y) {
            correct_pair = true;
          }
        }
      }
      TLBT_TEST_ASSERT(correct_pair, "iterated kvp not found in source data");
      ++iterations;
    }
    TLBT_TEST_ASSERT(iterations == m.count, "iteration count different from element count");
  }

  // copy test
  {
    tlbt_map_str_point_key keys2[16] = {0};
    point values2[16] = {0};

    tlbt_map_str_point m2 = {0};
    tlbt_map_str_point_init(&m2, 16, keys2, values2);
    const bool success = tlbt_map_str_point_copy(&m2, &m);
    TLBT_TEST_ASSERT(success, "should have succeeded copying");
    TLBT_TEST_ASSERT(m.count == m2.count, "should have the same count");

    string_slice *key = NULL;
    point *value = NULL;
    tlbt_map_iterator_str_point iter = {0};
    tlbt_map_iterator_str_point_init(&iter, &m2);
    while (tlbt_map_iterator_str_point_iterate(&iter, &key, &value)) {
      point value2 = {0};
      const bool contains = tlbt_map_str_point_get(&m, *key, &value2);
      TLBT_TEST_ASSERT(contains, "src map should contain the key");
      TLBT_TEST_ASSERT(value->x == value2.x && value->y == value2.y, "src and dest values should be the same");
    }

    // clear test
    tlbt_map_str_point_clear(&m2);
    TLBT_TEST_ASSERT(m2.count == 0, "count should be 0 after clear");
    TLBT_TEST_ASSERT(m2.capacity == 16, "capacity should still be 16 after clear");
  }

  // copy test 2
  {
    tlbt_map_str_point_key keys2[4] = {0};
    point values2[4] = {0};

    tlbt_map_str_point m2 = {0};
    tlbt_map_str_point_init(&m2, 4, keys2, values2);
    const bool success = tlbt_map_str_point_copy(&m2, &m);
    TLBT_TEST_ASSERT(!success, "shouldn't have succeeded copying");
  }

  // remove test
  for (int i = 0; i < 8; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool removed = tlbt_map_str_point_remove(&m, key);
    TLBT_TEST_ASSERT(removed, "should have removed element");
  }
  for (int i = 8; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool removed = tlbt_map_str_point_remove(&m, key);
    TLBT_TEST_ASSERT(!removed, "shouldn't have removed element");
  }
  TLBT_TEST_ASSERT(m.count == 0, "count should be 0 now");

  TLBT_TEST_DONE();
}

