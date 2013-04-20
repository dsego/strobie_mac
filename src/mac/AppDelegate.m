//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#import "AppDelegate.h"
#import "../engine/engine.h"

@implementation AppDelegate

Engine *engine;
Note note;


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  // manually allocate and initialize the window
  NSRect windowRect = NSMakeRect(0, 0, 500, 500);
  NSUInteger styleMask = (NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask);
  self.window = [[NSWindow alloc] initWithContentRect:windowRect
                                  styleMask:styleMask
                                  backing:NSBackingStoreBuffered
                                  defer:NO];

  [self.window setTitle:@"Strobie"];
  [self.window center];
  [self.window makeKeyAndOrderFront:self];


  engine = Engine_create();
  Engine_init_audio(engine);

  // window controller
  // self.windowController = [[NSWindowController alloc] initWithWindow:self.window];

  // [self.windowController showWindow:self];


//    NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:0.02
//                                                      target:self
//                                                    selector:@selector(refreshNote)
//                                                    userInfo:nil
//                                                     repeats:YES];


  // NSThread* thread = [[NSThread alloc] initWithTarget:self
  //                                            selector:@selector(estimatePitch)
  //                                              object:nil];
  // [thread start];
//    [timer fire];
}

- (void)estimatePitch
{
  double pitch;
  while (true) {
    pitch = Engine_estimate_pitch(engine);
    note  = Tuning12TET_find(pitch, 440.0, 0.0, 0);
//        NSLog([NSString stringWithFormat:@"%f", pitch]);
//        [self.label setStringValue:[NSString stringWithFormat:@"%f", pitch]];
    // [self.label setStringValue:[NSString stringWithFormat:@"%c%d", note.letter, note.octave]];
    usleep(50000);
  }
}

-(void)refreshStrobe
{

}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
  return YES;
}

-(IBAction)openOnlineDocumentation:(id)sender
{
  id url = [NSURL URLWithString:@"http://www.google.com"];
  [[NSWorkspace sharedWorkspace] openURL:url];
}
@end
