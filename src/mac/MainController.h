/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>
#import "StrobeAnimation.h"

@interface MainController : NSWindowController

@property (strong) NSTextField* pitchLabel;
@property (strong) NSView* strobeView;
@property (strong) StrobeAnimation* strobeLayer;

-(void)showWindow;
-(void)refreshPitch:(double)pitch peak:(double)peak;

@end