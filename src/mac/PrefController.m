/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "PrefController.h"
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

  [_maxGainSlider setFloatValue: 20.0 * log10(engine->config->maxGain)];

}


- (void) initAudioPreferences {

  // TODO: move this to engine

  // audio input device
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

  // selected device isn't in the list anymore
  bool success = [_inputDevicePopup selectItemWithTag: engine->config->inputDevice];
  if (success == NO) {
    [_inputDevicePopup selectItemWithTag: Engine_getDefaultInputDevice()];
  }

  [_bufferSizePopup addItemWithTitle: @"Auto"];
  [[_bufferSizePopup lastItem] setTag: 0];

  // buffer size
  for (int i = 32; i < 256; i+= 32) {
    [_bufferSizePopup addItemWithTitle: [NSString stringWithFormat: @"%i", i]];
    [[_bufferSizePopup lastItem] setTag: i];
  }

  for (int i = 256; i < 512; i+= 64) {
    [_bufferSizePopup addItemWithTitle: [NSString stringWithFormat: @"%i", i]];
    [[_bufferSizePopup lastItem] setTag: i];
  }

  for (int i = 512; i <= 1024; i+= 128) {
    [_bufferSizePopup addItemWithTitle: [NSString stringWithFormat: @"%i", i]];
    [[_bufferSizePopup lastItem] setTag: i];
  }

  [_bufferSizePopup selectItemWithTag: engine->config->inputBufferSize];

  // sample rate
  [_sampleratePopup addItemWithTitle: @"16000 Hz"];
  [[_sampleratePopup lastItem] setTag: 16000];
  [_sampleratePopup addItemWithTitle: @"32000 Hz"];
  [[_sampleratePopup lastItem] setTag: 32000];
  [_sampleratePopup addItemWithTitle: @"44100 Hz"];
  [[_sampleratePopup lastItem] setTag: 44100];
  [_sampleratePopup addItemWithTitle: @"48000 Hz"];
  [[_sampleratePopup lastItem] setTag: 48000];
  [_sampleratePopup addItemWithTitle: @"88200 Hz"];
  [[_sampleratePopup lastItem] setTag: 88200];
  [_sampleratePopup addItemWithTitle: @"96000 Hz"];
  [[_sampleratePopup lastItem] setTag: 96000];
  [_sampleratePopup selectItemWithTag: engine->config->samplerate];

}


- (IBAction)inputDeviceChanged: (id)sender {

  int device = [_inputDevicePopup selectedItem].tag;
  int bufferSize = [_bufferSizePopup selectedItem].tag;
  int samplerate = [_sampleratePopup selectedItem].tag;
  if (samplerate != engine->config->samplerate) {
    Engine_setStrobes(engine, engine->currentNote, samplerate);
  }
  int err = Engine_setInputDevice(engine, device, samplerate, bufferSize);
  // if (err) {
  //   NSAlert *alert = [[NSAlert alloc] init];
  //   alert.messageText = @"Audio device error";
  //   alert.informativeText = @"Current audio settings aren't working. "
  //     "Please try a different sample rate and buffer size, "
  //     "or choose a different audio device.";
  //   [alert runModal];
  // }

}



- (IBAction)transposeChanged: (id)sender {

  engine->config->transpose = [sender selectedItem].tag;
  [[NSNotificationCenter defaultCenter] postNotificationName:@"NoteChangeNotification" object:self];

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


- (IBAction)maxGainChanged: (id)sender {

  engine->config->maxGain = pow(10.0, [sender floatValue] / 20.0);

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
