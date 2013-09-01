/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>
#import "../engine/Engine.h"

@interface MainController : NSWindowController

  @property (strong) NSOpenGLView* strobeView;

  -(id)init: (Engine*)engine;
  -(void)showWindow;
  -(void)drawStrobe;

@end