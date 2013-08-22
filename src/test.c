/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/



#include <unistd.h>
#include <stdbool.h>
#include <GL/glfw.h>
#include <OpenGL/gl.h>
#include "Engine.h"
#include "utils.h"


int main() {

  if ( ! glfwInit() ) {
    return -1;
  }

  if ( ! glfwOpenWindow(800, 500, 8, 8, 8, 0, 24, 0, GLFW_WINDOW) ) {
    return -1;
  }



  Engine* engine = Engine_create();
  Engine_startAudio(engine);

  double hz = 0.0;
  double cents = 0.0;
  double err = 0.0;  // error in cents

  Note note;



  // smooth lines
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



  while (glfwGetWindowParam(GLFW_OPENED)) {

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    hz = Engine_estimatePitch(engine);

    // int length = engine->pitch->fftBinCount;
    // int length = engine->pitch->cepBinCount;
    int length = engine->pitch->fftLength;
    double dx = 2.0 / length;
    double x = -1.0;


    glColor3ub(20, 190, 250);

    // glBegin(GL_POINTS);
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < length; ++i) {
      // glVertex2d(x, 0.06 * log10(fft[i]));
      // glVertex2d(x, 20 * log10(magnitude(fft[i])) / length);
      // glVertex2d(x, 0.05 * engine->pitch->powSpectrum[i]);
      // glVertex2d(x, 0.1 * engine->pitch->powCepstrum[i]);
      // glVertex2d(x, 0.0001 * engine->pitch->sdf[i]);
      glVertex2d(x, 0.00005 * engine->pitch->sdf[i]);
      x += dx;
    }

    glEnd();

    glColor3ub(190, 230, 20);
    glBegin(GL_POINTS);

    length = engine->pitch->fftLength;
    // length = engine->pitch->fftLength / 2;
    dx = 2.0 / length;
    x = -1.0;

    for (int i = 0; i < length; ++i) {
      // glVertex2d(x, engine->pitch->fftWindow[i]);
      // glVertex2d(x, engine->pitch->timeData[i] + 0.5);
      // glVertex2d(x, 0.0005 * engine->pitch->sdfData[i]);
      // glVertex2d(x, engine->pitch->timeData[i]);
      x += dx;
    }

    glEnd();




    cents = Tuning12TET_freqToCents(hz, 440.0);
    note = Tuning12TET_centsToNote(cents, 440.0, 0.0, 0);
    err = cents - note.cents;

    printf("%4.4f Hz   %2.2f c           \r", hz, err);
    fflush(stdout);

    glfwSwapBuffers();
    usleep(100000);

  }

  return 0;

}