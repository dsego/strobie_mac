/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import "AppDelegate.h"
#import "shared.h"


@implementation AppDelegate {

  NSThread* estimatePitchThread;

}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

  engine = Engine_create();
  Engine_startAudio(engine);

  [_mainController.strobeView setupBuffers];

  estimatePitchThread = [
    [NSThread alloc] initWithTarget:self
    selector:@selector(estimatePitch)
    object:nil
  ];

  [estimatePitchThread start];

}


- (void)applicationWillTerminate:(NSNotification *)aNotification {

  [estimatePitchThread cancel];
  Engine_stopAudio(engine);
  Engine_destroy(engine);

}


- (void)estimatePitch {

  while ([estimatePitchThread isCancelled] == NO) {

    float pitch = Engine_estimatePitch(engine);

    Note note = Tuning12TET_find(
      pitch,
      engine->config->pitchStandard,
      engine->config->centsOffset,
      engine->config->transpose
    );

    float cents = Tuning12TET_freqToCents(pitch, engine->config->pitchStandard);

    Engine_setStrobeFreq(engine, note.frequency);
    [NSThread sleepForTimeInterval:0.05];

  }

}


- (void)applicationWillUnhide:(NSNotification *)aNotification {

  // engine start ?
  // PaError Pa_StartStream  ( PaStream *  stream  )
  //


}


- (void)applicationWillHide:(NSNotification *)aNotification {

  // engine stop ?
  // PaError Pa_StopStream  ( PaStream *  stream  )

}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {

  return YES;

}



-(IBAction)showHelp:(id)sender {

  id url = [NSURL URLWithString:@"http://www.google.com"];
  [[NSWorkspace sharedWorkspace] openURL:url];

}



@end