//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#import <AppKit/NSGraphicsContext.h>
#import "StrobeView.h"
#import "../engine/Engine.h"

@implementation StrobeView

- (void)drawRect:(NSRect)dirtyRect
{
  CGContextRef ctx = [[NSGraphicsContext currentContext] graphicsPort];
  CGContextSetRGBFillColor(ctx, 0, 0, 0, 1);
  CGContextFillRect(ctx, self.bounds);

  return;
  CGGradientRef myGradient;
  CGColorSpaceRef myColorspace;
  CGFloat components[16] = {
    1.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 1.0
  };
  CGPoint myStartPoint, myEndPoint;
  myStartPoint.x = 0.0;
  myStartPoint.y = 0.0;
  myEndPoint.x = self.bounds.size.width;
  myEndPoint.y = 0.0;
  myColorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
  myGradient = CGGradientCreateWithColorComponents(myColorspace, components, NULL, 4);
  CGContextDrawLinearGradient(ctx, myGradient, myStartPoint, myEndPoint, 0);
}

// flip coordinates
- (BOOL) isFlipped
{
  return YES;
}

-(void)reshape
{

}

@end