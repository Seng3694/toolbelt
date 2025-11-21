#include "common.h"
#include "../src/bitutils.h"

int main(void) {
  TLBT_TEST_ASSERT(TLBT_SET_BIT(0, 0) == 1, "setting the first bit of 0 should result in 1");
  TLBT_TEST_ASSERT(TLBT_SET_BIT(0, 7) == 128, "setting the eigth bit of 0 should result in 128");
  TLBT_TEST_ASSERT(TLBT_SET_BIT(1, 0) == 1, "setting the first bit of 1 should result in 1");
  TLBT_TEST_ASSERT(TLBT_SET_BIT(255, 0) == 255, "setting the first bit of 255 should result in 255");

  TLBT_TEST_ASSERT(TLBT_CLEAR_BIT(0, 0) == 0, "clearing the first bit of 0 should result in 0");
  TLBT_TEST_ASSERT(TLBT_CLEAR_BIT(128, 7) == 0, "clearing the eigth bit of 128 should result in 0");
  TLBT_TEST_ASSERT(TLBT_CLEAR_BIT(1, 0) == 0, "clearing the first bit of 1 should result in 0");
  TLBT_TEST_ASSERT(TLBT_CLEAR_BIT(255, 0) == 254, "clearing the first bit of 255 should result in 254");

  TLBT_TEST_ASSERT(TLBT_TOGGLE_BIT(0, 0) == 1, "toggling the first bit of 0 should result in 1");
  TLBT_TEST_ASSERT(TLBT_TOGGLE_BIT(0, 7) == 128, "toggling the eigth bit of 0 should result in 128");
  TLBT_TEST_ASSERT(TLBT_TOGGLE_BIT(128, 7) == 0, "toggling the eigth bit of 128 should result in 0");
  TLBT_TEST_ASSERT(TLBT_TOGGLE_BIT(1, 0) == 0, "toggling the first bit of 1 should result in 0");
  TLBT_TEST_ASSERT(TLBT_TOGGLE_BIT(255, 0) == 254, "toggling the first bit of 255 should result in 254");
  TLBT_TEST_ASSERT(TLBT_TOGGLE_BIT(254, 0) == 255, "toggling the first bit of 254 should result in 255");

  TLBT_TEST_ASSERT(TLBT_CHECK_BIT(0, 0) == 0, "checking the first bit of 0 should result in 0");
  TLBT_TEST_ASSERT(TLBT_CHECK_BIT(0, 7) == 0, "checking the eigth bit of 0 should result in 0");
  TLBT_TEST_ASSERT(TLBT_CHECK_BIT(128, 0) == 0, "checking the first bit of 128 should result in 0");
  TLBT_TEST_ASSERT(TLBT_CHECK_BIT(128, 7) == 1, "checking the eigth bit of 128 should result in 1");
  TLBT_TEST_ASSERT(TLBT_CHECK_BIT(255, 0) == 1, "checking the first bit of 255 should result in 1");
  TLBT_TEST_ASSERT(TLBT_CHECK_BIT(255, 7) == 1, "checking the eigth bit of 255 should result in 1");

  TLBT_TEST_ASSERT(TLBT_COPY_BIT(0, 1, 0) == 1, "copying the first bit from 1 to 0 should result in 1");
  TLBT_TEST_ASSERT(TLBT_COPY_BIT(1, 1, 0) == 1, "copying the first bit from 1 to 1 should result in 1");
  TLBT_TEST_ASSERT(TLBT_COPY_BIT(2, 1, 0) == 3, "copying the first bit from 1 to 2 should result in 3");
  TLBT_TEST_ASSERT(TLBT_COPY_BIT(254, 3, 0) == 255, "copying the first bit from 3 to 254 should result in 255");

  TLBT_TEST_DONE();
}

