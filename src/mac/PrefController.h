/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>


@interface PrefController : NSWindowController

  @property IBOutlet NSPopUpButton *inputDevicePopup;
  @property IBOutlet NSPopUpButton *transposePopup;
  @property IBOutlet NSTextField *concertPitchText;
  @property IBOutlet NSStepper *concertPitchStepper;
  @property IBOutlet NSTextField *centsOffsetText;
  @property IBOutlet NSStepper *centsOffsetStepper;

  - (void)loadPreferences;

  - (IBAction)inputDeviceChanged: (id)sender;
  - (IBAction)transposeChanged: (id)sender;
  - (IBAction)concertPitchChanged: (id)sender;
  - (IBAction)centsOffsetChanged: (id)sender;


@end