/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "shared.h"


@implementation MainController

- (void)awakeFromNib {

  [self setupStrobeLayer];

}


-(void)setupStrobeLayer {

  _strobeLayerDelegate = [[StrobeLayerDelegate alloc] init];

  _strobeLayer = [CALayer layer];
  _strobeLayer.delegate = _strobeLayerDelegate;

  // layer background
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  _strobeLayer.opaque = YES;
  _strobeLayer.backgroundColor = CGColorCreate(colorSpace, (CGFloat[]) { 0, 0, 0 });

  // display layer
  [_strobeLayer setNeedsDisplay];

  // set view to host the layer
  _strobeView.layer = _strobeLayer;

}


@end