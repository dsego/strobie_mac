//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MainController.h"

@implementation MainController

-(id)init
{
  // manually allocate and initialize the window
  NSRect windowRect = NSMakeRect(0, 0, 500, 500);
  NSUInteger styleMask = (NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask);
  NSWindow* window = [[NSWindow alloc] initWithContentRect:windowRect
                                       styleMask:styleMask
                                       backing:NSBackingStoreBuffered
                                       defer:NO];
  NSSize minSize = { 400, 400 };
  [window setMinSize: minSize];
  [window setTitle:@"Strobie"];
  [window center];

  self = [super initWithWindow: window];

  // add views
  self.strobeView = [[StrobeView alloc] initWithFrame:NSMakeRect(0, 0, 500, 300)];
  [self.window.contentView addSubview:self.strobeView];
  // [self.strobeView setNeedsDisplay:YES];


  return self;
}

-(void)showWindow
{
  [self.window makeKeyAndOrderFront:self];
}

@end