/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import <Cocoa/Cocoa.h>
#import "../engine/Engine.h"

@interface StrobeView : NSOpenGLView

  @property Engine* engine;

  -(id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format engine:(Engine *)engine;

@end
