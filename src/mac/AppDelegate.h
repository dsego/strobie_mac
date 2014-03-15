// Copyright (c) Davorin Šego. All rights reserved.

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "PrefController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

  @property (strong) IBOutlet MainController *mainController;
  @property (strong) IBOutlet PrefController *prefController;

  -(IBAction) showHelp:(id)sender;
  -(IBAction) showStrobieWebsite:(id)sender;

@end
