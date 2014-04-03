// Copyright (c) Davorin Šego. All rights reserved.

#import <Cocoa/Cocoa.h>
#import "NSWindow+AccessoryView.h"
#import "MainController.h"
#import "shared.h"
#import "misc.h"


@implementation MainController {

  NSButton *newVersionButton;

}


- (void)awakeFromNib {

  // Register to be notified when the note changed so we can update the UI
  [[NSNotificationCenter defaultCenter]
    addObserver:self
    selector:@selector(refreshDisplay)
    name:@"NoteChangeNotification"
    object:nil
  ];


  // Register to be notified that a new version is available
  [[NSNotificationCenter defaultCenter]
    addObserver:self
    selector:@selector(showNewVersionButton)
    name:@"NewVersionAvailable"
    object:nil
  ];

}


- (void)windowWillClose:(NSNotification *)notification {

  [[NSNotificationCenter defaultCenter] removeObserver:self];
  [NSApp terminate: self];

}


// prevents "beeps"
- (void)keyDown:(NSEvent *)theEvent {}


- (void)keyUp:(NSEvent *)theEvent {
  // NSLog(@"%u", [theEvent keyCode]);

  switch ([theEvent keyCode]) {

    case 123: // LEFT  semitone lower
      engine->mode = MANUAL; // automatically go to manual mode
      engine->currentNote = EqualTemp_moveBySemitones(engine->currentNote, -1);
      Engine_setStrobes(engine, engine->currentNote, engine->config->samplerate);
      break;

    case 124: // RIGHT  semitone higher
      engine->mode = MANUAL; // automatically go to manual mode
      engine->currentNote = EqualTemp_moveBySemitones(engine->currentNote, 1);
      Engine_setStrobes(engine, engine->currentNote, engine->config->samplerate);
      break;

    case 125: // DOWN   fourth lower (fifth higher)
      engine->mode = MANUAL; // automatically go to manual mode
      engine->currentNote = EqualTemp_moveBySemitones(engine->currentNote, 7);
      Engine_setStrobes(engine, engine->currentNote, engine->config->samplerate);
      break;

    case 126: // UP   fourth higher (fifth lower)
      engine->mode = MANUAL; // automatically go to manual mode
      engine->currentNote = EqualTemp_moveBySemitones(engine->currentNote, -7);
      Engine_setStrobes(engine, engine->currentNote, engine->config->samplerate);
      break;

    case 49:  // SPACEBAR -> toggle mode
      // mode button key equivalent is set to space bar
      break;

    default: // do nothing
      break;
  }

  // refresh mode button
  _modeButton.state = (engine->mode == AUTO) ? NSOffState : NSOnState;

  [self refreshDisplay];

}

- (IBAction)modeButtonOnClick: (id)sender {

  engine->mode = (engine->mode == AUTO) ? MANUAL : AUTO;

}


- (void)refreshDisplay {

  [_noteView setNeedsDisplay: YES];

}


- (void)showNewVersionButton {

  newVersionButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 107, 19)];
  [newVersionButton setButtonType:NSMomentaryChangeButton];

  NSShadow *shadow = [[NSShadow alloc] init];
  shadow.shadowBlurRadius = 1;
  shadow.shadowColor = [NSColor colorWithCalibratedRed:0 green:0 blue:0 alpha:0.8];
  shadow.shadowOffset = NSMakeSize(1, -1);

  NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc] init];
  [paragraphStyle setAlignment:NSCenterTextAlignment];
  NSAttributedString * title = [[NSAttributedString alloc] initWithString:@"Get New Version!"
    attributes: @{
      NSFontAttributeName: [NSFont fontWithName:@"Lucida Grande" size:11],
      NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:1 green:1 blue:1 alpha:1],
      NSShadowAttributeName: shadow,
      NSParagraphStyleAttributeName: paragraphStyle
  }];
  [newVersionButton.cell setAttributedTitle: title];
  [newVersionButton setImage: [NSImage imageNamed: @"new-version.png"]];
  [newVersionButton setImagePosition: NSImageOverlaps];
  [newVersionButton setBordered: NO];

  [self.window addViewToTitleBar:newVersionButton
    atXPosition:self.window.frame.size.width - newVersionButton.frame.size.width - 24];

  [newVersionButton setTarget:self.window];
  [newVersionButton setAction:@selector(showStrobieWebsite:)];

}

- (void)windowWillEnterFullScreen:(NSNotification *)notification {

  [newVersionButton setHidden: YES];

}

- (void)windowWillExitFullScreen:(NSNotification *)notification {

  [newVersionButton setHidden: NO];

}

-(IBAction)showStrobieWebsite:(id)sender {

  openWebsiteInBrowser(@"http://strobie-app.com");

}


@end
