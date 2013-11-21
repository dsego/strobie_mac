/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/



#include <unistd.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include "Engine.h"
#include "utils.h"

#include <GLFW/glfw3.h>

int main() {

  GLFWwindow* window;

  if (!glfwInit()) {
    return -1;
  }

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "test", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);


  Engine* engine = Engine_create();
  Engine_setInputDevice(engine, 0, 44100);

  double hz = 0.0;
  double cents = 0.0;
  double err = 0.0;

  Note note;

  // smooth lines
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {


    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // hz = Engine_estimatePitch(engine);

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

    glfwSwapBuffers(window);

    glfwPollEvents();

    usleep(100000);

  }

  glfwTerminate();
  return 0;

}

