// Copyright (c) Davorin Šego. All rights reserved.


#include <math.h>
#include <string.h>
#include <OpenGL/gl3.h>
#include "StrobeDisplay.h"
#include "shader.h"
#include "sources.h"

#define MAX_STROBE_COUNT 10


typedef struct {
  GLuint vao;
  const Buffer *dataBuffer;
  GLuint posBuffer;
  GLuint colorBuffer;
  GLfloat *positions;
  GLubyte *colors;
  float peak;
  int highlighted;
  int count;
} StrobeBand;


static StrobeBand bands[MAX_STROBE_COUNT];

static GLuint shadowElements[] = {
  0, 1, 2, 1, 2, 3,       // top shadow
  4, 5, 6, 5, 6, 7,       // bottom shadow
  8, 9, 10, 9, 10, 11,    // left shadow
  12, 13, 14, 13, 14, 15  // right shadow
};

static GLuint sceneShader;
static GLuint textureShader;
static GLuint shadowVao;
static GLuint shadowEbo;
static GLuint shadowVbo;
static GLuint shadowTexture;



static inline void initShadow() {
  glGenVertexArrays(1, &shadowVao);
  glGenBuffers(1, &shadowVbo);
  glGenBuffers(1, &shadowEbo);
  glGenTextures(1, &shadowTexture);
}


static inline void deleteShadow() {
  glDeleteVertexArrays(1, &shadowVao);
  glDeleteBuffers(1, &shadowVbo);
  glDeleteBuffers(1, &shadowEbo);
  glDeleteTextures(1, &shadowTexture);
}


static inline void initBandBuffers(Buffer *buffer, StrobeBand *band) {

  glGenVertexArrays(1, &band->vao);
  glGenBuffers(1, &band->posBuffer);
  glGenBuffers(1, &band->colorBuffer);
  glBindVertexArray(band->vao);

  // 2 vertices per sample
  int count = band->count = buffer->count * 2;
  band->dataBuffer = buffer;

  // Static position data
  glBindBuffer(GL_ARRAY_BUFFER, band->posBuffer);
  glBufferData(GL_ARRAY_BUFFER, 2 * count * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
  band->positions = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  GLint posAttrib = glGetAttribLocation(sceneShader, "position");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(posAttrib);

  // Dynamic color data
  glBindBuffer(GL_ARRAY_BUFFER, band->colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, 4 * count * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
  band->colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  GLint colAttrib = glGetAttribLocation(sceneShader, "color");
  glVertexAttribPointer(colAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
  glEnableVertexAttribArray(colAttrib);

}


static inline void deleteBandBuffers(StrobeBand *band) {
  glDeleteVertexArrays(1, &bands->vao);
  glDeleteBuffers(1, &bands->posBuffer);
  glDeleteBuffers(1, &bands->colorBuffer);
}


static inline void refreshShadow(int screenWidth, int screenHeight) {

  // 9px shadow
  float x = 1.f - 10.f * 2.f / (float)screenWidth;
  float y = 1.f - 10.f * 2.f / (float)screenHeight;

  glBindVertexArray(shadowVao);

  // position + texture coordinates (1D)
  GLfloat vertices[] = {
    // top shadow
    -1.f , 1.f  , 0.f ,
    1.f  , 1.f  , 0.f ,
    -1.f , y    , 1.f ,
    1.f  , y    , 1.f ,

    // bottom shadow
    -1.f , -y   , 1.f ,
    1.f  , -y   , 1.f ,
    -1.f , -1.f , 0.f ,
    1.f  , -1.f , 0.f ,

    // left shadow
    -1.f , 1.f  , 0.f ,
    -x   , 1.f  , 1.f ,
    -1.f , -1.f , 0.f ,
    -x   , -1.f , 1.f ,

    // right shadow
    x    , 1.f  , 1.f ,
    1.f  , 1.f  , 0.f ,
    x    , -1.f , 1.f ,
    1.f  , -1.f , 0.f
  };

  glBindBuffer(GL_ARRAY_BUFFER, shadowVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shadowElements), shadowElements, GL_STATIC_DRAW);

  GLint posAttrib = glGetAttribLocation(textureShader, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

  GLint texAttrib = glGetAttribLocation(textureShader, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 2 * sizeof(GLfloat));

  float pixels[] = {
    0, 0, 0, 0.6275,
    0, 0, 0, 0.5451,
    0, 0, 0, 0.4627,
    0, 0, 0, 0.3765,
    0, 0, 0, 0.2863,
    0, 0, 0, 0.2118,
    0, 0, 0, 0.1490,
    0, 0, 0, 0.0941,
    0, 0, 0, 0.0510,
    0, 0, 0, 0.0157
  };

  glBindTexture(GL_TEXTURE_2D, shadowTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10, 1, 0, GL_RGBA, GL_FLOAT, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}


void StrobeDisplay_setup(Engine *engine) {
  glDisable(GL_DITHER);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  sceneShader = createShaderProgram(sceneVertexSource, sceneFragmentSource);
  textureShader = createShaderProgram(texVertexSource, texFragmentSource);
  initShadow();
  for (int i = 0; i < engine->strobeCount; ++i) {
    initBandBuffers(engine->strobeBuffers[i], &bands[i]);
  }
}


void StrobeDisplay_cleanup(Engine *engine) {
  for (int i = 0; i < engine->strobeCount; ++i) {
    deleteBandBuffers(&bands[i]);
  }
  deleteShadow();
  glDeleteProgram(sceneShader);
  glDeleteProgram(textureShader);
}


static inline void refreshStrobePositions(Engine *engine, int w, int h) {

  // 2 vertices per sample
  //
  //      2  4
  //      +-----------------+
  //      |\ |\ |\ |\ |\ |\ |
  //      | \| \| \| \| \| \|
  //      +-----------------+
  //      1  3
  //

  float padding = 2.0f * 2.0f / (float)h; // circa 2px
  float height = ((2.0f + padding) / engine->strobeCount) - padding;
  float y = -1.0f;

  for (int sid = 0; sid < engine->strobeCount; ++sid) {

    // generate vertex positions
    float x = -1.0f;
    float dx = 2.0f / (engine->strobeBuffers[sid]->count - 1);

    for (int i = 0; i < 2 * bands[sid].count; ) {
      bands[sid].positions[i++] = x;
      bands[sid].positions[i++] = y + height;
      bands[sid].positions[i++] = x;
      bands[sid].positions[i++] = y;
      x += dx;
    }

    y += height + padding;
  }

}


static inline void refreshStrobeColors(Strobe* strobe, StrobeBand* band, ColorScheme scheme, float gain) {

  float dr = scheme.a[0] - scheme.b[0];
  float dg = scheme.a[1] - scheme.b[1];
  float db = scheme.a[2] - scheme.b[2];
  float *buffer = (float*) band->dataBuffer->data;
  int n = band->dataBuffer->count;

  if (Strobe_read(strobe, buffer, n)) {
    int i = 0;
    const GLubyte a = 255;
    do {
      --n;

      float value = buffer[n] * gain * 0.5 + 0.5;
      if (value < 0) {
        value = 0.0;
      }
      else if (value > 1) {
        value = 1.0;
      }

      GLubyte r = (GLubyte)(scheme.b[0] + dr * value);
      GLubyte g = (GLubyte)(scheme.b[1] + dg * value);
      GLubyte b = (GLubyte)(scheme.b[2] + db * value);

      band->colors[i++] = r;
      band->colors[i++] = g;
      band->colors[i++] = b;
      band->colors[i++] = a;

      band->colors[i++] = r;
      band->colors[i++] = g;
      band->colors[i++] = b;
      band->colors[i++] = a;

    } while (n > 0);
  }

}


void StrobeDisplay_initScene(Engine *engine, int w, int h) {
  glViewport(0, 0, w, h);
  refreshShadow(w, h);
  refreshStrobePositions(engine, w, h);
  glClearColor(0, 0, 0, 1);   // TODO - move to configuration
  glClear(GL_COLOR_BUFFER_BIT);
}


void StrobeDisplay_drawScene(Engine *engine) {
  float gain = Engine_gain(engine);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(sceneShader);
  glClearColor(0, 0, 0, 1);
  ColorScheme scheme = engine->config->schemes[engine->config->schemeIndex];
  for (int i = 0; i < engine->strobeCount; ++i) {
    refreshStrobeColors(engine->strobes[i], &bands[i], scheme, gain);
    glBindVertexArray(bands[i].vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, bands[i].count);
  }
  glBindVertexArray(shadowVao);
  glBindTexture(GL_TEXTURE_2D, shadowTexture);
  glUseProgram(textureShader);
  glDrawElements(GL_TRIANGLES, sizeof(shadowElements) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}
