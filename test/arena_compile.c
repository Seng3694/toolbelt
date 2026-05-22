// multi include without implementation should be fine
#include "../src/arena.h"
#include "../src/arena.h"

#define TLBT_IMPLEMENTATION
#include "../src/arena.h"

int main(void) {
  return 0;
}
