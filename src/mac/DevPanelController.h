/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>


@interface DevPanelController : NSWindowController

  @property IBOutlet NSColorWell *colorWell1;
  @property IBOutlet NSColorWell *colorWell2;
  @property IBOutlet NSColorWell *colorWell3;
  @property IBOutlet NSColorWell *colorWell4;

  - (IBAction)colorWellChanged: (id)sender;

@end
