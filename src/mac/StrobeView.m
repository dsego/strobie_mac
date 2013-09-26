/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <OpenGL/gl3.h>
#import "StrobeView.h"
#import "shared.h"


#define MAX_STROBE_COUNT 10


typedef struct {

  GLuint vertexBuffer;
  GLuint colorBuffer;
  GLfloat *vertices;
  GLubyte *colors;
  int count;

} StrobeDisplay;


@implementation StrobeView {

  StrobeDisplay strobes[MAX_STROBE_COUNT];
  NSTimer *timer;

}


- (void) awakeFromNib {

  // a timer works on the main thread
  timer = [NSTimer scheduledTimerWithTimeInterval:0.02 target:self selector:@selector(redraw) userInfo:nil repeats:YES];
  // [timer setTolerance: 0.01];

}


- (void)dealloc {

  [timer invalidate];

}


- (void)prepareOpenGL {

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];

  // Disable unnecessary state variables
  glDisable(GL_DITHER);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_FOG);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glPixelZoom(1.0,1.0);

  // Synchronize buffer swaps with vertical refresh rate
  GLint swapInt = 1;
  [context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

}


-(void) drawRect: (NSRect) bounds {

  [self drawFrame];

}


- (void)setupBuffers {

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];

  int strobeCount = engine->strobeCount;

  // allocate buffers
  for (int s = 0; s < strobeCount; ++s) {

    // 2 vertices per sample
    //   _________________
    //  |\ |\ |\ |\ |\ |\ |
    //  | \| \| \| \| \| \|
    //   -----------------
    //
    int count = strobes[s].count = engine->strobeBuffers[s].length * 2;

    // Static position data
    // glDeleteBuffers(1, &(strobes[s].vertexBuffer));
    glGenBuffers(1, &(strobes[s].vertexBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].vertexBuffer);

    // allocate GL buffer
    glBufferData(GL_ARRAY_BUFFER, 2 * count * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    // get pointer to buffer
    strobes[s].vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    // generate vertices
    float x = -1.0;
    float dx = 2.0 / (engine->strobeBuffers[s].length - 1);
    float y = -1.0 + 2.0 * s / (float)(strobeCount - 0.01);
    float height = 2.0 / (float)strobeCount - 0.01;

    int v = 0;
    while (v < 2 * count) {

      strobes[s].vertices[v++] = x;
      strobes[s].vertices[v++] = y;
      strobes[s].vertices[v++] = x;
      strobes[s].vertices[v++] = y + height;
      x += dx;

    }

    // Dynamic color data
    // glDeleteBuffers(1, &(strobes[s].colorBuffer));
    glGenBuffers(1, &(strobes[s].colorBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    strobes[s].colors = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  }

}


- (void)redraw {

  [self setNeedsDisplay:YES];

}


- (void)drawFrame {

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];

  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  if (engine == NULL) { return; }


  Engine_readStrobes(engine);

  for (int s = 0; s < engine->strobeCount; ++s) {

    GLubyte redBase     = engine->config->strobes[s].color2[0];
    GLubyte greenBase   = engine->config->strobes[s].color2[1];
    GLubyte blueBase    = engine->config->strobes[s].color2[2];
    GLubyte redFactor   = engine->config->strobes[s].color1[0] - engine->config->strobes[s].color2[0];
    GLubyte greenFactor = engine->config->strobes[s].color1[1] - engine->config->strobes[s].color2[1];
    GLubyte blueFactor  = engine->config->strobes[s].color1[2] - engine->config->strobes[s].color2[2];

    glBindBuffer(GL_ARRAY_BUFFER, strobes[s].vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    float gain = engine->config->strobes[s].gain;
    int i =  engine->strobeBuffers[s].length;
    int c = 0;

    while (c < 3 * strobes[s].count) {

      i -= 1;
      float value = engine->strobeBuffers[s].elements[i] * gain;

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

  [context flushBuffer];

}



@end