// multi include with the same type should be fine
#define TLBT_KEY_T int
#define TLBT_VALUE_T int
#define TLBT_HASH_FUNC(x) (x)
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_VALUE_T int
#define TLBT_HASH_FUNC(x) (x)
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_VALUE_T int
#define TLBT_HASH_FUNC(x) (x)
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

// same type with different name should be fine
#define TLBT_KEY_T int
#define TLBT_KEY_T_NAME integer
#define TLBT_VALUE_T int
#define TLBT_HASH_FUNC(x) (x)
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_KEY_T_NAME integer
#define TLBT_VALUE_T int
#define TLBT_HASH_FUNC(x) (x)
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_KEY_T_NAME integer
#define TLBT_VALUE_T int
#define TLBT_HASH_FUNC(x) (x)
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

// different type obviously as well
// const is required for pointer types on keys
#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_VALUE_T float *
#define TLBT_VALUE_T_NAME floatptr
#define TLBT_HASH_FUNC(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_VALUE_T float
#define TLBT_VALUE_T_NAME floatptr
#define TLBT_HASH_FUNC(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_VALUE_T float
#define TLBT_VALUE_T_NAME floatptr
#define TLBT_HASH_FUNC(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

// hashset
#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_HASH_FUNC(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_HASH_FUNC(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_HASH_FUNC(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS_FUNC(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

int main(void) {
  return 0;
}
