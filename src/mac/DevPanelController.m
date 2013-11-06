/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "DevPanelController.h"
#import "shared.h"


@implementation DevPanelController


- (void)awakeFromNib {

  NSArray *args = [[NSProcessInfo processInfo] arguments];

  // show the development panel
  if ([args count] > 1 && [[args objectAtIndex: 1] isEqual: @"-dev"]) {
    NSLog(@"development mode");
    [self.window makeKeyAndOrderFront: nil];
  }

}


- (IBAction)colorWellChanged: (id)sender {

  NSColor *colorA = [self.colorWellA color];
  NSColor *colorB = [self.colorWellB color];

  int rgbA[3];
  int rgbB[3];

  rgbA[0] = (int) ([colorA redComponent] * 255);
  rgbA[1] = (int) ([colorA greenComponent] * 255);
  rgbA[2] = (int) ([colorA blueComponent] * 255);

  rgbB[0] = (int) ([colorB redComponent] * 255);
  rgbB[1] = (int) ([colorB greenComponent] * 255);
  rgbB[2] = (int) ([colorB blueComponent] * 255);

  Engine_setColors(engine, rgbA, rgbB);

}


@end
