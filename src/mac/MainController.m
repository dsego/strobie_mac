// Copyright (c) Davorin Šego. All rights reserved.

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "shared.h"
#import "misc.h"


@implementation MainController {
}


-(void)awakeFromNib {

  // Register to be notified when mode changes
  [[NSNotificationCenter defaultCenter]
    addObserver:self
    selector:@selector(manualMode)
    name:@"MANUAL_MODE"
    object:nil
  ];

  // Register to be notified when the note changed so we can update the UI
  [[NSNotificationCenter defaultCenter]
    addObserver:self
    selector:@selector(refreshDisplay)
    name:@"NOTE_CHANGE_NOTIFICATION"
    object:nil
  ];

}


-(void)windowWillClose:(NSNotification *)notification {
  [[NSNotificationCenter defaultCenter] removeObserver:self];
  [NSApp terminate: self];
}


// prevent "beeps"
-(void)keyDown:(NSEvent *)theEvent {}


-(void)keyUp:(NSEvent *)theEvent {

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


-(IBAction)modeButtonOnClick: (id)sender {
  engine->mode = (engine->mode == AUTO) ? MANUAL : AUTO;
}


-(void)manualMode {

  if (engine->mode == AUTO) {
    engine->mode = MANUAL;
    [self modeButton].state = NSOnState;
  }

}


-(void)refreshDisplay {
  [_noteView setNeedsDisplay: YES];
}


-(IBAction)openStrobieWebChangelog:(id)sender {
  openWebsiteInBrowser(@"http://strobie-app.com");
}


@end
