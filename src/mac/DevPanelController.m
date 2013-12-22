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

  NSColor *color1 = [self.colorWell1 color];
  NSColor *color2 = [self.colorWell2 color];
  NSColor *color3 = [self.colorWell3 color];
  NSColor *color4 = [self.colorWell4 color];

  int rgb[4][3];

  rgb[0][0] = (int) ([color1 redComponent] * 255);
  rgb[0][1] = (int) ([color1 greenComponent] * 255);
  rgb[0][2] = (int) ([color1 blueComponent] * 255);

  rgb[1][0] = (int) ([color2 redComponent] * 255);
  rgb[1][1] = (int) ([color2 greenComponent] * 255);
  rgb[1][2] = (int) ([color2 blueComponent] * 255);

  rgb[2][0] = (int) ([color3 redComponent] * 255);
  rgb[2][1] = (int) ([color3 greenComponent] * 255);
  rgb[2][2] = (int) ([color3 blueComponent] * 255);

  rgb[3][0] = (int) ([color4 redComponent] * 255);
  rgb[3][1] = (int) ([color4 greenComponent] * 255);
  rgb[3][2] = (int) ([color4 blueComponent] * 255);

  Engine_setColors(engine, rgb, 4);

}


@end
