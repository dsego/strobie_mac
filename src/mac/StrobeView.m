/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <OpenGL/OpenGL.h>
#import "StrobeView.h"
#import "../engine/Engine.h"


typedef struct {

  GLuint vertexBuffer;
  GLuint colorBuffer;
  GLfloat *vertices;
  GLubyte *colors;
  int count;

} StrobeDisplay;


@implementation StrobeView {

  StrobeDisplay strobes[10];
  int strobeCount;

}


- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format engine:(Engine *)engine {

  NSOpenGLPixelFormatAttribute attributes[] = {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    0
  };

  NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];

  self = [
    super initWithFrame: NSMakeRect(0, 100, 400, 380)
    pixelFormat: pixelFormat
  ];
  self.engine = engine;
  strobeCount = _engine->strobeCount;
  return self;

}


- (void)prepareOpenGL {

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // allocate buffers

  for (int s = 0; s < strobeCount; ++s) {

    // 2 vertices per sample
    //   _________________
    //  |\ |\ |\ |\ |\ |\ |
    //  | \| \| \| \| \| \|
    //   -----------------
    //
    int count = strobes[s].count = _engine->strobeBuffers[s].length * 2;

    // Static position data
    glGenBuffers(1, &(strobes[s].vertexBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].vertexBuffer);

    // allocate GL buffer
    glBufferData(GL_ARRAY_BUFFER, 2 * count * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    // get pointer to buffer
    strobes[s].vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    // generate vertices
    float x = -1.0;
    float dx = 2.0 / (_engine->strobeBuffers[s].length - 1);
    float y = -1.0 + 2.0 * s / (float)strobeCount;
    float height = 2.0 / strobeCount - 0.01;

    int v = 0;
    while (v < 2 * count) {

      strobes[s].vertices[v++] = x;
      strobes[s].vertices[v++] = y;
      strobes[s].vertices[v++] = x;
      strobes[s].vertices[v++] = y + height;
      x += dx;

    }

    // Dynamic color data
    glGenBuffers(1, &(strobes[s].colorBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    strobes[s].colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  }

}


- (void)reshape {

  glViewport(0, 0, _bounds.size.width, _bounds.size.height);

}


- (void)drawRect:(NSRect)rect {


  GLubyte redBase     = _engine->config->strobeColor2[0];
  GLubyte greenBase   = _engine->config->strobeColor2[1];
  GLubyte blueBase    = _engine->config->strobeColor2[2];
  GLubyte redFactor   = _engine->config->strobeColor1[0] - _engine->config->strobeColor2[0];
  GLubyte greenFactor = _engine->config->strobeColor1[1] - _engine->config->strobeColor2[1];
  GLubyte blueFactor  = _engine->config->strobeColor1[2] - _engine->config->strobeColor2[2];

  Engine_readStrobes(_engine);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int s = 0; s < strobeCount; ++s) {

    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    int i =  _engine->strobeBuffers[s].length;
    int c = 0;
    while (c < 3 * strobes[s].count) {

      i -= 1;
      float value = _engine->strobeBuffers[s].elements[i];

      if (value < 0.0) {
        value = 0.0;
      }
      else if (value > 1.0) {
        value = 1.0;
      }

      GLubyte r = redBase + value * redFactor;
      GLubyte g = greenBase + value * greenFactor;
      GLubyte b = blueBase + value * blueFactor;

      // RGB
      strobes[s].colors[c++] = r;
      strobes[s].colors[c++] = g;
      strobes[s].colors[c++] = b;

      // RGB
      strobes[s].colors[c++] = r;
      strobes[s].colors[c++] = g;
      strobes[s].colors[c++] = b;

    }

    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].colorBuffer);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, strobes[s].count);

  }

  [[self openGLContext] flushBuffer];

}

@end