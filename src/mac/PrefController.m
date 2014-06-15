// Copyright (c) Davorin Šego. All rights reserved.

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "PrefController.h"
#import "misc.h"
#import "shared.h"


@implementation PrefController


- (void)loadFromConfig {

  [self initAudioPreferences];

  [self generateItemsForTransposePopup];
  [_transposePopup selectItemWithTag: engine->config->transpose];

  [_centsOffsetText setFloatValue: engine->config->centsOffset];
  [_centsOffsetStepper setFloatValue: engine->config->centsOffset];

  [_concertPitchText setFloatValue: engine->config->pitchStandard];
  [_concertPitchStepper setFloatValue: engine->config->pitchStandard];

  float gainVal = 20.0 * log10(engine->config->gain);
  [_gainSlider setFloatValue: gainVal];
  [_gainLabel setStringValue: [NSString stringWithFormat: @"%2.0f", gainVal]];

  [_highlightCheckbox setState: engine->config->highlightBands ? NSOnState : NSOffState];

  // add color schemes to pop-up
  for (int i = 0; i < engine->config->schemeCount; ++i) {
    NSString *str = @(engine->config->schemes[i].name);
    [_colorSchemePopup addItemWithTitle: str];
    [[_colorSchemePopup lastItem] setTag: i];
  }
  [_colorSchemePopup selectItemWithTag: engine->config->schemeIndex];

  [self toggleColorWells];

}


- (void) initAudioPreferences {

  // TODO: move this to engine

  // audio input device
  int deviceCount = Engine_deviceCount();

  if (deviceCount > 0) {

    for (int i = 0; i < deviceCount; ++i) {
      const DeviceInfo *info = Engine_deviceInfo(i);
      if (info->maxInputChannels > 0) {
        [_inputDevicePopup addItemWithTitle: @(info->name)];
        [[_inputDevicePopup lastItem] setTag: i];
      }
    }

    // selected device isn't in the list anymore
    bool success = [_inputDevicePopup selectItemWithTag: engine->config->inputDevice];
    if (success == NO) {
      [_inputDevicePopup selectItemWithTag: Engine_getDefaultInputDevice()];
    }

  }

  [_bufferSizePopup addItemWithTitle: @"Auto"];
  [[_bufferSizePopup lastItem] setTag: 0];

  // buffer sizes (zero-terminated)
  {
    int i = 0;
    while (engine->config->inputBufferSizes[i] != 0) {
      [_bufferSizePopup addItemWithTitle:
        [NSString stringWithFormat: @"%i", engine->config->inputBufferSizes[i]]];
      [[_bufferSizePopup lastItem] setTag: engine->config->inputBufferSizes[i]];
      i++;
    }
    [_bufferSizePopup selectItemWithTag: engine->config->inputBufferSize];
  }

  // sample rate
  [_sampleratePopup addItemWithTitle: @"44.1 kHz"];
  [[_sampleratePopup lastItem] setTag: 44100];
  [_sampleratePopup addItemWithTitle: @"48 kHz"];
  [[_sampleratePopup lastItem] setTag: 48000];
  [_sampleratePopup addItemWithTitle: @"88.2 kHz"];
  [[_sampleratePopup lastItem] setTag: 88200];
  [_sampleratePopup addItemWithTitle: @"96 kHz"];
  [[_sampleratePopup lastItem] setTag: 96000];
  [_sampleratePopup selectItemWithTag: engine->config->samplerate];

}


- (IBAction)inputDeviceChanged: (id)sender {

  engine->config->inputDevice = [_inputDevicePopup selectedItem].tag;
  engine->config->inputBufferSize = [_bufferSizePopup selectedItem].tag;
  int samplerate = [_sampleratePopup selectedItem].tag;
  if (samplerate != engine->config->samplerate) {
    engine->config->samplerate = samplerate;
    Engine_setStrobes(engine, engine->currentNote, samplerate);
  }
  int err = Engine_setInputDevice(
    engine,
    engine->config->inputDevice,
    engine->config->samplerate,
    engine->config->inputBufferSize
  );
  if (err) {
    audioDeviceErrorAlert();
  }

}



- (IBAction)transposeChanged: (id)sender {

  engine->config->transpose = [sender selectedItem].tag;
  [[NSNotificationCenter defaultCenter]
    postNotificationName:@"NOTE_CHANGE_NOTIFICATION" object:self];

}


- (IBAction)concertPitchChanged: (id)sender {

  Engine_setPitchStandard(engine, [sender floatValue]);
  [_concertPitchStepper setFloatValue: engine->config->pitchStandard];
  [_concertPitchText setFloatValue: engine->config->pitchStandard];

}


- (IBAction)centsOffsetChanged: (id)sender {

  Engine_setCentsOffset(engine, [sender floatValue]);
  [_centsOffsetText setFloatValue: engine->config->centsOffset];
  [_centsOffsetStepper setFloatValue: engine->config->centsOffset];

}


- (IBAction)gainChanged: (id)sender {

  float gainVal = [sender floatValue];
  engine->config->gain = pow(10.0, gainVal / 20.0);
  [_gainLabel setStringValue: [NSString stringWithFormat: @"%2.0f", gainVal]];

}


- (IBAction)highlightChanged: (id)sender {

  engine->config->highlightBands = ([_highlightCheckbox state] == NSOnState) ? 1 : 0;

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


-(void)toggleColorWells {

  // Custom theme -> show color wells
  if ([self.colorSchemePopup selectedItem].tag == engine->config->schemeCount - 1) {
    [self.colorWellA setHidden: NO];
    [self.colorWellB setHidden: NO];
  }
  else {
    [self.colorWellA setHidden: YES];
    [self.colorWellB setHidden: YES];
  }

}


- (IBAction)colorSchemeChanged: (id)sender {

  int i = engine->config->schemeIndex = [sender selectedItem].tag;

  [self toggleColorWells];

  [self.colorWellA setColor:
    [NSColor colorWithCalibratedRed: engine->config->schemes[i].a[0] / 255
      green: engine->config->schemes[i].a[1] / 255
      blue: engine->config->schemes[i].a[2] / 255
      alpha: 1]];

  [self.colorWellB setColor:
    [NSColor colorWithCalibratedRed: engine->config->schemes[i].b[0] / 255
      green: engine->config->schemes[i].b[1] / 255
      blue: engine->config->schemes[i].b[2] / 255
      alpha: 1]];

}


- (IBAction)colorWellChanged: (id)sender {

  // float h, s, v;

  // h = [color1 hueComponent];
  // s = [color1 saturationComponent];
  // v = [color1 brightnessComponent];

  int i = engine->config->schemeIndex;

  if ([self.colorWellA isEqual: sender]) {
    NSColor *color1 = [self.colorWellA color];
    engine->config->schemes[i].a[0] = 255 * [color1 redComponent];
    engine->config->schemes[i].a[1] = 255 * [color1 greenComponent];
    engine->config->schemes[i].a[2] = 255 * [color1 blueComponent];
  }
  else if ([self.colorWellB isEqual: sender]) {
    NSColor *color2 = [self.colorWellB color];
    engine->config->schemes[i].b[0] = 255 * [color2 redComponent];
    engine->config->schemes[i].b[1] = 255 * [color2 greenComponent];
    engine->config->schemes[i].b[2] = 255 * [color2 blueComponent];
  }

}



@end
