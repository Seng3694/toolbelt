// multi include with the same type should be fine
#define TLBT_KEY_T int
#define TLBT_VALUE_T int
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_VALUE_T int
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_VALUE_T int
#define TLBT_HASH(x) (x)
#define TLBT_EQUALS(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

// same type with different name should be fine
#define TLBT_KEY_T int
#define TLBT_KEY_T_NAME integer
#define TLBT_VALUE_T int
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_KEY_T_NAME integer
#define TLBT_VALUE_T int
#include "../src/hashmap.h"

#define TLBT_KEY_T int
#define TLBT_KEY_T_NAME integer
#define TLBT_VALUE_T int
#define TLBT_HASH(x) (x)
#define TLBT_EQUALS(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

// different type obviously as well
// const is required for pointer types on keys
#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_VALUE_T float *
#define TLBT_VALUE_T_NAME floatptr
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_VALUE_T float
#define TLBT_VALUE_T_NAME floatptr
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_VALUE_T float
#define TLBT_VALUE_T_NAME floatptr
#define TLBT_HASH(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

// hashset
#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#include "../src/hashmap.h"

#define TLBT_KEY_T const char *
#define TLBT_KEY_T_NAME cstring
#define TLBT_HASH(x) (x == NULL ? 0 : (unsigned int)x[0])
#define TLBT_EQUALS(a, b) (a == b)
#define TLBT_STATIC
#include "../src/hashmap.h"

// reference hash and equals
#define TLBT_KEY_T char
#define TLBT_VALUE_T char
#include "../src/hashmap.h"

#define TLBT_KEY_T char
#define TLBT_VALUE_T char
#include "../src/hashmap.h"

static inline unsigned int hash_ref_test_func(const char *c) {
  return (unsigned int)*c;
}

static inline int equals_ref_test_func(const char *left, const char *right) {
  return *left == *right;
}

#define TLBT_KEY_T char
#define TLBT_VALUE_T char
#define TLBT_HASH_REF(x) hash_ref_test_func(x)
#define TLBT_EQUALS_REF(a, b) equals_ref_test_func(a, b)
#define TLBT_STATIC
#include "../src/hashmap.h"

int main(void) {
  return 0;
}
