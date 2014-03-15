// Copyright (c) Davorin Šego. All rights reserved.

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "shared.h"


static void finish(void) {

  Engine_destroy(engine);

}


int main(int argc, char *argv[]) {

  engine = Engine_create();
  atexit(finish);
  return NSApplicationMain(argc, (const char **)argv); // never returns

}
