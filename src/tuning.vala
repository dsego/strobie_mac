/*
  Copyright (C) 2012 Davorin Šego
*/

namespace Tuning {

  class 12TET {

    public struct Note {
      public string letter;
      public string sign;
      public string alt_letter;
      public string alt_sign;
      public int octave;
      public float cents;
      public float frequency;
    }

    public static Note find(float freq, float pitch_standard = 440f, float cents_offset = 0.0f, int transpose = 0) {
      var cents = freq_to_cents(freq, pitch_standard);
      return cents_to_note(cents, pitch_standard, cents_offset, transpose);
    }

    public static float freq_to_cents(float freq, float pitch_standard = 440f) {
      return (float) (1200.0f * Math.log2((freq / pitch_standard)));
    }

    public static float cents_to_freq(float cents, float pitch_standard = 440f) {
      return (float) (pitch_standard * (Math.pow(2.0, cents / 1200.0f)));
    }

    public static Note cents_to_note(float cents, float pitch_standard = 440f, float cents_offset = 0f, int transpose = 0) {
      var nearest       = (int) Math.round(cents / 100.0);
      var transposed    = nearest + transpose;
      var octave        = (int) ((transposed / 12.0) + 4.75);
      Note note         = Note();
      note.octave       = octave;
      note.cents        = nearest * 100f + cents_offset;
      note.frequency    = cents_to_freq(note.cents, pitch_standard);
      note.sign         = "";
      note.alt_sign     = "";

      switch (transposed % 12) {
        case 0:
          note.letter     = "A";
          note.alt_letter = "A";
          break;
        case   1:
        case -11:
          note.letter     = "A";
          note.sign       = "♯";
          note.alt_letter = "B";
          note.alt_sign   = "♭";
          break;
        case   2:
        case -10:
          note.letter     = "B";
          note.alt_letter = "B";
          break;
        case  3:
        case -9:
          note.letter     = "C";
          note.alt_letter = "C";
          break;
        case  4:
        case -8:
          note.letter     = "C";
          note.sign       = "♯";
          note.alt_letter = "D";
          note.alt_sign   = "♭";
          break;
        case  5:
        case -7:
          note.letter     = "D";
          note.alt_letter = "D";
          break;
        case  6:
        case -6:
          note.letter     = "D";
          note.sign       = "♯";
          note.alt_letter = "E";
          note.alt_sign   = "♭";
          break;
        case  7:
        case -5:
          note.letter     = "E";
          note.alt_letter = "E";
          break;
        case  8:
        case -4:
          note.letter     = "F";
          note.alt_letter = "F";
          break;
        case  9:
        case -3:
          note.letter     = "F";
          note.sign       = "♯";
          note.alt_letter = "G";
          note.alt_sign   = "♭";
          break;
        case 10:
        case -2:
          note.letter     = "G";
          note.alt_letter = "G";
          break;
        case 11:
        case -1:
          note.letter     = "G";
          note.sign       = "♯";
          note.alt_letter = "A";
          note.alt_sign   = "♭";
          break;
      }

      return note;
    }

    /* A simple binary search, slightly modified to find the nearest value.
       The notes parameter holds note values in cents.
    */
    public static Note find_nearest(float freq, float[] notes, float pitch_standard) {
      var cents = freq_to_cents(freq, pitch_standard);
      int mid = 0;
      int low = 0;
      int high = notes.length - 1;
      while (high - low > 1) {
        mid = low + ((high - low) / 2);
        if (notes[mid] >= cents)
          high = mid;
        else
          low = mid;
      }
      cents = Math.fabsf(cents);

      if (Math.fabsf(Math.fabsf(notes[low]) - cents) < Math.fabsf(Math.fabsf(notes[high]) - cents))
        return cents_to_note(notes[low], pitch_standard);
      else
        return cents_to_note(notes[high], pitch_standard);
    }

  }

}