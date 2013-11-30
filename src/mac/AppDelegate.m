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
  [defaults setInteger: engine->config->inputBufferSize forKey: @"inputBufferSize"];
  [defaults setInteger: engine->config->transpose forKey: @"transpose"];
  [defaults setFloat: engine->config->pitchStandard forKey: @"pitchStandard"];
  [defaults setFloat: engine->config->centsOffset forKey: @"centsOffset"];
  [defaults setFloat: engine->config->freq forKey: @"freq"];
  [defaults setFloat: engine->config->gain forKey: @"gain"];
  [defaults synchronize];

}


- (void)loadPreferences {

  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

  // register factory defaults
  NSDictionary *factoryValues = @{
    @"inputDevice": @(engine->config->inputDevice),
    @"inputBufferSize": @(engine->config->inputBufferSize),
    @"transpose": @(engine->config->transpose),
    @"pitchStandard": @(engine->config->pitchStandard),
    @"centsOffset": @(engine->config->centsOffset),
    @"freq": @(engine->config->freq),
    @"gain": @(engine->config->gain)
  };

  [defaults registerDefaults:factoryValues];

  // read saved values
  engine->config->inputDevice     = [defaults integerForKey: @"inputDevice"];
  engine->config->inputBufferSize = [defaults integerForKey: @"inputBufferSize"];
  engine->config->transpose       = [defaults integerForKey: @"transpose"];
  engine->config->pitchStandard   = [defaults floatForKey: @"pitchStandard"];
  engine->config->centsOffset     = [defaults floatForKey: @"centsOffset"];
  engine->config->freq            = [defaults floatForKey: @"freq"];
  engine->config->gain            = [defaults floatForKey: @"gain"];

  [_prefController loadFromConfig];

}


- (void)awakeFromNib {

  [self loadPreferences];

  Note note = Tuning12TET_find(engine->config->freq, engine->config->pitchStandard, engine->config->centsOffset);
  Engine_setStrobes(engine, note);
  Engine_setInputDevice(engine, engine->config->inputDevice, engine->config->samplerate, engine->config->inputBufferSize);

}


- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {

  estimatePitchThread = [[NSThread alloc] initWithTarget:self selector:@selector(estimatePitch) object:nil ];
  [estimatePitchThread start];

}


- (void)applicationDidChangeOcclusionState:(NSNotification *)notification {

  if ([(NSApplication *) NSApp occlusionState] & NSApplicationOcclusionStateVisible) {
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


- (void)estimatePitch {

  while ([estimatePitchThread isCancelled] == NO) {

    Engine_estimatePitch(engine);
    if (engine->mode == AUTO) {
      dispatch_async(dispatch_get_main_queue(),^ {
        [[NSNotificationCenter defaultCenter] postNotificationName:@"NoteChangeNotification" object:self];
      });
    }

    [NSThread sleepForTimeInterval:0.04];

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
