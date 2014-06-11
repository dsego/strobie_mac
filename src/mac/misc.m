// Copyright (c) Davorin Šego. All rights reserved.


#import <Cocoa/Cocoa.h>
#import "misc.h"


void audioDeviceErrorAlert(void) {

  NSAlert *alert = [[NSAlert alloc] init];
  alert.messageText = @"No audio input";
  alert.informativeText = @"Try changing your audio settings or choose a different device.";
  [alert runModal];

}


void openWebsiteInBrowser(NSString* url) {

  [[NSWorkspace sharedWorkspace] openURL: [NSURL URLWithString: url]];

}
