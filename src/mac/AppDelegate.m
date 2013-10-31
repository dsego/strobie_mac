/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import "AppDelegate.h"
#import "shared.h"


@implementation AppDelegate {

  NSThread* estimatePitchThread;
  NSTimer *setStrobesTimer;
  NSTimer *strobeDisplayTimer;
  Note note;

}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

  [_prefController loadPreferences];
  Engine_setInputDevice(engine, engine->config->inputDevice, engine->config->samplerate);

  estimatePitchThread = [[NSThread alloc] initWithTarget:self selector:@selector(estimatePitch) object:nil ];
  [estimatePitchThread start];


  // a timer works on the main thread

  setStrobesTimer = [NSTimer timerWithTimeInterval:0.05 target:self selector:@selector(setStrobes) userInfo:nil repeats:YES];
  [setStrobesTimer setTolerance: 0.01];
  [[NSRunLoop currentRunLoop] addTimer:setStrobesTimer forMode:NSDefaultRunLoopMode];

  strobeDisplayTimer = [NSTimer timerWithTimeInterval:0.01 target:_mainController.strobeView selector:@selector(redraw) userInfo:nil repeats:YES];
  [strobeDisplayTimer setTolerance: 0.01];
  [[NSRunLoop currentRunLoop] addTimer:strobeDisplayTimer forMode:NSDefaultRunLoopMode];
  [[NSRunLoop currentRunLoop] addTimer:strobeDisplayTimer forMode:NSEventTrackingRunLoopMode]; //Ensure timer fires during resize

}


- (void)applicationDidChangeOcclusionState:(NSNotification *)notification {

  if ([NSApp occlusionState] & NSApplicationOcclusionStateVisible) {
    // Visible

  } else {
    // Occluded


  }


}


- (void)applicationWillTerminate:(NSNotification *)aNotification {

  [estimatePitchThread cancel];
  Engine_destroy(engine);

}


- (void)setStrobes {

  if (engine->mode == AUTO) {
    Engine_setStrobes(engine, note);
  }

}


- (void)estimatePitch {

  // near-Nyquist
  float maxFreq = 0.45 * engine->config->samplerate;

  while ([estimatePitchThread isCancelled] == NO) {

    if (engine->mode == AUTO) {
      float pitch = Engine_estimatePitch(engine);

      // sanity check
      if (pitch > 0.0 && pitch < maxFreq) {
        note = Tuning12TET_find(pitch, engine->config->pitchStandard, engine->config->centsOffset);
        // float cents = Tuning12TET_freqToCents(pitch, engine->config->pitchStandard);
        // Note transposed = Tuning12TET_transpose(note, engine->config->transpose);
      }
    }

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

  return NO;

}



-(IBAction)showHelp:(id)sender {

  id url = [NSURL URLWithString:@"http://www.google.com"];
  [[NSWorkspace sharedWorkspace] openURL:url];

}



@end