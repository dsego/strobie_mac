/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


typedef struct {

  char letter;
  char accidental[4];
  char altLetter;
  char altAccidental[4];
  int octave;
  float cents;
  float frequency;

} Note;



// given a frequency, find the closest note in equal temperament
Note Tuning12TET_find(float freq, float pitchStandard, float centsOffset, int transpose);

// find the nearest note from a list of pitches (in cents)
Note Tuning12TET_findNearest(float freq, float* notesInCents, int notesLength, float pitchStandard);

float Tuning12TET_freqToCents(float freq, float pitchStandard);
float Tuning12TET_centsToFreq(float cents, float pitchStandard);
float Tuning12TET_centsMoveToOctave(float cents, int octave);
Note Tuning12TET_centsToNote(float cents, float pitchStandard, float centsOffset, int transpose);