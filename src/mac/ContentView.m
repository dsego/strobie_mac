/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */


#import "ContentView.h"

@implementation ContentView {

  NSColor *bgColor1;
  NSColor *bgColor2;
  NSGradient *gradient;

}


- (void)awakeFromNib {

  bgColor1 = [NSColor colorWithCalibratedRed:27.0/255.0 green:27.0/255.0 blue:27.0/255.0 alpha:1.0];
  bgColor2 = [NSColor colorWithCalibratedRed:21.0/255.0 green:21.0/255.0 blue:21.0/255.0 alpha:1.0];
  gradient = [NSGradient alloc];

}


- (void)drawRect:(NSRect)dirtyRect {

  NSRect frame = [self frame];
  gradient = [gradient initWithColorsAndLocations: bgColor2, 0.0, bgColor1, 100.0 / frame.size.height, nil];
  [gradient drawInRect:frame angle:90];

}

@end
