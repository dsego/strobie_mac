// Copyright (c) 2013 Davorin Šego. All rights reserved.

#include "unity.h"
#include "trial.h"


void setUp(void) {
}
void tearDown(void) {
}


void testTrial() {

  int daysLeft = 0;

  daysLeft = checkTrialExpiry(genTimestamp(), genTimestamp(), 15);
  TEST_ASSERT_EQUAL_INT(15, daysLeft);

  // 1984-5-9 8:00, 1984-5-19 10:00
  daysLeft = checkTrialExpiry(452937600, 453808800, 15);
  TEST_ASSERT_EQUAL_INT(5, daysLeft);

  // 1984-5-19 10:00, 1984-5-9 8:00
  daysLeft = checkTrialExpiry(453808800, 452937600, 15);
  TEST_ASSERT_EQUAL_INT(15, daysLeft);

  // 1984-5-9 8:00, 1984-5-26 23:00
  daysLeft = checkTrialExpiry(452937600, 454460400, 15);
  TEST_ASSERT_EQUAL_INT(-2, daysLeft);

}


