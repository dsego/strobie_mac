/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "shared.h"


int main(int argc, char *argv[]) {

  engine = Engine_create();
  Engine_initAudio(engine);
  return NSApplicationMain(argc, (const char **)argv);

}