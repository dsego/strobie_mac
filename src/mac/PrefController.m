/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "PrefController.h"
#import "shared.h"


@implementation PrefController


- (void)loadPreferences {

  int deviceCount = Engine_deviceCount();

  for (int i = 0; i < deviceCount; ++i) {

    char name[100];
    int isInput;
    int isOutput;
    Engine_deviceName(i, name, &isInput, &isOutput);

    if (isInput) {
      [_inputDevicePopup addItemWithTitle: [NSString stringWithCString: name encoding: NSASCIIStringEncoding]];
      [[_inputDevicePopup lastItem] setTag: i];
    }

  }

  [_inputDevicePopup selectItemWithTag: engine->config->inputDevice];

  [self generateItemsForTransposePopup];
  [_transposePopup selectItemWithTag: engine->config->transpose];


  [_centsOffsetText setFloatValue: engine->config->centsOffset];
  [_centsOffsetStepper setFloatValue: engine->config->centsOffset];

  [_concertPitchText setFloatValue: engine->config->pitchStandard];
  [_concertPitchStepper setFloatValue: engine->config->pitchStandard];

}


- (IBAction)inputDeviceChanged: (id)sender {

  int device = [sender selectedItem].tag;
  Engine_setInputDevice(engine, device, engine->config->samplerate);

}


- (IBAction)transposeChanged: (id)sender {

  engine->config->transpose = [sender selectedItem].tag;

  // there should be a nicer way to do this
  AppDelegate *delegate = [NSApp delegate];
  [delegate.mainController refreshNoteDisplay];

}


- (IBAction)concertPitchChanged: (id)sender {

  float val = [sender floatValue];

  // not an empty text field
  if (val > 0) {
     engine->config->pitchStandard = val;
  }
  [_concertPitchStepper setFloatValue: engine->config->pitchStandard];
  [_concertPitchText setFloatValue: engine->config->pitchStandard];

}


- (IBAction)centsOffsetChanged: (id)sender {

  engine->config->centsOffset = [sender floatValue];
  [_centsOffsetText setFloatValue: engine->config->centsOffset];
  [_centsOffsetStepper setFloatValue: engine->config->centsOffset];

}


- (void)generateItemsForTransposePopup {

  [_transposePopup addItemWithTitle: @"-11 Db"];
  [[_transposePopup lastItem] setTag: -11];

  [_transposePopup addItemWithTitle: @"-10 D"];
  [[_transposePopup lastItem] setTag: -10];

  [_transposePopup addItemWithTitle: @"-9  Eb"];
  [[_transposePopup lastItem] setTag: -9];

  [_transposePopup addItemWithTitle: @"-8  E"];
  [[_transposePopup lastItem] setTag: -8];

  [_transposePopup addItemWithTitle: @"-7  F"];
  [[_transposePopup lastItem] setTag: -7];

  [_transposePopup addItemWithTitle: @"-6  Gb"];
  [[_transposePopup lastItem] setTag: -6];

  [_transposePopup addItemWithTitle: @"-5  G"];
  [[_transposePopup lastItem] setTag: -5];

  [_transposePopup addItemWithTitle: @"-4  Ab"];
  [[_transposePopup lastItem] setTag: -4];

  [_transposePopup addItemWithTitle: @"-3  A"];
  [[_transposePopup lastItem] setTag: -3];

  [_transposePopup addItemWithTitle: @"-2  Bb"];
  [[_transposePopup lastItem] setTag: -2];

  [_transposePopup addItemWithTitle: @"-1  B"];
  [[_transposePopup lastItem] setTag: -1];

  [_transposePopup addItemWithTitle: @"±0  C"];
  [[_transposePopup lastItem] setTag: 0];

  [_transposePopup addItemWithTitle: @"+1  Db"];
  [[_transposePopup lastItem] setTag: 1];

  [_transposePopup addItemWithTitle: @"+2  D"];
  [[_transposePopup lastItem] setTag: 2];

  [_transposePopup addItemWithTitle: @"+3  Eb"];
  [[_transposePopup lastItem] setTag: 3];

  [_transposePopup addItemWithTitle: @"+4  E"];
  [[_transposePopup lastItem] setTag: 4];

  [_transposePopup addItemWithTitle: @"+5  F"];
  [[_transposePopup lastItem] setTag: 5];

  [_transposePopup addItemWithTitle: @"+6  Gb"];
  [[_transposePopup lastItem] setTag: 6];

  [_transposePopup addItemWithTitle: @"+7  G"];
  [[_transposePopup lastItem] setTag: 7];

  [_transposePopup addItemWithTitle: @"+8  Ab"];
  [[_transposePopup lastItem] setTag: 8];

  [_transposePopup addItemWithTitle: @"+9  A"];
  [[_transposePopup lastItem] setTag: 9];

  [_transposePopup addItemWithTitle: @"+10 Bb"];
  [[_transposePopup lastItem] setTag: 10];

  [_transposePopup addItemWithTitle: @"+11 B"];
  [[_transposePopup lastItem] setTag: 11];

}


@end