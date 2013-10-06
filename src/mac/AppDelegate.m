/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import "AppDelegate.h"
#import "shared.h"


@implementation AppDelegate {

  NSThread* estimatePitchThread;
  NSTimer *timer;
  Note note;

}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

  [_prefController loadPreferences];

  estimatePitchThread = [[NSThread alloc] initWithTarget:self selector:@selector(estimatePitch) object:nil ];
  [estimatePitchThread start];


  // a timer works on the main thread
  timer = [NSTimer timerWithTimeInterval:0.05 target:self selector:@selector(setStrobes) userInfo:nil repeats:YES];
  // [timer setTolerance: 0.01];
  [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];


  Engine_setInputDevice(engine, engine->config->inputDevice, engine->config->samplerate);
  [_mainController.strobeView startDrawing];


}


// - (void)applicationDidChangeOcclusionState:(NSNotification *)notification {

//   if ([self occlusionState] != NSApplicationOcclusionStateVisible) {

//     // app is occluded,
//   }

// }


- (void)applicationWillTerminate:(NSNotification *)aNotification {

  [estimatePitchThread cancel];
  Engine_destroy(engine);

}


- (void)setStrobes {

  Engine_setStrobes(engine, note);

}


- (void)estimatePitch {

  // quasi-Nyquist
  float maxFreq = 0.45 * engine->config->samplerate;

  while ([estimatePitchThread isCancelled] == NO) {

    float pitch = Engine_estimatePitch(engine);


      // engine->config->transpose

    // sanity check
    if (pitch > 0.0 && pitch < maxFreq) {


      note = Tuning12TET_find(pitch, engine->config->pitchStandard, engine->config->centsOffset);

      float cents = Tuning12TET_freqToCents(pitch, engine->config->pitchStandard);

      Note transposed = Tuning12TET_transpose(note, engine->config->transpose);

      // printf("%.2f          \r", pitch); fflush(stdout);
      printf(
        "%4.2f %c%i%1s  %c%i%1s     %2.2f          \r",
        pitch, note.letter, note.octave, note.accidental,
        transposed.letter, transposed.octave, transposed.accidental,
        cents - note.cents
      );
      fflush(stdout);
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