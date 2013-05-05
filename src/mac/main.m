/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

void buildMainMenu();

int main(int argc, char *argv[])
{
  AppDelegate* delegate = [[AppDelegate alloc] init];
  id sharedApplication = [NSApplication sharedApplication];
  [sharedApplication setDelegate:delegate];
  buildMainMenu();
  [NSApp run];
  return 0;
}


void buildMainMenu()
{
  NSString* appName = [[NSProcessInfo processInfo] processName];

  id mainMenu = [[NSMenu alloc] init];

  // application
  id appMenuItem = [[NSMenuItem alloc] init];
  [mainMenu addItem:appMenuItem];

  // application sub menu
  id appMenu = [[NSMenu alloc] init];
  [appMenuItem setSubmenu:appMenu];

  // about
  id aboutMenuItem = [[NSMenuItem alloc] initWithTitle:[@"About " stringByAppendingString:appName]
                                         action:@selector(orderFrontStandardAboutPanel:)
                                         keyEquivalent:@""];
  [appMenu addItem:aboutMenuItem];
  [appMenu addItem:NSMenuItem.separatorItem];

  // preferences
  id prefMenuItem = [[NSMenuItem alloc] initWithTitle:@"Preferences..."
                                        action:NULL
                                        keyEquivalent:@","];
  [appMenu addItem:prefMenuItem];
  [appMenu addItem:NSMenuItem.separatorItem];

  // hide
  id hideMenuItem = [[NSMenuItem alloc] initWithTitle:[@"Hide " stringByAppendingString:appName]
                                        action:@selector(hide:)
                                        keyEquivalent:@"h"];
  [appMenu addItem:hideMenuItem];

  // hide others
  id hideOthersMenuItem = [[NSMenuItem alloc] initWithTitle:@"Hide Others"
                                              action:@selector(hideOtherApplications:)
                                              keyEquivalent:@"h"];
  [hideOthersMenuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask | NSCommandKeyMask)];
  [appMenu addItem:hideOthersMenuItem];

  // show all
  id showAllMenuItem = [[NSMenuItem alloc] initWithTitle:@"Show All"
                                           action:@selector(unhideAllApplications:)
                                           keyEquivalent:@""];
  [appMenu addItem:showAllMenuItem];
  [appMenu addItem:NSMenuItem.separatorItem];

  // quit
  id quitMenuItem = [[NSMenuItem alloc] initWithTitle:[@"Quit " stringByAppendingString:appName]
                                        action:@selector(terminate:)
                                        keyEquivalent:@"q"];
  [appMenu addItem:quitMenuItem];




  // window
  id windowMenuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
  [mainMenu addItem:windowMenuItem];

  // window sub menu
  id windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
  [windowMenuItem setSubmenu:windowMenu];

  // minimize
  id minimizeMenuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize"
                                            action:@selector(performMiniaturize:)
                                            keyEquivalent:@"m"];
  [windowMenu addItem:minimizeMenuItem];

  // zoom
  id zoomMenuItem = [[NSMenuItem alloc] initWithTitle:@"Zoom"
                                        action:@selector(performZoom:)
                                        keyEquivalent:@""];
  [windowMenu addItem:zoomMenuItem];
  [windowMenu addItem:NSMenuItem.separatorItem];

  // bring to front
  id frontMenuItem = [[NSMenuItem alloc] initWithTitle:@"Bring All to Front"
                                         action:@selector(arrangeInFront:)
                                         keyEquivalent:@""];
  [windowMenu addItem:frontMenuItem];



  // help
  id helpMenuItem = [[NSMenuItem alloc] initWithTitle:@"Help" action:nil keyEquivalent:@""];
  [mainMenu addItem:helpMenuItem];

  // help sub menu
  id helpMenu = [[NSMenu alloc] initWithTitle:@"Help"];
  [helpMenuItem setSubmenu:helpMenu];

  // documentation
  id docsMenuItem = [[NSMenuItem alloc] initWithTitle:[appName stringByAppendingString:@" Help"]
                                        action:@selector(openOnlineDocumentation:)
                                        keyEquivalent:@""];
  [helpMenu addItem:docsMenuItem];

  // add a main men
  [NSApp setMainMenu:mainMenu];
}
