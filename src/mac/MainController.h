/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>
#import "StrobeLayerDelegate.h";

@interface MainController : NSWindowController

  @property (strong) IBOutlet NSView *strobeView;
  @property (strong) CALayer *strobeLayer;
  @property (strong) StrobeLayerDelegate *strobeLayerDelegate;

@end