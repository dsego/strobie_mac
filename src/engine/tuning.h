/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#ifndef TUNING_H
#define TUNING_H



typedef struct {

  int index;    // C = 0, C# = 1, ... B = 11
  int isSharp;
  int octave;
  float cents;
  float frequency;
  float pitchStandard;
  float centsOffset;

} Note;


Note Tuning12TET_noteFromIndex(int index, int octave, float pitchStandard, float centsOffset);
Note Tuning12TET_centsToNote(float cents, float pitchStandard, float centsOffset);

// given a frequency, find the closest note in equal temperament
Note Tuning12TET_find(float freq, float pitchStandard, float centsOffset);

// find the nearest note from a list of pitches (in cents)
// Note Tuning12TET_findNearest(float freq, float* notesInCents, int notesLength, float pitchStandard);

Note Tuning12TET_transpose(Note note, int semitones);
Note Tuning12TET_moveToOctave(Note note, int octave);
Note Tuning12TET_moveBySemitones(Note note, int semitones);
Note Tuning12TET_moveByCents(Note note, float cents);
Note Tuning12TET_transpose(Note note, int semitones);


float Tuning12TET_freqToCents(float freq, float pitchStandard);
float Tuning12TET_centsToFreq(float cents, float pitchStandard);

#endif