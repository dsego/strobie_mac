/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>


@interface PrefController : NSWindowController

  @property IBOutlet NSPopUpButton *inputDevicePopup;
  @property IBOutlet NSTextField *concertPitchText;
  @property IBOutlet NSTextField *offsetText;
  @property IBOutlet NSPopUpButton *transposePopup;

  - (IBAction)inputDeviceChanged: (id)sender;
  - (IBAction)concertPitchChanged: (id)sender;
  - (IBAction)offsetChanged: (id)sender;
  - (IBAction)transposeChanged: (id)sender;

  - (void)loadPreferences;

@end