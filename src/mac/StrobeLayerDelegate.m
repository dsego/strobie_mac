/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#import "StrobeLayerDelegate.h"



@implementation StrobeLayerDelegate


-(void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx {

    NSGraphicsContext *nsGraphicsContext;
   nsGraphicsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:ctx
                                                                  flipped:NO];
   [NSGraphicsContext saveGraphicsState];
   [NSGraphicsContext setCurrentContext:nsGraphicsContext];

   // ...Draw content using NS APIs...
   NSRect aRect=NSMakeRect(10.0,10.0,30.0,30.0);
   NSBezierPath *thePath=[NSBezierPath bezierPathWithRect:aRect];
   [[NSColor redColor] set];
   [thePath fill];

   [NSGraphicsContext restoreGraphicsState];

}


@end