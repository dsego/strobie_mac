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
  double err = 0.0;

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

    // int length = engine->pitch->sdf.length;
    int length = engine->pitch->powCepstrum.length;
    double dx = 2.0 / (double)length;
    double x = -1.0;

    glColor3ub(20, 190, 250);
    glBegin(GL_POINTS);
    // glBegin(GL_LINE_STRIP);

    for (int i = 0; i < length; ++i) {
      glVertex2d(x, 0.05 * engine->pitch->powCepstrum.elements[i]);
      // glVertex2d(x, 0.05 * engine->pitch->powSpectrum.elements[i]);
      // glVertex2d(x, 0.5 * engine->pitch->sdf.elements[i]);
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