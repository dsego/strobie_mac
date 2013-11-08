/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <math.h>
#include <OpenGL/gl.h>
#include "utils.h"
#include "StrobeDisplay.h"

#define MAX_STROBE_COUNT 10


typedef struct {

  GLuint vertexBuffer;
  GLuint colorBuffer;
  GLfloat *vertices;
  GLubyte *colors;
  int count;

} StrobeDisplay;


static StrobeDisplay strobes[MAX_STROBE_COUNT];


void StrobeDisplay_setup(){

  // Disable unnecessary state variables
  glDisable(GL_DITHER);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_FOG);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glPixelZoom(1.0, 1.0);

}


void StrobeDisplay_initScene(Engine *engine, int w, int h) {

  int strobeCount = engine->strobeCount;
  float padding = 2.0f * 2.0f / (float)h; // circa 2px
  float height = ((2.0f + padding) / strobeCount) - padding;
  float y = -1.0f;

  // allocate buffers
  for (int s = 0; s < strobeCount; ++s) {

    // 2 vertices per sample
    //   _________________
    //  |\ |\ |\ |\ |\ |\ |
    //  | \| \| \| \| \| \|
    //   -----------------
    //
    int count = strobes[s].count = engine->strobeLengths[s] * 2;

    // Static position data
    glDeleteBuffers(1, &(strobes[s].vertexBuffer));
    glGenBuffers(1, &(strobes[s].vertexBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].vertexBuffer);

    // allocate GL buffer (x, y for each vertex)
    glBufferData(GL_ARRAY_BUFFER, 2 * count * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    // get pointer to buffer
    strobes[s].vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    // generate vertices
    float x = -1.0f;
    float dx = 2.0f / (engine->strobeLengths[s] - 1);
    int v = 0;

    while (v < 2 * count) {

      strobes[s].vertices[v++] = x;
      strobes[s].vertices[v++] = y;
      strobes[s].vertices[v++] = x;
      strobes[s].vertices[v++] = y + height;
      x += dx;

    }

    y += height + padding;

    // Dynamic color data
    glDeleteBuffers(1, &(strobes[s].colorBuffer));
    glGenBuffers(1, &(strobes[s].colorBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    strobes[s].colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  }
}


void StrobeDisplay_drawScene(Engine *engine) {

  int fresh = Engine_readStrobes(engine);

  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT);


  for (int s = 0; s < engine->strobeCount; ++s) {

    if (fresh) {

      int start0  = engine->config->strobes[s].color1[0];
      int start1  = engine->config->strobes[s].color1[1];
      int start2  = engine->config->strobes[s].color1[2];

      int end0 = engine->config->strobes[s].color2[0];
      int end1 = engine->config->strobes[s].color2[1];
      int end2 = engine->config->strobes[s].color2[2];

      float scale0  = end0 - start0;
      float scale1  = end1 - start1;
      float scale2  = end2 - start2;


      // TODO : add hysteresis !
      float gain = 1000.0;

      int i = engine->strobeLengths[s];
      int c = 0;

      while (c < 3 * strobes[s].count) {

        i -= 1;

        float value = engine->strobeBuffers[s].elements[i];

        // value = 1.0 / (1.0 + exp(-gain * value));
        value = (value * gain + 1) * 0.5;

        if (value < 0.0) {
          value = 0.0;
        }
        else if (value > 1.0) {
          value = 1.0;
        }

        GLubyte color[3];
        // hsv2rgb(hsv, rgb);

        color[0] = (GLubyte) (start0 + scale0 * value);
        color[1] = (GLubyte) (start1 + scale1 * value);
        color[2] = (GLubyte) (start2 + scale2 * value);

        // color (RGB)
        strobes[s].colors[c++] = color[0];
        strobes[s].colors[c++] = color[1];
        strobes[s].colors[c++] = color[2];

        // color (RGB)
        strobes[s].colors[c++] = color[0];
        strobes[s].colors[c++] = color[1];
        strobes[s].colors[c++] = color[2];

      }

    }

    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].colorBuffer);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, strobes[s].count);

  }

}
