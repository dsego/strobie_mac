/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>


@interface DevPanelController : NSWindowController

  @property IBOutlet NSColorWell *colorWellA;
  @property IBOutlet NSColorWell *colorWellB;

  - (IBAction)colorWellChanged: (id)sender;

@end