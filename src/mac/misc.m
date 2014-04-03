// Copyright (c) Davorin Šego. All rights reserved.


#import <Cocoa/Cocoa.h>
#import "misc.h"


void audioDeviceErrorAlert(void) {

  NSAlert *alert = [[NSAlert alloc] init];
  alert.messageText = @"Audio device error";
  alert.informativeText = @"Current audio settings aren't working. "
                          "Please try a different sample rate and buffer size, "
                          "or choose a different audio device.";
  [alert runModal];

}


void openWebsiteInBrowser(NSString* url) {

  [[NSWorkspace sharedWorkspace] openURL: [NSURL URLWithString: url]];

}
