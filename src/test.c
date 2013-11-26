/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/



#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include "Engine.h"
#include "utils.h"




static void speedTest(Engine *engine) {

  float start = (float)(int) clock() / CLOCKS_PER_SEC;
  float duration = 0;
  float minDuration = 100000000;
  float maxDuration = 0;

  float freq, clarity;

  AudioFeed_read(engine->audioFeed, engine->audioBuffer.elements, engine->audioBuffer.length);

  for (int i=0; i < 10000; ++i) {
    duration = (float)(int)clock() / CLOCKS_PER_SEC;
    Pitch_estimate(engine->pitch, engine->audioBuffer.elements, &freq, &clarity);
    duration = (float)(int)clock() / CLOCKS_PER_SEC - duration;
    if (duration > maxDuration) {
      maxDuration = duration;
    }
    else if (duration < minDuration) {
      minDuration = duration;
    }
  }

  float overallDuration = (float)(int)clock() / CLOCKS_PER_SEC - start;
  float avgDuration = overallDuration / 10000.0;

  printf(
    "speed (executions per second) avg %8.2f   max %8.2f  min %8.2f    \n",
    1.0/avgDuration,
    1.0/minDuration,
    1.0/maxDuration
  );

}



int main() {


  GLFWwindow* window;
  if (!glfwInit()) {
    return -1;
  }
  window = glfwCreateWindow(640, 480, "test", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  Engine* engine = Engine_create();

  // 0 - default input, 2 - sound flower
  Engine_setInputDevice(engine, 2, 44100);
  float freq = 0.0;
  float clarity = 0.0;
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  // speedTest(engine);

  while (!glfwWindowShouldClose(window)) {

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    AudioFeed_read(engine->audioFeed, engine->audioBuffer.elements, engine->audioBuffer.length);
    Pitch_estimate(engine->pitch, engine->audioBuffer.elements, &freq, &clarity);

    int length = engine->pitch->nsdf.length / 4;
    float *nsdf = engine->pitch->nsdf.elements;
    // int length = engine->pitch->powCepstrum.length;
    double dx = 2.0 / (double)length;
    double x = -1.0;

    // printf("%7.2f     %4.2f   \r", freq, clarity);
    // fflush(stdout);

    glColor3ub(20, 190, 250);
    glPointSize(1);
    glBegin(GL_POINTS);
    // glBegin(GL_LINE_STRIP);

    for (int i = 0; i < length; ++i) {
      // glVertex2d(x, 0.05 * engine->pitch->powCepstrum.elements[i]);
      // glVertex2d(x, 0.05 * engine->pitch->powSpectrum.elements[i]);
      glVertex2d(x, 0.5 * nsdf[i]);
      x += dx;
    }
    glEnd();

    glPointSize(7);
    glColor3ub(220, 30, 20);
    glBegin(GL_POINTS);
      glVertex2d( dx * 44100.0/freq - 1.0, 0.5 * clarity);
    glEnd();

    glfwSwapBuffers(window);
    glfwPollEvents();
    usleep(20000);

  }

  Engine_destroy(engine);

  glfwTerminate();
  return 0;


}

