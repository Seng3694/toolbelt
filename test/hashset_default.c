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
#define TLBT_HASH_FUNC(x) string_slice_hash(&x)
#define TLBT_EQUALS_FUNC(a, b) string_slice_equals(&a, &b)
#define TLBT_KEY_T_NAME str
#define TLBT_MAX_LOAD_FACTOR 0.5 // makes no sense but easier for testing
#define TLBT_ASSERT INTERNAL_ASSERT
#define TLBT_STATIC
#include "../src/hashmap.h"

int main(void) {
  tlbt_set_str_key keys[16] = {0};

  tlbt_set_str m = {0};
  tlbt_set_str_init(&m, 16, keys);
  TLBT_TEST_ASSERT(!internal_assert_triggered, "there should be no failed assertion");
  TLBT_TEST_ASSERT(m.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(m.count == 0, "count should be 0");
  TLBT_TEST_ASSERT(m.keys == keys, "should use the same array for keys");

  const char *test_strings[16] = {"hello",   "world", "!",      "these", "are",  "some", "unique", "test",
                                  "strings", "I",     "should", "not",   "need", "more", "than",   "sixteen"};

  // contains test with empty map
  for (int i = 0; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool contains = tlbt_set_str_contains(&m, key);
    TLBT_TEST_ASSERT(!contains, "shouldn't have found element");
  }

  // iterator test with empty map
  {
    string_slice *key = NULL;
    tlbt_set_iterator_str iter = {0};
    tlbt_set_iterator_str_init(&iter, &m);
    bool iterated = tlbt_set_iterator_str_iterate(&iter, &key);
    TLBT_TEST_ASSERT(!iterated, "should not be able to iterate because there are no elements");
  }

  // insert test
  for (int i = 0; i < 8; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool success = tlbt_set_str_insert(&m, key);
    TLBT_TEST_ASSERT(success, "should have inserted successfully");
  }
  for (int i = 8; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool success = tlbt_set_str_insert(&m, key);
    TLBT_TEST_ASSERT(!success, "shouldn't have inserted successfully due to load factor");
  }
  TLBT_TEST_ASSERT(m.capacity == 16, "capacity should be 16");
  TLBT_TEST_ASSERT(m.count == 8, "count should be 8");

  // contains test
  for (int i = 0; i < 8; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool contains = tlbt_set_str_contains(&m, key);
    TLBT_TEST_ASSERT(contains, "should have contained element");
  }
  for (int i = 8; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool contains = tlbt_set_str_contains(&m, key);
    TLBT_TEST_ASSERT(!contains, "shouldn't' have contained element");
  }

  // iterator test
  {
    string_slice *key = NULL;
    tlbt_set_iterator_str iter = {0};
    tlbt_set_iterator_str_init(&iter, &m);
    int iterations = 0;
    while (tlbt_set_iterator_str_iterate(&iter, &key)) {
      bool found = false;
      // try finding the key in the source data
      for (int i = 0; i < 8; ++i) {
        if (strcmp(key->data, test_strings[i]) == 0) {
          found = true;
          break;
        }
      }
      TLBT_TEST_ASSERT(found, "iterated key not found in source data");
      ++iterations;
    }
    TLBT_TEST_ASSERT(iterations == m.count, "iteration count different from element count");
  }

  // copy test
  {
    tlbt_set_str_key keys2[16] = {0};

    tlbt_set_str m2 = {0};
    tlbt_set_str_init(&m2, 16, keys2);
    const bool success = tlbt_set_str_copy(&m2, &m);
    TLBT_TEST_ASSERT(success, "should have succeeded copying");
    TLBT_TEST_ASSERT(m.count == m2.count, "should have the same count");

    string_slice *key = NULL;
    tlbt_set_iterator_str iter = {0};
    tlbt_set_iterator_str_init(&iter, &m2);
    while (tlbt_set_iterator_str_iterate(&iter, &key)) {
      const bool contains = tlbt_set_str_contains(&m, *key);
      TLBT_TEST_ASSERT(contains, "src map should contain the key");
    }

    // clear test
    tlbt_set_str_clear(&m2);
    TLBT_TEST_ASSERT(m2.count == 0, "count should be 0 after clear");
    TLBT_TEST_ASSERT(m2.capacity == 16, "capacity should still be 16 after clear");
  }

  // copy test 2
  {
    tlbt_set_str_key keys2[4] = {0};

    tlbt_set_str m2 = {0};
    tlbt_set_str_init(&m2, 4, keys2);
    const bool success = tlbt_set_str_copy(&m2, &m);
    TLBT_TEST_ASSERT(!success, "shouldn't have succeeded copying");
  }

  // remove test
  for (int i = 0; i < 8; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool removed = tlbt_set_str_remove(&m, key);
    TLBT_TEST_ASSERT(removed, "should have removed element");
  }
  for (int i = 8; i < 16; ++i) {
    string_slice key = {.data = test_strings[i], .len = strlen(test_strings[i])};
    const bool removed = tlbt_set_str_remove(&m, key);
    TLBT_TEST_ASSERT(!removed, "shouldn't have removed element");
  }
  TLBT_TEST_ASSERT(m.count == 0, "count should be 0 now");

  TLBT_TEST_DONE();
}

