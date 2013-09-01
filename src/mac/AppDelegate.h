/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "PrefController.h"
#import "../engine/Engine.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

  @property Engine* engine;
  @property (strong) MainController* mainController;
  @property (strong) PrefController* prefController;

  -(IBAction) openOnlineDocumentation:(id)sender;
  -(IBAction) openPreferences:(id)sender;

@end
