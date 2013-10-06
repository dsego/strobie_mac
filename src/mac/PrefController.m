/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
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

  [_transposePopup selectItemWithTag: engine->config->inputDevice];



  _concertPitchText.floatValue = engine->config->pitchStandard;
  _offsetText.floatValue = engine->config->centsOffset;

  [_transposePopup addItemWithTitle: @"-12"];
  [[_transposePopup lastItem] setTag: -12];

  [_transposePopup addItemWithTitle: @"-11"];
  [[_transposePopup lastItem] setTag: -11];

  [_transposePopup addItemWithTitle: @"-1"];
  [[_transposePopup lastItem] setTag: -10];

  [_transposePopup addItemWithTitle: @"-9"];
  [[_transposePopup lastItem] setTag: -9];

  [_transposePopup addItemWithTitle: @"-8"];
  [[_transposePopup lastItem] setTag: -8];

  [_transposePopup addItemWithTitle: @"-7"];
  [[_transposePopup lastItem] setTag: -7];

  [_transposePopup addItemWithTitle: @"-6"];
  [[_transposePopup lastItem] setTag: -6];

  [_transposePopup addItemWithTitle: @"-5"];
  [[_transposePopup lastItem] setTag: -5];

  [_transposePopup addItemWithTitle: @"-4"];
  [[_transposePopup lastItem] setTag: -4];

  [_transposePopup addItemWithTitle: @"-3"];
  [[_transposePopup lastItem] setTag: -3];

  [_transposePopup addItemWithTitle: @"-2"];
  [[_transposePopup lastItem] setTag: -2];

  [_transposePopup addItemWithTitle: @"-1"];
  [[_transposePopup lastItem] setTag: -1];

  [_transposePopup addItemWithTitle: @"±0 Concert"];
  [[_transposePopup lastItem] setTag: 0];

  [_transposePopup addItemWithTitle: @"+1"];
  [[_transposePopup lastItem] setTag: 1];

  [_transposePopup addItemWithTitle: @"+2"];
  [[_transposePopup lastItem] setTag: 2];

  [_transposePopup addItemWithTitle: @"+3"];
  [[_transposePopup lastItem] setTag: 3];

  [_transposePopup addItemWithTitle: @"+4"];
  [[_transposePopup lastItem] setTag: 4];

  [_transposePopup addItemWithTitle: @"+5"];
  [[_transposePopup lastItem] setTag: 5];

  [_transposePopup addItemWithTitle: @"+6"];
  [[_transposePopup lastItem] setTag: 6];

  [_transposePopup addItemWithTitle: @"+7"];
  [[_transposePopup lastItem] setTag: 7];

  [_transposePopup addItemWithTitle: @"+8"];
  [[_transposePopup lastItem] setTag: 8];

  [_transposePopup addItemWithTitle: @"+9"];
  [[_transposePopup lastItem] setTag: 9];

  [_transposePopup addItemWithTitle: @"+10"];
  [[_transposePopup lastItem] setTag: 10];

  [_transposePopup addItemWithTitle: @"+11"];
  [[_transposePopup lastItem] setTag: 11];

  [_transposePopup addItemWithTitle: @"+12"];
  [[_transposePopup lastItem] setTag: 12];

  [_transposePopup selectItemWithTag: engine->config->transpose];

}


- (IBAction)inputDeviceChanged: (id)sender {

  int device = [sender selectedItem].tag;
  Engine_setInputDevice(engine, device, engine->config->samplerate);

}


- (IBAction)concertPitchChanged: (id)sender {

  engine->config->pitchStandard = [sender floatValue];

}


- (IBAction)offsetChanged: (id)sender {

  engine->config->centsOffset = [sender floatValue];

}


- (IBAction)transposeChanged: (id)sender {

  engine->config->transpose = [sender selectedItem].tag;;

}





@end