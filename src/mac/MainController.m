/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"

@implementation MainController


-(id)init {

  // manually allocate and initialize the window
  NSRect windowRect = NSMakeRect(0, 0, 500, 500);

  NSUInteger styleMask = (
    NSTitledWindowMask |
    NSClosableWindowMask |
    NSMiniaturizableWindowMask |
    NSResizableWindowMask
  );

  NSWindow* window = [[NSWindow alloc] initWithContentRect:windowRect
                                       styleMask:styleMask
                                       backing:NSBackingStoreBuffered
                                       defer:NO];
  NSSize minSize = { 400, 400 };

  [window setMinSize: minSize];
  [window setTitle:@"Strobie"];
  [window center];

  self = [super initWithWindow: window];

  [self initStrobeView];

    _pitchLabel = [[NSTextField alloc] initWithFrame: NSMakeRect(30, 30, 300, 30)];
  [self.window.contentView addSubview:_pitchLabel];


  return self;

}


-(void)drawStrobe:(Engine*) engine {

  // [strobeView drawStrobeBuffers:engine->strobeBuffers
  //             bufferLengths:engine->strobeBufferLengths
  //             numBuffers:engine->strobeCount];

}


-(void)refreshPitch:(double)pitch peak:(double)peak {

  [_pitchLabel setStringValue:[NSString stringWithFormat:@"%.10lf %.1lf", pitch, pitch]];

}


-(void)showWindow {

  [self.window makeKeyAndOrderFront:self];

}


-(void)initStrobeView {

  // add strobe view to window
  self.strobeView = [[NSView alloc] initWithFrame:NSMakeRect(0, 100, 500, 400)];
  [self.window.contentView addSubview:self.strobeView];
  [self.strobeView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

  // create and attach animation layer to host view
  self.strobeLayer = [StrobeAnimation layer];

  self.strobeLayer.asynchronous = YES;
  self.strobeLayer.needsDisplayOnBoundsChange = YES;
  self.strobeLayer.backgroundColor = CGColorCreateGenericRGB(0, 0, 0, 1);

  self.strobeView.layer = self.strobeLayer;
  [self.strobeView.layer setNeedsDisplay];
  [self.strobeView setWantsLayer:YES];

}


@end