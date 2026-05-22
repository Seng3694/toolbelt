// multi include with the same type should be fine
#define TLBT_T int
#define TLBT_COMPARE(a, b) (a - b)
#include "../src/heap.h"

#define TLBT_T int
#define TLBT_COMPARE(a, b) (a - b)
#include "../src/heap.h"

#define TLBT_T int
#define TLBT_COMPARE(a, b) (a - b)
#define TLBT_STATIC
#include "../src/heap.h"

// same type with different name should be fine
#define TLBT_T int
#define TLBT_T_NAME integer
#define TLBT_COMPARE_REF(a, b) ((*a) - (*b))
#include "../src/heap.h"
#define TLBT_T int
#define TLBT_T_NAME integer
#define TLBT_COMPARE_REF(a, b) ((*a) - (*b))
#include "../src/heap.h"

#define TLBT_T int
#define TLBT_T_NAME integer
#define TLBT_COMPARE_REF(a, b) ((*a) - (*b))
#define TLBT_STATIC
#include "../src/heap.h"

// different type obviously as well
#define TLBT_T float *
#define TLBT_T_NAME floatptr
#define TLBT_COMPARE(a, b) ((int)(*a - *b))
#include "../src/heap.h"
#define TLBT_T float *
#define TLBT_T_NAME floatptr
#define TLBT_COMPARE(a, b) ((int)(*a - *b))
#include "../src/heap.h"

#define TLBT_T float *
#define TLBT_T_NAME floatptr
#define TLBT_COMPARE(a, b) ((int)(*a - *b))
#define TLBT_STATIC
#include "../src/heap.h"

// max heap
#define TLBT_T float *
#define TLBT_T_NAME floatptr
#define TLBT_COMPARE(a, b) ((int)(*a - *b))
#define TLBT_MAX_HEAP
#include "../src/heap.h"
#define TLBT_T float *
#define TLBT_T_NAME floatptr
#define TLBT_COMPARE(a, b) ((int)(*a - *b))
#define TLBT_MAX_HEAP
#include "../src/heap.h"

#define TLBT_T float *
#define TLBT_T_NAME floatptr
#define TLBT_COMPARE(a, b) ((int)(*a - *b))
#define TLBT_MAX_HEAP
#define TLBT_STATIC
#include "../src/heap.h"

int main(void) {
  return 0;
}
