/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>
#import "MainLayerDelegate.h"
#import "StrobeView.h"


@interface MainController : NSWindowController

  @property IBOutlet StrobeView *strobeView;

@end