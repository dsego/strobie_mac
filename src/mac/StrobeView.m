/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import "StrobeView.h"
#import "../engine/StrobeDisplay.h"
#import "shared.h"



@implementation StrobeView




- (void)awakeFromNib {

  NSOpenGLPixelFormatAttribute attributes[] = {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,   // Core Profile !
    NULL
  };

  NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
  NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:format shareContext: NULL];
  [self setPixelFormat: format];
  [self setOpenGLContext: context];

}


- (void)prepareOpenGL {

  NSOpenGLContext* context = [self openGLContext];

  // Synchronize buffer swaps with vertical refresh rate
  GLint swapInt = 1;
  [context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

  StrobeDisplay_setup(engine);

}


-(void)drawRect:(NSRect)bounds {

  // NSOpenGLContext* context = [self openGLContext];
  // [context makeCurrentContext];

  StrobeDisplay_drawScene(engine);

  // An implicit glFlush is done by flushBuffer before it returns.
  // [context flushBuffer];

}


-(void)reshape {

  // recalculate vertices
  StrobeDisplay_initScene(engine, _bounds.size.width, _bounds.size.height);

}


- (BOOL)mouseDownCanMoveWindow {

  return YES;

}


-(void)dealloc {

  StrobeDisplay_cleanup();

}



@end
