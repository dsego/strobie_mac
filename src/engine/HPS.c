
#include "HPS.h"


/*

  http://www.embedded.com/design/embedded/4008835/DSP-Tricks-Frequency-Domain-Windowing

  -------------------------------------------------------
   window             alpha       beta        gamma
  -------------------------------------------------------
  rectangular         1.0         -           -
  hann                0.5         0.5         -
  hamming             0.54        0.46        -
  blackman            0.42        0.5         0.08
  exact blackman      7938/18608  9240/18608  1430/18608
  blackman-harris     0.42323     0.49755     0.07922
  -------------------------------------------------------

 */

// frequency domain five-term blackman window
inline static float complex blackmanFreq(
  const float complex a,
  const float complex b,
  const float complex c,
  const float complex d,
  const float complex e) {

  const float alpha = 0.42;
  const float beta  = 0.5;
  const float gamma = 0.08;
  return alpha * c + 0.5 * (gamma * (a + e) - beta * (b + d));

}



inline static void hps(HPS* self, float *outFreq) {

  // Blackman
  // for (int i = 0; i < self->window.length; ++i) {
    // self->audio.elements[i] *= self->window.elements[i];
  // }

  // FFT
  // ffts_execute(self->fftPlan, self->audio.elements, self->fft.elements);

  // log power spectrum
  // for (int i = 0; i < self->fft.length; ++i) {
  //   self->spectrum.elements[i] = creal(magnitude(self->fft.elements[i])) / 10.0;
  // }


}
