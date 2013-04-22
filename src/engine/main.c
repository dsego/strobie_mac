//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <unistd.h>
#include <GL/glfw.h>
#include <OpenGL/gl.h>
#include "engine.h"


int main(int argc, char *argv[])
{
  Engine* engine = Engine_create();
  Engine_initAudio(engine);
  Note note;
  note.frequency = 440;

  double pitch;

  if (!glfwInit())
    return -1;

  if (!glfwOpenWindow(500, 500, 8, 8, 8, 0, 24, 0, GLFW_WINDOW))
    return -1;

  double dx = 2.0 / engine->config->fftLength;
  double x  = -1;

  while (glfwGetWindowParam(GLFW_OPENED)) {
    AudioFeed_read(engine->audioFeed, engine->audioBuffer, engine->config->fftLength);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(1.0);
    glColor3f(0.9, 0.0, 0.0);
    glBegin(GL_POINTS);

    x = -1;
    for (int i = 0; i < engine->config->fftLength; ++i) {
      // glVertex2d(x, -engine->audio_buffer[i]);
      // glVertex2d(x, -engine->pitch_estimation->autocorr_data[i] * 10.0);
      x += dx;
    }
    glEnd();
    glfwSwapBuffers();
    // glFlush();
    // Engine_read_strobes(engine, note);
    pitch = Engine_estimatePitch(engine);
    printf("%.2f\n", pitch);
    usleep(20000);
  }
  return 0;
}


//   public void draw() {
//     window_background();

//     if (instrument_mode == true)
//       note = Tuning.12TET.find_nearest(pitch, config.ins_notes, config.pitch_standard);
//     else
//       note = Tuning.12TET.find(pitch, config.pitch_standard, config.cents_offset, config.transpose);

//     if (config.display_flats)
//       render_note(note.alt_letter, note.alt_sign, note.octave.to_string());
//     else
//       render_note(note.letter, note.sign, note.octave.to_string());

//     // float h = height / strobe_signals.length;

//     lock (strobe_signals) {
//       strobe_display(strobe_signals, config.strobe_gain, config.strobe_background, config.strobe_foreground);
//     }


//     // to_dbfs(peak);

//     // draw_signal(pitch_estimation.autocorr_data, 0.000005f);

//     // /* center */
//     // context.translate(width / 2, height / 2);

//     // /* keep aspect ratio */
//     // if (height > width)
//     //   context.scale(width, width);
//     // else
//     //   context.scale(height, height);

//     // draw_wheel(strobe_signals[2].data, 2000f);
//     // context.restore();
//   }




//   // protected process_events() {
//   //   base.process_events();

//   //   if (manual_mode == true) {
//   //     if (get_key(Key.UP)) {
//   //       pitch =
//   //     }
//   //   }
//   // }

//   public static int main(string[] args) {
//     if (!Thread.supported()) {
//       stderr.printf ("Cannot run without thread support.\n");
//       return 1;
//     }
//     var app = new App();
//     Thread<int> thread_a = new Thread<int>("a", app.find_pitch);
//     Thread<int> thread_b = new Thread<int>("b", app.strobe);

//     while (!app.quit) {
//       app.draw();
//       app.process_events();
//       Thread.usleep(10000);
//     }
//     return 0;
//   }

// }