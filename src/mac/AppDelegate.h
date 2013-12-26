/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "PrefController.h"
#import "DevPanelController.h"


@interface AppDelegate : NSObject <NSApplicationDelegate>

  @property (strong) IBOutlet MainController *mainController;
  @property (strong) IBOutlet PrefController *prefController;
  @property (strong) IBOutlet DevPanelController *devPanelController;

  -(IBAction) showHelp:(id)sender;
  -(IBAction) showStrobieWebsite:(id)sender;

@end
