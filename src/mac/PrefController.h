/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSWindowController.h>


@interface PrefController : NSWindowController

  @property IBOutlet NSPopUpButton *inputDevicePopup;
  @property IBOutlet NSPopUpButton *sampleratePopup;
  @property IBOutlet NSPopUpButton *bufferSizePopup;
  @property IBOutlet NSPopUpButton *transposePopup;
  @property IBOutlet NSTextField *concertPitchText;
  @property IBOutlet NSStepper *concertPitchStepper;
  @property IBOutlet NSTextField *centsOffsetText;
  @property IBOutlet NSStepper *centsOffsetStepper;
  @property IBOutlet NSSlider *maxGainSlider;
  @property IBOutlet NSTextField *maxGainLabel;

  - (void)loadFromConfig;

  - (IBAction)inputDeviceChanged: (id)sender;
  - (IBAction)transposeChanged: (id)sender;
  - (IBAction)concertPitchChanged: (id)sender;
  - (IBAction)centsOffsetChanged: (id)sender;
  - (IBAction)maxGainChanged: (id)sender;


@end
