/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <math.h>
#include <string.h>
#include "Tuning.h"
#include <stdio.h>


float Tuning12TET_freqToCents(float freq, float pitchStandard) {

  return 1200.0 * log2(freq / pitchStandard);

}


float Tuning12TET_centsToFreq(float cents, float pitchStandard) {

  return pitchStandard * exp2f(cents / 1200.0);

}


Note Tuning12TET_transpose(Note note, int semitones) {

  return Tuning12TET_moveBySemitones(note, -semitones);

}


Note Tuning12TET_moveBySemitones(Note note, int semitones) {

  return Tuning12TET_moveByCents(note, semitones * 100);

}


Note Tuning12TET_moveByCents(Note note, float cents) {

  return Tuning12TET_centsToNote(note.cents + cents, note.pitchStandard, note.centsOffset);

}


Note Tuning12TET_moveToOctave(Note note, int octave) {

  float octaveDelta = octave - note.octave;
  Note newNote = note;
  newNote.octave = octave;
  newNote.frequency = note.frequency * exp2f(octaveDelta);
  newNote.cents += octaveDelta * 1200.0;
  return newNote;

}


// semitone index ->  C = 0, C# = 1, ... B = 11
Note Tuning12TET_noteFromIndex(int index, int octave, float pitchStandard, float centsOffset) {

  // octave 5  (300-1400 cents)
  int cents = 100 * ((index % 12) + 3);

  // move to desired octave
  cents += (octave - 5) * 1200;

  return Tuning12TET_centsToNote(cents, pitchStandard, centsOffset);

}


Note Tuning12TET_centsToNote(
  float cents,
  float pitchStandard,
  float centsOffset
) {

  int nearest = (int)(float) roundf(cents / 100); // semitones
  int octave  = (int)(float) floorf((nearest / 12.0) + 4.75);

  Note note;
  note.pitchStandard = pitchStandard;
  note.octave        = octave;
  note.centsOffset   = centsOffset;
  note.cents         = nearest * 100 + note.centsOffset;
  note.frequency     = Tuning12TET_centsToFreq(note.cents, note.pitchStandard);

  int index = (nearest % 12) + 9;  // C = 0

  // semitone index from 0 to 11
  if (index < 0) {
    index += 12;
  }
  else if (index > 11) {
    index -= 12;
  }

  // C#, D#, F#, G#, A#
  if (index == 1 || index == 3 || index == 6 || index == 8 || index == 10) {
    note.isSharp = 1;
  }
  else {
    note.isSharp = 0;
  }

  note.index = index;

  return note;

}


Note Tuning12TET_find(
  float freq,
  float pitchStandard,
  float centsOffset
) {

  float cents = Tuning12TET_freqToCents(freq, pitchStandard);
  return Tuning12TET_centsToNote(cents, pitchStandard, centsOffset);

}


// A simple binary search, slightly modified to find the nearest value.
// Note Tuning12TET_findNearest(
//   float freq,
//   float* notesInCents,
//   int notesLength,
//   float pitchStandard
// ) {

//   int cents = Tuning12TET_freqToCents(freq, pitchStandard);
//   int mid = 0;
//   int low = 0;
//   int high = notesLength - 1;

//   while (high - low > 1) {
//     mid = low + ((high - low) / 2);
//     if (notesInCents[mid] >= cents) {
//       high = mid;
//     }
//     else {
//       low = mid;
//     }
//   }

//   cents = fabs(cents);
//   float distanceToLow = fabs(fabs(notesInCents[low]) - cents);
//   float distanceToHigh = fabs(fabs(notesInCents[high]) - cents);

//   // return nearest note
//   if (distanceToLow < distanceToHigh) {
//     return Tuning12TET_centsToNote(notesInCents[low], pitchStandard, 0);
//   }
//   else {
//     return Tuning12TET_centsToNote(notesInCents[high], pitchStandard, 0);
//   }

// }




#undef SHARP_SYMBOL
#undef FLAT_SYMBOL
