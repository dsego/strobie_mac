/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

  @property (strong) IBOutlet MainController *mainController;

  -(IBAction) showHelp:(id)sender;

@end
