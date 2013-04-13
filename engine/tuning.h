/*
  Copyright (C) 2012 Davorin Šego
*/

typedef struct {
  char letter;
  char sign[4];
  char alt_letter;
  char alt_sign[4];
  int octave;
  double cents;
  double frequency;
} Note;


double Tuning12TET_freq_to_cents(double freq, double pitch_standard);
double Tuning12TET_cents_to_freq(double cents, double pitch_standard);
Note Tuning12TET_cents_to_note(double cents, double pitch_standard, double cents_offset, int transpose);
Note Tuning12TET_find(double freq, double pitch_standard, double cents_offset, int transpose);
Note Tuning12TET_find_nearest(double freq, double* notes, int notes_len, double pitch_standard);