// multi include should be fine
#include "../src/assert.h"
#include "../src/assert.h"

int main(void) {
  tlbt_assert(1);
  tlbt_assert_msg(1, "test");
  tlbt_assert_fmt(1, "test %d", 0);
  return 0;
}
