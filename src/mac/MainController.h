//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#import <AppKit/NSWindowController.h>
#import "StrobeView.h"

@interface MainController : NSWindowController

@property (strong) StrobeView* strobeView;

-(void)showWindow;
// -(void)

@end