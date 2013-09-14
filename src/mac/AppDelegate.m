/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <AppKit/NSMenuItem.h>
#import "AppDelegate.h"
#import "shared.h"


@implementation AppDelegate {

  NSThread* estimatePitchThread;
  NSThread* refreshStrobeThread;

}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

  engine = Engine_create();
  Engine_startAudio(engine);

  estimatePitchThread = [
    [NSThread alloc] initWithTarget:self
    selector:@selector(estimatePitch)
    object:nil
  ];

  refreshStrobeThread = [
    [NSThread alloc] initWithTarget:self
    selector:@selector(refreshStrobe)
    object:nil
  ];

  [estimatePitchThread start];
  [refreshStrobeThread start];

}


- (void)applicationWillTerminate:(NSNotification *)aNotification {

  [estimatePitchThread cancel];
  [refreshStrobeThread cancel];
  Engine_stopAudio(engine);
  Engine_destroy(engine);

}


- (void)estimatePitch {

  while (true) {

    if ([estimatePitchThread isCancelled]) { break; }

    float pitch = Engine_estimatePitch(engine);

    Note note = Tuning12TET_find(
      pitch,
      engine->config->pitchStandard,
      engine->config->centsOffset,
      engine->config->transpose
    );

    float cents = Tuning12TET_freqToCents(pitch, engine->config->pitchStandard);

    Engine_setStrobeFreq(engine, note.frequency);
    usleep(50000);

  }

}


-(void)refreshStrobe {

  while (true) {

    if ([estimatePitchThread isCancelled]) { break; }
    // [_mainController.window.contentView setNeedsDisplay: YES];
    usleep(40000);

  }

}


// -(IBAction)showHelp:(id)sender {

//   id url = [NSURL URLWithString:@"http://www.google.com"];
//   [[NSWorkspace sharedWorkspace] openURL:url];

// }


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