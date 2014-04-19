// Copyright (c) 2013 Davorin Šego. All rights reserved.

#include <math.h>
#include <time.h>



int checkTrialExpiry(int then, int now, int trialDays) {
  if (then >= now) {
    return trialDays;
  }
  int elapsed = now - then;
  int trialDuration = 60 * 60 * 24 * trialDays;
  double timeLeft = trialDuration - elapsed;
  int daysLeft = (int)(double) ceil(timeLeft / (24 * 60 * 60));
  return daysLeft;
}


int genTimestamp() {
  return (int)time(NULL);
}
