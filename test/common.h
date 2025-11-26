#pragma once

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define TLBT_TEST_START()                                                                                              \
  clock_t _test_start_time = clock();                                                                                  \
  fprintf(stdout, "Starting tests in '%s'.\n", __FILE__)

#define TLBT_TEST_DONE()                                                                                               \
  clock_t _test_end_time = clock();                                                                                    \
  fprintf(stdout, "All tests in '%s' ran successfully in %f s.\n", __FILE__,                                           \
          (float)(_test_end_time - _test_start_time) / CLOCKS_PER_SEC);                                                \
  return 0

typedef struct string_slice {
  const char *data;
  size_t len;
} string_slice;

typedef struct point {
  int x, y;
} point;

static inline uint32_t string_slice_hash(const string_slice *const s) {
  uint32_t hash = 36591911;
  const char *str = s->data;
  for (size_t i = 0; i < s->len; ++i) {
    hash = (hash << 5) + hash + (uint8_t)(str[i]);
  }
  return hash;
}

static inline bool string_slice_equals(const string_slice *const a, const string_slice *const b) {
  return a->len == b->len && memcmp(a->data, b->data, a->len) == 0;
}

