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

    double pitch = Engine_estimatePitch(_engine);
    [_mainController refreshPitch:pitch];

    double standard = _engine->config->pitchStandard;
    double offset = _engine->config->centsOffset;
    int transpose = _engine->config->transpose;
    Note note = Tuning12TET_find(pitch, standard, offset, transpose);

    Engine_setStrobeFreq(_engine, note.frequency);
// [self.label setStringValue:[NSString stringWithFormat:@"%f", pitch]];
    // [self.label setStringValue:[NSString stringWithFormat:@"%c%d", note.letter, note.octave]];
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