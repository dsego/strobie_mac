/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <math.h>
#include <OpenGL/gl3.h>
#include "utils.h"
#include "StrobeDisplay.h"

#define MAX_STROBE_COUNT 10


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



static const GLchar* vertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "in vec3 color;"
  "out vec3 Color;"
  "void main() {"
  "   Color = color;"
  "   gl_Position = vec4(position, 0.0, 1.0);"
  "}";

static const GLchar* fragmentSource =
  "#version 150 core\n"
  "out vec4 outColor;"
  "in vec3 Color;"
  "void main() {"
  "   outColor = vec4(Color, 1.0);"
  "}";

static GLuint shaderProgram;



static inline GLuint loadShaders() {

  GLint status;

  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("%s\n", infoLog);
  }

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
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


// OpenGL debugging
// static void printError() {

//   switch(glGetError()) {
//     case GL_INVALID_ENUM: printf("GL_INVALID_ENUM\n"); break;
//     case GL_INVALID_VALUE: printf("GL_INVALID_VALUE\n"); break;
//     case GL_INVALID_OPERATION: printf("GL_INVALID_OPERATION\n"); break;
//     case GL_INVALID_FRAMEBUFFER_OPERATION: printf("GL_INVALID_FRAMEBUFFER_OPERATION\n"); break;
//     case GL_OUT_OF_MEMORY: printf("GL_OUT_OF_MEMORY\n"); break;
//     case GL_NO_ERROR: printf("GL_NO_ERROR\n"); break;
//     default: printf("default\n"); break;
//   }

// }


// OpenGL debugging
// static void printProgramInfo() {

//   GLint status;
//   glValidateProgram(shaderProgram);
//   glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &status);
//   if (status != GL_TRUE) {
//     char infoLog[512];
//     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//     printf("%s\n", infoLog);
//   }

// }




static inline void initStrobeBuffers(Engine *engine) {

  strobeCount = engine->strobeCount;

  for (int sid = 0; sid < strobeCount; ++sid) {

    glGenVertexArrays(1, &strobes[sid].vao);
    glBindVertexArray(strobes[sid].vao);

    int count = strobes[sid].count = engine->strobeLengths[sid] * 2;

    // Static position data
    glDeleteBuffers(1, &(strobes[sid].posBuffer));
    glGenBuffers(1, &(strobes[sid].posBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[sid].posBuffer);

    glBufferData(GL_ARRAY_BUFFER, 2 * count * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    strobes[sid].positions = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    // Dynamic color data
    glDeleteBuffers(1, &(strobes[sid].colorBuffer));
    glGenBuffers(1, &(strobes[sid].colorBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[sid].colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    strobes[sid].colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glVertexAttribPointer(colAttrib, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(colAttrib);

    // unbind
    // glBindVertexArray(NULL);
    // glBindBuffer(GL_ARRAY_BUFFER, NULL);

  }

}


void StrobeDisplay_setup(Engine *engine){

  glDisable(GL_DITHER);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);

  shaderProgram = loadShaders();
  glUseProgram(shaderProgram);
  initStrobeBuffers(engine);

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


static inline void refreshStrobeColors(Engine *engine) {

  int fresh = Engine_readStrobes(engine);

  for (int sid = 0; sid < engine->strobeCount; ++sid) {

    if (fresh) {

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

        i -= 1;

        float value = engine->strobeBuffers[sid].elements[i];

        // value = 1.0 / (1.0 + exp(-gain * value));
        value = (value * gain + 1) * 0.5;

        if (value < 0.0) {
          value = 0.0;
        }
        else if (value > 1.0) {
          value = 1.0;
        }

        // GLubyte color[3];
        GLfloat color[3];
        // hsv2rgb(hsv, rgb);

        color[0] = (GLubyte) (start0 + scale0 * value);
        color[1] = (GLubyte) (start1 + scale1 * value);
        color[2] = (GLubyte) (start2 + scale2 * value);

        // color (RGB)
        strobes[sid].colors[cid++] = color[0];
        strobes[sid].colors[cid++] = color[1];
        strobes[sid].colors[cid++] = color[2];
        // strobes[sid].colors[cid++] = 255;

        // color (RGB)
        strobes[sid].colors[cid++] = color[0];
        strobes[sid].colors[cid++] = color[1];
        strobes[sid].colors[cid++] = color[2];
        // strobes[sid].colors[cid++] = 255;

      }

    }

  }

}


void StrobeDisplay_initScene(Engine *engine, int w, int h) {

  refreshStrobePositions(engine, w, h);

}


void StrobeDisplay_drawScene(Engine *engine) {

  // Clear the screen to black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  refreshStrobeColors(engine);

  // draw strobes
  for (int i = 0; i < strobeCount; ++i) {

    glBindVertexArray(strobes[i].vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, strobes[i].count);

  }

}
