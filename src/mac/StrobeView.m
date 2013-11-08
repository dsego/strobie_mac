/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import "StrobeView.h"
#import "../engine/StrobeDisplay.h"
#import "shared.h"



@implementation StrobeView


- (BOOL)mouseDownCanMoveWindow {

  return YES;

}


- (void)prepareOpenGL {

  NSOpenGLContext* context = [self openGLContext];
  // [context makeCurrentContext];

  // Enable the multithreading
  // CGLEnable([context CGLContextObj], kCGLCEMPEngine);

  StrobeDisplay_setup();

  // Synchronize buffer swaps with vertical refresh rate
  GLint swapInt = 1;
  [context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

}


-(void)drawRect: (NSRect)bounds {

  NSOpenGLContext* context = [self openGLContext];
  StrobeDisplay_drawScene(engine);

  // An implicit glFlush is done by flushBuffer before it returns.
  [context flushBuffer];

}


-(void)reshape {

  // recalculate vertices
  StrobeDisplay_initScene(engine, _bounds.size.width, _bounds.size.height);

}


- (void)setupBuffers {

  // NSOpenGLContext* context = [self openGLContext];
  // [context makeCurrentContext];
  StrobeDisplay_initScene(engine, _bounds.size.width, _bounds.size.height);

}


@end
