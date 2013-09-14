/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@interface StrobeLayerDelegate : NSObject

-(void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx;

@end