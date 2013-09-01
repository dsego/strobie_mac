/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "StrobeView.h"

@implementation MainController


-(id)init:(Engine*) engine {

  NSSize minSize = { 400, 480 };

  // manually allocate and initialize the window
  NSRect windowRect = NSMakeRect(0, 0, minSize.width, minSize.height);

  NSUInteger styleMask = (
    NSTitledWindowMask |
    NSClosableWindowMask |
    NSMiniaturizableWindowMask |
    NSResizableWindowMask
  );

  NSWindow* window = [
    [NSWindow alloc] initWithContentRect:windowRect
    styleMask:styleMask
    backing:NSBackingStoreBuffered
    defer:NO
  ];

  [window setMinSize: minSize];
  [window setTitle:@"Strobie"];
  [window center];
  self = [super initWithWindow: window];
  [self initStrobeView: engine];

  return self;

}


-(void)drawStrobe {

  [_strobeView setNeedsDisplay: YES];

}


-(void)showWindow {

  [self.window makeKeyAndOrderFront:self];

}


-(void)initStrobeView:(Engine*) engine {


  // add strobe view to window
  _strobeView = [
    [StrobeView alloc] initWithFrame: NSMakeRect(0, 100, 500, 400)
    pixelFormat: [NSOpenGLView defaultPixelFormat]
    engine: engine
  ];

  [self.window.contentView addSubview:self.strobeView];
  [_strobeView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

}


@end