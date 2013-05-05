/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property Engine* engine;
@property (strong) MainController* mainController;

-(IBAction) openOnlineDocumentation:(id)sender;

@end
