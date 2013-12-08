

#include <stdio.h>
#include <math.h>
#include "resonator.h"
#include "Biquad.h"


#define SAMPLERATE 44100
#define BANDWIDTH 12
#define PI  3.14159265358979323846264338327950288419716939937510582097494459230





int main() {

  int freq = 110;
  const int length = 8192;

  float input[length];
  float output[length];

  for (int i = 0; i < length; ++i) {
    input[i] = sin(2.0 * PI * i * freq / SAMPLERATE);
  }

  Biquad* filter = Biquad_create(1);
  double a[3], b[3];
  resonator(freq, BANDWIDTH, SAMPLERATE, a, b);
  Biquad_setCoefficients(filter, a[0], a[1], a[2], b[1], b[2]);

  Biquad_filter(filter, input, output, length);

  float inputPeak = 0;
  float outputPeak = 0;
  for (int i = 0; i < length; ++i) {
    if (inputPeak < fabs(input[i])) inputPeak = fabs(input[i]);
    if (outputPeak < fabs(output[i])) outputPeak = fabs(output[i]);
  }

  // for (int i = 0; i < length; ++i) {
  //   printf("%i %f\n", i, input[i]);
  // }

  // for (int i = 0; i < length; ++i) {
  //   printf("%i %f\n", i, output[i]);
  // }
  printf("%f     %f\n", inputPeak, outputPeak);

  return 0;

}

