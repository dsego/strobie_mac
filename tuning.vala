/*
  Copyright (C) 2012 Davorin Šego
*/

namespace Tuning {

  class 12TET {


    /* ?????????????????
    public enum note_name {
      A,
      Ais,
      B,
      C,
      D,
      E,
      F,
      G,
    }*/

    public struct Note {
      public string letter;
      public string sign;
      public string alt_letter;
      public string alt_sign;
      public int octave;
      public double cents;
      public double frequency;
    }

    public static Note find(double freq, double pitch_standard = 440.0) {
      var cents    = freq_to_cents(freq, pitch_standard);
      var semitone = nearest_semitone(cents);
      return cents_to_note(semitone);
    }

    public static double freq_to_cents(double freq, double pitch_standard = 440.0) {
      return 1200.0 * Math.log2((freq / pitch_standard));
    }

    public static double cents_to_freq(double cents, double pitch_standard = 440.0) {
      return pitch_standard * (Math.pow(2.0, cents / 1200.0));
    }

    /* in cents */
    public static int nearest_semitone(double cents) {
      return (int) Math.round(cents / 100.0) * 100;
    }

    public static Note cents_to_note(double cents) {
      int nearest    = (int) Math.round(cents / 100.0);
      int octave     = (int) ((nearest / 12.0) + 4.75);
      Note note      = Note();
      note.octave    = octave;
      note.cents     = cents;
      note.frequency = cents_to_freq(cents);
      note.sign      = "";

      switch (nearest % 12) {
        case 0:
          note.letter = "A";
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
          note.letter = "B";
          break;
        case  3:
        case -9:
          note.letter = "C";
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
          note.letter = "D";
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
          note.letter = "E";
          break;
        case  8:
        case -4:
          note.letter = "F";
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
          note.letter = "G";
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

//    public static Note get_note(string name) {
//      Note note = Note();
//      return note;
//    }

  }

}
