/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */


#include <math.h>
#include <string.h>
#include <OpenGL/gl3.h>
#include "utils.h"
#include "StrobeDisplay.h"
// #include "glDebug.h"

#define MAX_STROBE_COUNT 10



static const char* sceneVertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "in vec4 color;"
  "out vec4 Color;"
  "void main() {"
  "   Color = color;"
  "   gl_Position = vec4(position, 0.0, 1.0);"
  "}";

static const char* sceneFragmentSource =
  "#version 150 core\n"
  "out vec4 outColor;"
  "in vec4 Color;"
  "void main() {"
  "   outColor = Color;"
  "}";

static const char* texVertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "in vec2 texcoord;"
  "out vec2 Texcoord;"
  "void main() {"
  "  Texcoord = texcoord;"
  "  gl_Position = vec4(position, 0.0, 1.0);"
  "}";

static const char* texFragmentSource =
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
  float peak;
  int highlighted;
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





static inline GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {

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

    int count = strobes[sid].count = engine->strobeBuffers[sid]->count * 2;

    // Static position data
    glBindBuffer(GL_ARRAY_BUFFER, strobes[sid].posBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * count * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    strobes[sid].positions = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    GLint posAttrib = glGetAttribLocation(sceneShader, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    // Dynamic color data
    glBindBuffer(GL_ARRAY_BUFFER, strobes[sid].colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * count * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    strobes[sid].colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    GLint colAttrib = glGetAttribLocation(sceneShader, "color");
    glVertexAttribPointer(colAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(colAttrib);

  }

}


static inline void refreshShadowPositions(int screenWidth, int screenHeight) {

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

  genObjects();
  initStrobeBuffers(engine);

}


void StrobeDisplay_cleanup() {

  deleteObjects();
  glDeleteProgram(sceneShader);
  glDeleteProgram(textureShader);

}

// #define PI  3.14159265358979323846264338327950288419716939937510582097494459230


// static inline void refreshStrobePositions(Engine *engine, int w, int h) {

//   float baseRadius = 0.1;
//   float padding = 2.0f * 2.0f / (float)h; // circa 2px
//   float height = 0.22f;
//   float r = baseRadius;

//   float ratio = (float)h / (float)w;

//   for (int sid = 0; sid < strobeCount; ++sid) {

//     float t = 0.0;
//     float length = PI;
//     float dt = length / (engine->strobeLengths[sid] - 1);

//     for (int i = 0; i < 2 * strobes[sid]->count; ) {
//       strobes[sid].positions[i++] = (r + height) * cos(t) * ratio;
//       strobes[sid].positions[i++] = (r + height) * sin(t) - 1;
//       strobes[sid].positions[i++] = r * cos(t) * ratio;
//       strobes[sid].positions[i++] = r * sin(t) - 1;
//       t += dt;
//     }

//     r += height + padding;
//   }

// }



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
    float dx = 2.0f / (engine->strobeBuffers[sid]->count - 1);

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


static inline void refreshStrobeColors(Engine *engine, int sid, float gain) {

  ColorScheme scheme = engine->config->schemes[engine->config->schemeIndex];

  GLubyte color[3];
  float scale[3];

  scale[0] = scheme.a[0] - scheme.b[0];
  scale[1] = scheme.a[1] - scheme.b[1];
  scale[2] = scheme.a[2] - scheme.b[2];


  // scale[0] = scheme.a[0] - scheme.b[0];
  // scale[1] = scheme.a[1] - scheme.b[1];
  // scale[2] = scheme.a[2] - scheme.b[2];

  if (Engine_readStrobe(engine, sid)) {

    float *buffer = (float*)engine->strobeBuffers[sid]->elements;
    int i = engine->strobeBuffers[sid]->count;
    int cid = 0;

    const GLubyte opacity = 255;

    // if (engine->config->highlightBands) {

      // float peak = gain * findWavePeak(buffer, i);
    //   float upper = 3;
    //   float lower = 0.2;

    //   if (peak > upper) {
    //     strobes[sid].highlighted = 1;
    //   }
    //   else if (strobes[sid].highlighted && peak < lower) {
    //     strobes[sid].highlighted = 0;
    //   }

    //   opacity = strobes[sid].highlighted ? 255 : 150;

    // }

    do {

      --i;

      float value = buffer[i] * gain * 0.5 + 0.5;

      if (value < 0) {
        value = 0.0;
      }
      else if (value > 1) {
        value = 1.0;
      }

      color[0] = (GLubyte) (scheme.b[0] + scale[0] * value);
      color[1] = (GLubyte) (scheme.b[1] + scale[1] * value);
      color[2] = (GLubyte) (scheme.b[2] + scale[2] * value);

      // float hsv[3], rgb[3];
      // hsv[0] = (scheme.b[0] + scale[0] * value);
      // hsv[1] = (scheme.b[1] + scale[1] * value);
      // hsv[2] = (scheme.b[2] + scale[2] * value);

      // hsv2rgb(hsv, rgb);

      // color[0] = roundf(rgb[0] * 255);
      // color[1] = roundf(rgb[1] * 255);
      // color[2] = roundf(rgb[2] * 255);


      // color (RGB)
      strobes[sid].colors[cid++] = color[0];
      strobes[sid].colors[cid++] = color[1];
      strobes[sid].colors[cid++] = color[2];
      strobes[sid].colors[cid++] = opacity;

      // color (RGB)
      strobes[sid].colors[cid++] = color[0];
      strobes[sid].colors[cid++] = color[1];
      strobes[sid].colors[cid++] = color[2];
      strobes[sid].colors[cid++] = opacity;

    } while (i > 0);

  }

}


void StrobeDisplay_initScene(Engine *engine, int w, int h) {

  glViewport(0, 0, w, h);
  refreshShadowPositions(w, h);
  refreshStrobePositions(engine, w, h);
  glClearColor(0, 0, 0, 1);   // TODO - move to configuration
  glClear(GL_COLOR_BUFFER_BIT);

}


void StrobeDisplay_drawScene(Engine *engine) {

  float gain = Engine_gain(engine);

  glClear(GL_COLOR_BUFFER_BIT);

  // draw strobes
  glUseProgram(sceneShader);
  glClearColor(0, 0, 0, 1);

  for (int i = 0; i < strobeCount; ++i) {
    refreshStrobeColors(engine, i, gain);
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
