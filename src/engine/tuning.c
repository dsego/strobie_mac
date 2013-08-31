/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <math.h>
#include <string.h>
#include "Tuning.h"


float Tuning12TET_freqToCents(float freq, float pitchStandard) {

  return 1200.0 * log2(freq / pitchStandard);

}


float Tuning12TET_centsToFreq(float cents, float pitchStandard) {

  return pitchStandard * pow(2.0, cents / 1200.0);

}


float Tuning12TET_centsMoveToOctave(float cents, int octave) {

  if (octave < 0 || octave > 8) return -1;

  int origOctave = (int) ((cents / 1200.0) + 4.75);
  float transpose = (octave - origOctave) * 1200.0;
  return cents + transpose;

}


Note Tuning12TET_centsToNote(
  float cents,
  float pitchStandard,
  float centsOffset,
  int transpose
) {

  int nearest    = (int) round(cents / 100.0);
  int transposed = nearest + transpose;
  int octave     = (int) ((transposed / 12.0) + 4.75);

  Note note;
  note.octave    = octave;
  note.cents     = nearest * 100.0 + centsOffset;
  note.frequency = Tuning12TET_centsToFreq(note.cents, pitchStandard);

  switch (transposed % 12) {
    case 0:
      note.letter     = 'A';
      note.altLetter  = 'A';
      break;
    case   1:
    case -11:
      note.letter     = 'A';
      note.altLetter  = 'B';
      strcpy(note.accidental, "♯");
      strcpy(note.altAccidental, "♭");
      break;
    case   2:
    case -10:
      note.letter     = 'B';
      note.altLetter  = 'B';
      break;
    case  3:
    case -9:
      note.letter     = 'C';
      note.altLetter  = 'C';
      break;
    case  4:
    case -8:
      note.letter     = 'C';
      note.altLetter  = 'D';
      strcpy(note.accidental, "♯");
      strcpy(note.altAccidental, "♭");
      break;
    case  5:
    case -7:
      note.letter     = 'D';
      note.altLetter  = 'D';
      break;
    case  6:
    case -6:
      note.letter     = 'D';
      note.altLetter  = 'E';
      strcpy(note.accidental, "♯");
      strcpy(note.altAccidental, "♭");
      break;
    case  7:
    case -5:
      note.letter     = 'E';
      note.altLetter  = 'E';
      break;
    case  8:
    case -4:
      note.letter     = 'F';
      note.altLetter  = 'F';
      break;
    case  9:
    case -3:
      note.letter     = 'F';
      note.altLetter  = 'G';
      strcpy(note.accidental, "♯");
      strcpy(note.altAccidental, "♭");
      break;
    case 10:
    case -2:
      note.letter     = 'G';
      note.altLetter  = 'G';
      break;
    case 11:
    case -1:
      note.letter     = 'G';
      note.altLetter  = 'A';
      strcpy(note.accidental, "♯");
      strcpy(note.altAccidental, "♭");
      break;
  }

  return note;

}


Note Tuning12TET_find(
  float freq,
  float pitchStandard,
  float centsOffset,
  int transpose
) {

  float cents = Tuning12TET_freqToCents(freq, pitchStandard);
  return Tuning12TET_centsToNote(cents, pitchStandard, centsOffset, transpose);

}


// A simple binary search, slightly modified to find the nearest value.
Note Tuning12TET_findNearest(
  float freq,
  float* notesInCents,
  int notesLength,
  float pitchStandard
) {

  int cents = Tuning12TET_freqToCents(freq, pitchStandard);
  int mid = 0;
  int low = 0;
  int high = notesLength - 1;

  while (high - low > 1) {
    mid = low + ((high - low) / 2);
    if (notesInCents[mid] >= cents) {
      high = mid;
    }
    else {
      low = mid;
    }
  }

  cents = fabs(cents);
  float distanceToLow = fabs(fabs(notesInCents[low]) - cents);
  float distanceToHigh = fabs(fabs(notesInCents[high]) - cents);

  // return nearest note
  if (distanceToLow < distanceToHigh) {
    return Tuning12TET_centsToNote(notesInCents[low], pitchStandard, 0, 0);
  }
  else {
    return Tuning12TET_centsToNote(notesInCents[high], pitchStandard, 0, 0);
  }

}