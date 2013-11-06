/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import "AppDelegate.h"
#import "shared.h"


@implementation AppDelegate {

  NSThread* estimatePitchThread;
  NSTimer *setStrobesTimer;
  NSTimer *strobeDisplayTimer;

}


- (void)savePreferences {

  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

  [defaults setInteger: engine->config->inputDevice forKey: @"inputDevice"];
  [defaults setInteger: engine->config->transpose forKey: @"transpose"];
  [defaults setFloat: engine->config->pitchStandard forKey: @"pitchStandard"];
  [defaults setFloat: engine->config->centsOffset forKey: @"centsOffset"];
  [defaults setFloat: engine->config->freq forKey: @"freq"];
  [defaults synchronize];

}


- (void)loadPreferences {

  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

  // register factory defaults
  NSDictionary *factoryValues = [NSDictionary dictionaryWithObjectsAndKeys:
    [NSNumber numberWithInt: engine->config->inputDevice], @"inputDevice",
    [NSNumber numberWithInt: engine->config->transpose], @"transpose",
    [NSNumber numberWithFloat: engine->config->pitchStandard], @"pitchStandard",
    [NSNumber numberWithFloat: engine->config->centsOffset], @"centsOffset",
    [NSNumber numberWithFloat: engine->config->freq], @"freq",
    nil];
  [defaults registerDefaults:factoryValues];

  // read saved values
  engine->config->inputDevice   = [defaults integerForKey: @"inputDevice"];
  engine->config->transpose     = [defaults integerForKey: @"transpose"];
  engine->config->pitchStandard = [defaults floatForKey: @"pitchStandard"];
  engine->config->centsOffset   = [defaults floatForKey: @"centsOffset"];
  engine->config->freq          = [defaults floatForKey: @"freq"];

  [_prefController loadFromConfig];

}


- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {

  [self loadPreferences];

  Note note = Tuning12TET_find(engine->config->freq, engine->config->pitchStandard, engine->config->centsOffset);
  Engine_setStrobes(engine, note);
  Engine_setInputDevice(engine, engine->config->inputDevice, engine->config->samplerate);

  // this is ugly as hell
  [_mainController refreshNoteDisplay];
  [_mainController.strobeView setupBuffers];

  // estimatePitchThread = [[NSThread alloc] initWithTarget:self selector:@selector(estimatePitch) object:nil ];
  // [estimatePitchThread start];


  // setStrobesTimer = [NSTimer timerWithTimeInterval:0.05 target:self selector:@selector(setStrobes) userInfo:nil repeats:YES];
  // [setStrobesTimer setTolerance: 0.01];
  // [[NSRunLoop currentRunLoop] addTimer:setStrobesTimer forMode:NSDefaultRunLoopMode];

  strobeDisplayTimer = [NSTimer
    timerWithTimeInterval:0.01
    target:self
    selector:@selector(redrawStrobes:)
    userInfo:nil
    repeats:YES
  ];

  [strobeDisplayTimer setTolerance: 0.01];
  [[NSRunLoop currentRunLoop] addTimer:strobeDisplayTimer forMode:NSDefaultRunLoopMode];

  //Ensure timer fires during resize
  [[NSRunLoop currentRunLoop] addTimer:strobeDisplayTimer forMode:NSEventTrackingRunLoopMode];

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
  [self savePreferences];
  Engine_destroy(engine);

}


- (void)redrawStrobes: (NSTimer *)timer {

  [_mainController.strobeView setNeedsDisplay:YES];

}


- (void)setStrobes {

  if (engine->mode == AUTO) {
    Engine_setStrobes(engine, engine->currentNote);
    [_mainController.noteView setNeedsDisplay: YES];
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
        engine->currentNote = Tuning12TET_find(pitch, engine->config->pitchStandard, engine->config->centsOffset);
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