// multi include without implementation should be fine
#include "../src/bitutils.h"
#include "../src/bitutils.h"

// have to use the macros to see if they would compile. this is not really testing any functionality
int main(void) {
  (void)TLBT_CLEAR_BIT(1, 0);
  (void)TLBT_SET_BIT(1, 0);
  (void)TLBT_CHECK_BIT(1, 0);
  (void)TLBT_COPY_BIT(0, 1, 0);
  (void)TLBT_TOGGLE_BIT(1, 0);
  return 0;
}
