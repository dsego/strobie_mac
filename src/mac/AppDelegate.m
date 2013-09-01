/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSMenuItem.h>
#import "AppDelegate.h"
#import "../engine/Engine.h"


@implementation AppDelegate

NSThread* estimatePitchThread;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

  _engine = Engine_create();
  Engine_startAudio(_engine);

  _mainController = [[MainController alloc] init: _engine];
  [_mainController showWindow];
//    NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:0.02
//                                                      target:self
//                                                    selector:@selector(refreshNote)
//                                                    userInfo:nil
//                                                     repeats:YES];
//    [timer fire];


  estimatePitchThread = [
    [NSThread alloc] initWithTarget:self
    selector:@selector(estimatePitch)
    object:nil
  ];
  [estimatePitchThread start];

  NSThread* refreshStrobeThread = [
    [NSThread alloc] initWithTarget:self
    selector:@selector(refreshStrobe)
    object:nil
  ];
  [refreshStrobeThread start];

}


- (void)applicationWillTerminate:(NSNotification *)aNotification {

  [estimatePitchThread cancel];
  Engine_stopAudio(_engine);
  Engine_destroy(_engine);

}


- (void)estimatePitch {

  while (true) {

    if ([estimatePitchThread isCancelled]) {
      break;
    }

    float pitch = Engine_estimatePitch(_engine);


    Note note = Tuning12TET_find(
      pitch,
      _engine->config->pitchStandard,
      _engine->config->centsOffset,
      _engine->config->transpose
    );

    float cents = Tuning12TET_freqToCents(pitch, _engine->config->pitchStandard);
    printf("%4.2f Hz   %2.2f c    \r", pitch, cents - note.cents);
    fflush(stdout);


    // Engine_setStrobeFreq(_engine, 82.41);
    Engine_setStrobeFreq(_engine, note.frequency);
    usleep(50000);
  }

}


-(void)refreshStrobe {

  while (true) {

    [_mainController drawStrobe];
    usleep(40000);

  }

}



- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {

  return YES;

}


-(IBAction)openOnlineDocumentation:(id)sender {

  id url = [NSURL URLWithString:@"http://www.google.com"];
  [[NSWorkspace sharedWorkspace] openURL:url];

}


@end