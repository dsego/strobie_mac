/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <math.h>
#include <OpenGL/gl3.h>
#include "utils.h"
#include "StrobeDisplay.h"
#include "glDebug.h"

#define MAX_STROBE_COUNT 10



static char* sceneVertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "in vec3 color;"
  "out vec3 Color;"
  "void main() {"
  "   Color = color;"
  "   gl_Position = vec4(position, 0.0, 1.0);"
  "}";

static char* sceneFragmentSource =
  "#version 150 core\n"
  "out vec4 outColor;"
  "in vec3 Color;"
  "void main() {"
  "   outColor = vec4(Color, 1.0);"
  "}";

static char* texVertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "in vec2 texcoord;"
  "out vec2 Texcoord;"
  "void main() {"
  "  Texcoord = texcoord;"
  "  gl_Position = vec4(position, 0.0, 1.0);"
  "}";

static char* texFragmentSource =
  "#version 150 core\n"
  "in vec2 Texcoord;"
  "out vec4 outColor;"
  "uniform sampler2D tex;"
  "void main() {"
  "   outColor = texture(tex, Texcoord);"
  "}";





typedef struct {

  GLuint vao;
  GLuint posBuffer;
  GLuint colorBuffer;
  GLfloat *positions;
  GLubyte *colors;
  int count;

} StrobeDisplay;


static StrobeDisplay strobes[MAX_STROBE_COUNT];
static int strobeCount = 0;

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





static inline GLuint createShaderProgram(char* vertexSource, char* fragmentSource) {

  GLint status;

  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (const GLchar *const *) &vertexSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("%s\n", infoLog);
  }

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1,(const GLchar *const *) &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("%s\n", infoLog);
  }

  // Link the vertex and fragment shader into a shader program
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glBindFragDataLocation(program, 0, "outColor");
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    char infoLog[512];
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    printf("%s\n", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;

}


static inline void genObjects() {

  for (int i = 0; i < MAX_STROBE_COUNT; ++i) {
    glGenVertexArrays(1, &strobes[i].vao);
    glGenBuffers(1, &(strobes[i].posBuffer));
    glGenBuffers(1, &(strobes[i].colorBuffer));
  }

  glGenVertexArrays(1, &shadowVao);
  glGenBuffers(1, &shadowVbo);
  glGenBuffers(1, &shadowEbo);
  glGenTextures(1, &shadowTexture);

}


static inline void deleteObjects() {

  for (int i = 0; i < MAX_STROBE_COUNT; ++i) {
    glDeleteVertexArrays(1, &strobes[i].vao);
    glDeleteBuffers(1, &(strobes[i].posBuffer));
    glDeleteBuffers(1, &(strobes[i].colorBuffer));
  }

  glDeleteVertexArrays(1, &shadowVao);
  glDeleteBuffers(1, &shadowVbo);
  glDeleteBuffers(1, &shadowEbo);
  glDeleteTextures(1, &shadowTexture);

}


static inline void initStrobeBuffers(Engine *engine) {

  strobeCount = engine->strobeCount;

  for (int sid = 0; sid < strobeCount; ++sid) {

    glBindVertexArray(strobes[sid].vao);

    int count = strobes[sid].count = engine->strobeLengths[sid] * 2;

    // Static position data
    glBindBuffer(GL_ARRAY_BUFFER, strobes[sid].posBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * count * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    strobes[sid].positions = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    GLint posAttrib = glGetAttribLocation(sceneShader, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    // Dynamic color data
    glBindBuffer(GL_ARRAY_BUFFER, strobes[sid].colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * count * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    strobes[sid].colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    GLint colAttrib = glGetAttribLocation(sceneShader, "color");
    glVertexAttribPointer(colAttrib, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(colAttrib);

  }

}


static inline void refreshShadowPositions(int screenWidth, int screenHeight) {

  // 6 px shadow
  float x = 1.f - 6.f * 2.f / (float)screenWidth;
  float y = 1.f - 6.f * 2.f / (float)screenHeight;

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
    0.f, 0.f, 0.f, 0.455f,
    0.f, 0.f, 0.f, 0.326f,
    0.f, 0.f, 0.f, 0.212f,
    0.f, 0.f, 0.f, 0.126f,
    0.f, 0.f, 0.f, 0.063f,
    0.f, 0.f, 0.f, 0.024f
  };

  glBindTexture(GL_TEXTURE_2D, shadowTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 6, 1, 0, GL_RGBA, GL_FLOAT, pixels);
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

  genObjects();
  initStrobeBuffers(engine);

}


void StrobeDisplay_cleanup() {

  deleteObjects();
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
  float height = ((2.0f + padding) / strobeCount) - padding;
  float y = -1.0f;

  for (int sid = 0; sid < strobeCount; ++sid) {

    // generate vertex positions
    float x = -1.0f;
    float dx = 2.0f / (engine->strobeLengths[sid] - 1);

    for (int i = 0; i < 2 * strobes[sid].count; ) {
      strobes[sid].positions[i++] = x;
      strobes[sid].positions[i++] = y + height;
      strobes[sid].positions[i++] = x;
      strobes[sid].positions[i++] = y;
      x += dx;
    }

    y += height + padding;
  }

}


static inline void refreshStrobeColors(Engine *engine, int sid) {

  if (Engine_readStrobe(engine, sid)) {

    int start0 = engine->config->strobes[sid].color1[0];
    int start1 = engine->config->strobes[sid].color1[1];
    int start2 = engine->config->strobes[sid].color1[2];

    int end0 = engine->config->strobes[sid].color2[0];
    int end1 = engine->config->strobes[sid].color2[1];
    int end2 = engine->config->strobes[sid].color2[2];

    int scale0 = end0 - start0;
    int scale1 = end1 - start1;
    int scale2 = end2 - start2;


    // TODO : add hysteresis !
    float gain = 1000.0;

    int i = engine->strobeLengths[sid];

    for (int cid = 0; cid < 3 * strobes[sid].count; ) {

      --i;

      float value = engine->strobeBuffers[sid].elements[i];

      // value = 1.0 / (1.0 + exp(-gain * value));
      value = (value * gain + 1) * 0.5;

      if (value < 0) {
        value = 0.0;
      }
      else if (value > 1) {
        value = 1.0;
      }

      GLubyte color[3];
      // GLfloat color[3];
      // hsv2rgb(hsv, rgb);

      color[0] = (GLubyte) (start0 + scale0 * value);
      color[1] = (GLubyte) (start1 + scale1 * value);
      color[2] = (GLubyte) (start2 + scale2 * value);

      // color (RGB)
      strobes[sid].colors[cid++] = color[0];
      strobes[sid].colors[cid++] = color[1];
      strobes[sid].colors[cid++] = color[2];

      // color (RGB)
      strobes[sid].colors[cid++] = color[0];
      strobes[sid].colors[cid++] = color[1];
      strobes[sid].colors[cid++] = color[2];

    }
  }

}


void StrobeDisplay_initScene(Engine *engine, int w, int h) {

  glViewport(0, 0, w, h);
  refreshShadowPositions(w, h);
  refreshStrobePositions(engine, w, h);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

}


void StrobeDisplay_drawScene(Engine *engine) {

  glClear(GL_COLOR_BUFFER_BIT);

  // draw strobes
  glUseProgram(sceneShader);

  for (int i = 0; i < strobeCount; ++i) {
    refreshStrobeColors(engine, i);
    glBindVertexArray(strobes[i].vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, strobes[i].count);
  }

  // draw inner shadow
  glBindVertexArray(shadowVao);
  // glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, shadowTexture);
  glUseProgram(textureShader);
  glDrawElements(GL_TRIANGLES, sizeof(shadowElements) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

}
