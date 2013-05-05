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

  self.mainController = [[MainController alloc] init];
  self.mainController.strobeLayer.engine = _engine;
  [self.mainController showWindow];
//    NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:0.02
//                                                      target:self
//                                                    selector:@selector(refreshNote)
//                                                    userInfo:nil
//                                                     repeats:YES];
//    [timer fire];


  estimatePitchThread = [[NSThread alloc] initWithTarget:self
                                          selector:@selector(estimatePitch)
                                          object:nil];

  [estimatePitchThread start];


  // NSThread* refreshStrobeThread = [[NSThread alloc] initWithTarget:self
  //                                                   selector:@selector(refreshStrobe:)
  //                                                   object:nil];
  // [refreshStrobeThread start];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {

  [estimatePitchThread cancel];
  Engine_stopAudio(_engine);
  Engine_destroy(_engine);

}


- (void)estimatePitch {

  double pitch = 0;
  while (true) {
    if ([estimatePitchThread isCancelled]) {
      break;
    }

    pitch = Engine_estimatePitch(_engine);
    [_mainController refreshPitch:pitch peak:0];

    // Note note  = Tuning12TET_find(pitch, 440.0, 0.0, 0);
    // Engine_setStrobeFreq(engine, note.frequency);
     // NSLog([NSString stringWithFormat:@"%f", pitch]);
//        [self.label setStringValue:[NSString stringWithFormat:@"%f", pitch]];
    // [self.label setStringValue:[NSString stringWithFormat:@"%c%d", note.letter, note.octave]];
    usleep(200000);
    // usleep(50000);
  }

}


-(void)refreshStrobe {

  // Engine_readStrobes(engine);
  // [self.mainController drawStrobe: engine];

}



- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {

  return YES;

}


-(IBAction)openOnlineDocumentation:(id)sender {

  id url = [NSURL URLWithString:@"http://www.google.com"];
  [[NSWorkspace sharedWorkspace] openURL:url];

}


@end