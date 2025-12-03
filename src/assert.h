#pragma once

#ifndef NDEBUG

#include <stdio.h>

#define tlbt_assert(cond)                                                                                              \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      fprintf(stderr, "Assertion failed:\nFile: '%s:%d'\nFunction: '%s'\nCondition: '%s'\n", __FILE__, __LINE__,       \
              __FUNCTION__, #cond);                                                                                    \
      abort();                                                                                                         \
    }                                                                                                                  \
  } while (0)

#define tlbt_assert_msg(cond, message)                                                                                 \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      fprintf(stderr, "Assertion failed: '%s'\nFile: '%s:%d'\nFunction: '%s'\nCondition: '%s'\n", message, __FILE__,   \
              __LINE__, __FUNCTION__, #cond);                                                                          \
      abort();                                                                                                         \
    }                                                                                                                  \
  } while (0)

#define tlbt_assert_fmt(cond, message, ...)                                                                            \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      char buffer[4096] = {0};                                                                                         \
      snprintf(buffer, sizeof(buffer), message, __VA_ARGS__);                                                          \
      fprintf(stderr, "Assertion failed: '%s'\nFile: '%s:%d'\nFunction: '%s'\nCondition: '%s'\n", buffer, __FILE__,    \
              __LINE__, __FUNCTION__, #cond);                                                                          \
      abort();                                                                                                         \
    }                                                                                                                  \
  } while (0)

#define tlbt_assert_unreachable()                                                                                      \
  do {                                                                                                                 \
    fprintf(stderr, "Assertion failed: 'should never reach'\nFile: '%s:%d'\nFunction: '%s'\n", __FILE__, __LINE__,     \
            __FUNCTION__);                                                                                             \
    abort();                                                                                                           \
  } while (0)

#else

#define tlbt_assert(cond)
#define tlbt_assert_msg(cond, message)
#define tlbt_assert_fmt(cond, message, ...)
#define tlbt_assert_unreachable()

#endif
