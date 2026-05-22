// multi include with the same type should be fine
#define TLBT_T int
#define TLBT_COMPARE(a, b) ((a) - (b))
#include "../src/deque.h"
#define TLBT_T int
#define TLBT_COMPARE(a, b) ((a) - (b))
#include "../src/deque.h"

#define TLBT_T int
#define TLBT_COMPARE(a, b) ((a) - (b))
#define TLBT_STATIC
#include "../src/deque.h"

// same type with different name should be fine
#define TLBT_T int
#define TLBT_T_NAME integer
#define TLBT_COMPARE_REF(a, b) (*(a) - *(b))
#include "../src/deque.h"
#define TLBT_T int
#define TLBT_T_NAME integer
#define TLBT_COMPARE_REF(a, b) (*(a) - *(b))
#include "../src/deque.h"

#define TLBT_T int
#define TLBT_T_NAME integer
#define TLBT_COMPARE_REF(a, b) (*(a) - *(b))
#define TLBT_STATIC
#include "../src/deque.h"

// different type obviously as well
#define TLBT_T float
#define TLBT_COMPARE(a, b) ((int)(a > b ? 1 : a < b ? -1 : 0))
#include "../src/deque.h"
#define TLBT_T float
#define TLBT_COMPARE(a, b) ((int)(a > b ? 1 : a < b ? -1 : 0))
#include "../src/deque.h"

#define TLBT_T float
#define TLBT_COMPARE(a, b) ((int)(a > b ? 1 : a < b ? -1 : 0))
#define TLBT_STATIC
#include "../src/deque.h"

int main(void) {
  return 0;
}
