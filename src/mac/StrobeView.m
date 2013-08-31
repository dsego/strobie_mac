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
    super initWithFrame: NSMakeRect(0, 100, 500, 400)
    pixelFormat: pixelFormat
  ];
  self.engine = engine;
  self->strobeCount = _engine->strobeCount;
  return self;

}


- (void)prepareOpenGL {

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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
    float height = 0.95 * 2.0 / strobeCount; // leave some padding in between bands

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
    glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    strobes[s].colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    int c = 0;
    while (c < 4 * strobes[s].count) {

      // RGBA
      strobes[s].colors[c++] = 255;
      strobes[s].colors[c++] = 0;
      strobes[s].colors[c++] = 0;
      strobes[s].colors[c++] = 255;

      // RGBA
      strobes[s].colors[c++] = 255;
      strobes[s].colors[c++] = 0;
      strobes[s].colors[c++] = 0;
      strobes[s].colors[c++] = 255;

    }

  }

}


- (void)reshape {


}


- (void)drawRect:(NSRect)rect {

  Engine_readStrobes(_engine);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  float gain = _engine->config->strobeGain;;

  for (int s = 0; s < strobeCount; ++s) {

    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    int i = 0;
    int c = 3;
    while (c < 4 * strobes[s].count) {

      float alpha = gain * _engine->strobeBuffers[s].elements[i];
      if (alpha < 0.0) { alpha = 0.0; }
      i += 1;
      strobes[s].colors[c] = (GLubyte) (alpha * 255);
      c += 4;
      strobes[s].colors[c] = (GLubyte) (alpha * 255);
      c += 4;

    }

    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].colorBuffer);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, strobes[s].count);

  }

  [[self openGLContext] flushBuffer];

}

@end