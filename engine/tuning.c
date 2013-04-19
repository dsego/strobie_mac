/*
  Copyright (C) 2012 Davorin Šego
*/


#include <math.h>
#include <string.h>
#include "tuning.h"


double Tuning12TET_freq_to_cents(double freq, double pitch_standard)
{
  return 1200.0 * log2(freq / pitch_standard);
}

double Tuning12TET_cents_to_freq(double cents, double pitch_standard)
{
  return pitch_standard * pow(2.0, cents / 1200.0);
}

Note Tuning12TET_cents_to_note(double cents, double pitch_standard, double cents_offset, int transpose)
{
  int nearest       = (int) round(cents / 100.0);
  int transposed    = nearest + transpose;
  int octave        = (int) ((transposed / 12.0) + 4.75);

  Note note;
  note.octave    = octave;
  note.cents     = nearest * 100.0 + cents_offset;
  note.frequency = Tuning12TET_cents_to_freq(note.cents, pitch_standard);

  switch (transposed % 12) {
    case 0:
      note.letter     = 'A';
      note.alt_letter = 'A';
      break;
    case   1:
    case -11:
      note.letter     = 'A';
      note.alt_letter = 'B';
      strcpy(note.sign, "♯");
      strcpy(note.alt_sign, "♭");
      break;
    case   2:
    case -10:
      note.letter     = 'B';
      note.alt_letter = 'B';
      break;
    case  3:
    case -9:
      note.letter     = 'C';
      note.alt_letter = 'C';
      break;
    case  4:
    case -8:
      note.letter     = 'C';
      note.alt_letter = 'D';
      strcpy(note.sign, "♯");
      strcpy(note.alt_sign, "♭");
      break;
    case  5:
    case -7:
      note.letter     = 'D';
      note.alt_letter = 'D';
      break;
    case  6:
    case -6:
      note.letter     = 'D';
      note.alt_letter = 'E';
      strcpy(note.sign, "♯");
      strcpy(note.alt_sign, "♭");
      break;
    case  7:
    case -5:
      note.letter     = 'E';
      note.alt_letter = 'E';
      break;
    case  8:
    case -4:
      note.letter     = 'F';
      note.alt_letter = 'F';
      break;
    case  9:
    case -3:
      note.letter     = 'F';
      note.alt_letter = 'G';
      strcpy(note.sign, "♯");
      strcpy(note.alt_sign, "♭");
      break;
    case 10:
    case -2:
      note.letter     = 'G';
      note.alt_letter = 'G';
      break;
    case 11:
    case -1:
      note.letter     = 'G';
      note.alt_letter = 'A';
      strcpy(note.sign, "♯");
      strcpy(note.alt_sign, "♭");
      break;
  }

  return note;
}

Note Tuning12TET_find(double freq, double pitch_standard, double cents_offset, int transpose)
{
  double cents = Tuning12TET_freq_to_cents(freq, pitch_standard);
  return Tuning12TET_cents_to_note(cents, pitch_standard, cents_offset, transpose);
}

// A simple binary search, slightly modified to find the nearest value.
Note Tuning12TET_find_nearest(double freq, double* notes, int notes_len, double pitch_standard)
{
  int cents = Tuning12TET_freq_to_cents(freq, pitch_standard);
  int mid = 0;
  int low = 0;
  int high = notes_len - 1;
  while (high - low > 1) {
    mid = low + ((high - low) / 2);
    if (notes[mid] >= cents)
      high = mid;
    else
      low = mid;
  }
  cents = fabs(cents);

  if (fabs(fabs(notes[low]) - cents) < fabs(fabs(notes[high]) - cents))
    return Tuning12TET_cents_to_note(notes[low], pitch_standard, 0, 0);
  else
    return Tuning12TET_cents_to_note(notes[high], pitch_standard, 0, 0);
}