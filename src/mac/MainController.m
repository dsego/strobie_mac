/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "shared.h"


@implementation MainController


- (void)awakeFromNib {

  NSColor *bg = [NSColor colorWithDeviceRed:36.0/255.0 green:36.0/255.0 blue:36.0/255.0 alpha:1.0];
  [self.window setBackgroundColor: bg];
  // Register to be notified when the note changed so we can update the UI
  [[NSNotificationCenter defaultCenter]
    addObserver:self
    selector:@selector(refreshDisplay)
    name:@"NoteChangeNotification"
    object:nil
  ];

}


- (void)windowWillClose:(NSNotification *)notification {

  [[NSNotificationCenter defaultCenter] removeObserver:self];
  [NSApp terminate: self];

}


- (void)keyDown:(NSEvent *)theEvent {
}


- (void)keyUp:(NSEvent *)theEvent {
  // NSLog(@"%u", [theEvent keyCode]);

  switch ([theEvent keyCode]) {

    case 123: // LEFT  semitone lower
      if (engine->mode == MANUAL) {
        engine->currentNote = Tuning12TET_moveBySemitones(engine->currentNote, -1);
        Engine_setStrobes(engine, engine->currentNote);
      }
      break;

    case 124: // RIGHT  semitone higher
      if (engine->mode == MANUAL) {
        engine->currentNote = Tuning12TET_moveBySemitones(engine->currentNote, 1);
        Engine_setStrobes(engine, engine->currentNote);
      }
      break;

    case 125: // DOWN   fifth lower (fourth higher)
      if (engine->mode == MANUAL) {
        engine->currentNote = Tuning12TET_moveBySemitones(engine->currentNote, -7);
        Engine_setStrobes(engine, engine->currentNote);
      }
      break;

    case 126: // UP   fifth higher (fourth lower)
      if (engine->mode == MANUAL) {
        engine->currentNote = Tuning12TET_moveBySemitones(engine->currentNote, 7);
        Engine_setStrobes(engine, engine->currentNote);
      }
      break;

    case 49:  // SPACEBAR
      engine->mode = (engine->mode == AUTO) ? MANUAL : AUTO;
      break;

    default: // do nothing
      break;
  }
  [self refreshDisplay];

}


- (void)refreshDisplay {

  [_noteView setNeedsDisplay: YES];

}


@end
