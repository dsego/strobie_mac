/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


typedef struct {

  char letter;
  char accidental[4];
  char altLetter;
  char altAccidental[4];
  int octave;
  double cents;
  double frequency;

} Note;



// given a frequency, find the closest note in equal temperament
Note Tuning12TET_find(double freq, double pitchStandard, double centsOffset, int transpose);

// find the nearest note from a list of pitches (in cents)
Note Tuning12TET_findNearest(double freq, double* notesInCents, int notesLength, double pitchStandard);

double Tuning12TET_freqToCents(double freq, double pitchStandard);
double Tuning12TET_centsToFreq(double cents, double pitchStandard);
double Tuning12TET_centsMoveToOctave(double cents, int octave);
Note Tuning12TET_centsToNote(double cents, double pitchStandard, double centsOffset, int transpose);