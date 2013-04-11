/*
  Copyright (C) 2012 Davorin Šego
*/

public class Median {

  float[] delayline;
  float[] sorted;
  int state;

  public Median(int length) {
    delayline = new float[length];
    sorted = new float[length];
  }

  public float process(float sample) {
    var result = 0f;

    /* add input to the delay line */
    delayline[state] = sample;

    /* find the median */


    /* decrement state (and wrap) */
    state = state - 1;
    if (state < 0)
      state += delayline.length;



    return result;
  }

  public find_median() {

  }

}
