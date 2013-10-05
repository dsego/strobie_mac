/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "shared.h"


@implementation MainController

- (void)windowWillClose:(NSNotification *)notification {

  [NSApp terminate: self];

}


@end