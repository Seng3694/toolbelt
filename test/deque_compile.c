// multi include with the same type should be fine
#define TLBT_T int
#include "../src/deque.h"
#define TLBT_T int
#include "../src/deque.h"

#define TLBT_T int
#define TLBT_STATIC
#include "../src/deque.h"

// same type with different name should be fine
#define TLBT_T int
#define TLBT_T_NAME integer
#include "../src/deque.h"
#define TLBT_T int
#define TLBT_T_NAME integer
#include "../src/deque.h"

#define TLBT_T int
#define TLBT_T_NAME integer
#define TLBT_STATIC
#include "../src/deque.h"

// different type obviously as well
#define TLBT_T float
#include "../src/deque.h"
#define TLBT_T float
#include "../src/deque.h"

#define TLBT_T float
#define TLBT_STATIC
#include "../src/deque.h"

int main(void) {
  return 0;
}
