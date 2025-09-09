// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.


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
