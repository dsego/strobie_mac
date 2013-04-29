/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


/*
  Decibels relative to full scale (dBFS), measures decibel amplitude levels in digital systems
    which have a defined maximum available peak level.
 */
double to_dBFS(double value);

double from_dBFS(double value);

double min(int a, int b);

double minf(double a, double b);

double max(int a, int b);

double maxf(double a, double b);

double clamp(double val, double lower, double upper);

double findPeak(float* buffer, int bufferLength);