//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AppKit/NSMenuItem.h>
#import "AppDelegate.h"

int main(int argc, char *argv[])
{
  AppDelegate* delegate = [[AppDelegate alloc] init];
  [[NSApplication sharedApplication] setDelegate:delegate];
  // id menubar = [NSMenu new];
  // id appMenuItem = [NSMenuItem new];
  // [menubar addItem:appMenuItem];
  // [NSApp setMainMenu:menubar];
  //
  //
  NSMenu *newMenu;
    NSMenuItem *newItem;


  // NSMenuItem *fileMenuItem = [NSMenuItem alloc];
  // [[NSApp mainMenu] addItem: fileMenuItem];

  [NSApp run];
  return 0;
}
