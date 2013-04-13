double to_dbfs(double value)
{
  return 20 * log10(value);
}

double from_dbfs(double value)
{
  return pow(10, value / 20);
}

double min(double a, double b)
{
  return (a < b) ? a : b;
}

double max(double a, double b)
{
  return (a > b) ? a : b;
}

double clamp(double val, double lower, double upper)
{
  return min(max(val, lower), upper);
}

double find_peak(double* buffer, int buffer_len) {
  double peak = 0;
  for (int i = 0; i < buffer_len; ++i) {
    if (buffer[i] < -peak)
      peak = -buffer[i];
    else if (buffer[i] > peak)
      peak = buffer[i];
  }
  return peak;
}