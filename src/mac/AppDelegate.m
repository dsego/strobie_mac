// Copyright (c) Davorin Šego. All rights reserved.


#import "AppDelegate.h"
#import "shared.h"
#import "misc.h"
#import "version.h"
#import "trial.h"

#define PITCH_ESTIMATE_INTERVAL 0.06


@implementation AppDelegate {

  NSThread* estimatePitchThread;
  NSUserDefaults *defaults;

}


- (void)savePreferences {

  defaults = [NSUserDefaults standardUserDefaults];

  const DeviceInfo *info = Engine_deviceInfo(engine->config->inputDevice);
  if (info != NULL) {
    NSString *inputDeviceName  = @(info->name);
    [defaults setObject: inputDeviceName forKey: @"inputDeviceName"];
  }

  [defaults setInteger: engine->config->samplerate forKey: @"inputSamplerate"];
  [defaults setInteger: engine->config->inputBufferSize forKey: @"inputBufferSize"];
  [defaults setInteger: engine->config->transpose forKey: @"transpose"];
  [defaults setFloat: engine->config->pitchStandard forKey: @"pitchStandard"];
  [defaults setFloat: engine->config->centsOffset forKey: @"centsOffset"];
  [defaults setFloat: engine->config->freq forKey: @"freq"];
  [defaults setFloat: engine->config->gain forKey: @"gain"];
  [defaults setInteger: engine->config->schemeIndex forKey: @"schemeIndex"];


  ColorScheme custom = engine->config->schemes[engine->config->schemeCount-1];

  [defaults setObject: [NSString stringWithFormat:
    @"%f %f %f",
    custom.a[0], custom.a[1], custom.a[2]]
    forKey: @"customColorA"];

  [defaults setObject: [NSString stringWithFormat:
    @"%f %f %f",
    custom.b[0], custom.b[1], custom.b[2]]
    forKey: @"customColorB"];

  [defaults synchronize];

}


- (void)loadPreferences {

  defaults = [NSUserDefaults standardUserDefaults];
  ColorScheme custom = engine->config->schemes[engine->config->schemeCount-1];

  // register factory defaults
  NSDictionary *factoryValues = @{
    @"inputDeviceName": @"",
    @"inputSamplerate": @(engine->config->samplerate),
    @"inputBufferSize": @(engine->config->inputBufferSize),
    @"transpose": @(engine->config->transpose),
    @"pitchStandard": @(engine->config->pitchStandard),
    @"centsOffset": @(engine->config->centsOffset),
    @"freq": @(engine->config->freq),
    @"gain": @(engine->config->gain),
    @"schemeIndex": @(engine->config->schemeIndex),
    @"customColorA": [NSString stringWithFormat: @"%f %f %f", custom.a[0], custom.a[1], custom.a[2]],
    @"customColorB": [NSString stringWithFormat: @"%f %f %f", custom.b[0], custom.b[1], custom.b[2]]
  };

  [defaults registerDefaults:factoryValues];

  // read saved values

  NSString *inputDeviceName = [defaults stringForKey: @"inputDeviceName"];

  int count = Engine_deviceCount();
  for (int i = 0; i < count; ++i) {
    const DeviceInfo *info = Engine_deviceInfo(i);
    if ([inputDeviceName isEqualToString: @(info->name)]) {
      engine->config->inputDevice = i;
      break;
    }
  }

  engine->config->samplerate      = [defaults integerForKey: @"inputSamplerate"];
  engine->config->inputBufferSize = [defaults integerForKey: @"inputBufferSize"];
  engine->config->transpose       = [defaults integerForKey: @"transpose"];
  engine->config->pitchStandard   = [defaults floatForKey: @"pitchStandard"];
  engine->config->centsOffset     = [defaults floatForKey: @"centsOffset"];
  engine->config->freq            = [defaults floatForKey: @"freq"];
  engine->config->gain            = [defaults floatForKey: @"gain"];
  engine->config->schemeIndex     = [defaults floatForKey: @"schemeIndex"];

  const char *customColorA = [[defaults stringForKey: @"customColorA"] UTF8String];
  sscanf(customColorA, "%f %f %f", custom.a, custom.a+1, custom.a+2);

  const char* customColorB = [[defaults stringForKey: @"customColorB"] UTF8String];
  sscanf(customColorB, "%f %f %f", custom.b, custom.b+1, custom.b+2);

  [_prefController loadFromConfig];

}


- (void)awakeFromNib {

  [self loadPreferences];

  #ifdef TRIAL_VERSION
  [self checkTrial];
  #endif

  Note note = EqualTemp_findNearest(
    engine->config->freq,
    engine->config->pitchStandard,
    engine->config->centsOffset
  );

  Engine_setStrobes(engine, note, engine->config->samplerate);

  int err = Engine_setInputDevice(
    engine,
    engine->config->inputDevice,
    engine->config->samplerate,
    engine->config->inputBufferSize
  );

  if (err) {
    audioDeviceErrorAlert();
  }

  estimatePitchThread = [[NSThread alloc] initWithTarget:self
    selector:@selector(estimatePitch)
    object:nil];

  [estimatePitchThread start];

}

// - (void)applicationDidChangeOcclusionState:(NSNotification *)notification {
//   if ([(NSApplication *) NSApp occlusionState] & NSApplicationOcclusionStateVisible) {
//     // Visible
//   } else {
//     // Occluded
//   }
// }


- (void)applicationWillTerminate:(NSNotification *)aNotification {

  [estimatePitchThread cancel];
  [self savePreferences];

}


- (void)estimatePitch {

  while ([estimatePitchThread isCancelled] == NO) {

    if (engine->mode == AUTO) {
      Engine_estimatePitch(engine);
      dispatch_async(dispatch_get_main_queue(),^ {
        [[NSNotificationCenter defaultCenter]
          postNotificationName:@"NOTE_CHANGE_NOTIFICATION"
          object:self];
      });
    }
    [NSThread sleepForTimeInterval:PITCH_ESTIMATE_INTERVAL];

  }

}


// - (void)applicationWillUnhide:(NSNotification *)aNotification {
//   // engine start ?
//   // PaError Pa_StartStream  ( PaStream *  stream  )
//   //
// }


// - (void)applicationWillHide:(NSNotification *)aNotification {
//   // engine stop ?
//   // PaError Pa_StopStream  ( PaStream *  stream  )
// }


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {

  return NO;

}


-(IBAction)showStrobieWebsite:(id)sender {

  openWebsiteInBrowser(@"http://strobie-app.com");

}


-(IBAction)showHelp:(id)sender {

  // [self openWebsite: @"http://www.strobie-app.com/help"];

}


-(void)checkTrial {

  int then = [defaults integerForKey: @"then"];

  if (then == 0) {
    [defaults setInteger: (int)time(NULL) forKey: @"then"];
    [defaults synchronize];
    return;
  }

  int now = genTimestamp();
  int daysLeft = checkTrialExpiry(then, now, 15);
  NSAlert *reminder;

  if (daysLeft > 0) {
    NSString *msgDaysLeft = (daysLeft <= 1)
      ? @"This is the last day of your trial."
      : [NSString stringWithFormat: @"%d days left!", daysLeft];

    reminder = [NSAlert
      alertWithMessageText: msgDaysLeft
      defaultButton: @"Continue"
      alternateButton: @"Purchase"
      otherButton: nil
      informativeTextWithFormat: @"Thank you for trying out Strobie."
    ];
  }
  else {
    reminder = [NSAlert
      alertWithMessageText: @"Your trial has ended."
      defaultButton: @"Close"
      alternateButton: @"Purchase"
      otherButton: nil
      informativeTextWithFormat: @"If you would like to continue using Strobie, please purchase the full version."
    ];
  }

  int button = [reminder runModal];

  // Close
  if (button == NSAlertDefaultReturn && daysLeft <= 0) {
    [NSApp terminate: self];
  }

  // Purchase
  else if (button == NSAlertAlternateReturn) {
    [self showStrobieWebsite: nil];
    [NSApp terminate: self];
  }

}


@end
