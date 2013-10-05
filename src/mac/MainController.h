/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>
#import "StrobeView.h"


@interface MainController : NSWindowController <NSWindowDelegate>

  @property IBOutlet StrobeView *strobeView;

@end