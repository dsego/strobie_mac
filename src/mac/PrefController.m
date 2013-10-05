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
      [self.inputDevicePopup addItemWithTitle: [NSString stringWithCString: name encoding: NSASCIIStringEncoding]];
      [[self.inputDevicePopup lastItem] setTag: i];
    }

  }

}


- (IBAction)inputDeviceChanged: (id)sender {

  int device = [sender selectedItem].tag;
  Engine_setInputDevice(engine, device, engine->config->samplerate);

}


- (IBAction)concertPitchChanged: (id)sender {


}


- (IBAction)offsetChanged: (id)sender {


}





@end