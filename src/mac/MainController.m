/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import "MainController.h"
#import "shared.h"


@implementation MainController


- (void)awakeFromNib {

  NSColor *bg = [NSColor colorWithDeviceRed:35.0/255.0 green:35.0/255.0 blue:35.0/255.0 alpha:1.0];
  [self.window setBackgroundColor: bg];

}


- (void)windowWillClose:(NSNotification *)notification {

  [NSApp terminate: self];

}


- (void)keyDown:(NSEvent *)theEvent {
  // NSLog(@"%u", [theEvent keyCode]);

  switch ([theEvent keyCode]) {

    case 123: // LEFT  (semitone lower)
      if (engine->mode == MANUAL) {
        Note newNote = Tuning12TET_centsToNote(engine->currentNote.cents - 100, engine->config->pitchStandard, engine->config->centsOffset);
        Engine_setStrobes(engine, newNote);
      }
      break;

    case 124: // RIGHT   (semitone higher)
      if (engine->mode == MANUAL) {
        Note newNote = Tuning12TET_centsToNote(engine->currentNote.cents + 100, engine->config->pitchStandard, engine->config->centsOffset);
        Engine_setStrobes(engine, newNote);
      }
      break;

    case 125: // DOWN   (octave lower)
      if (engine->mode == MANUAL) {
        Note newNote = Tuning12TET_centsToNote(engine->currentNote.cents - 1200, engine->config->pitchStandard, engine->config->centsOffset);
        Engine_setStrobes(engine, newNote);
      }
      break;

    case 126: // UP   (octave higher)
      if (engine->mode == MANUAL) {
        Note newNote = Tuning12TET_centsToNote(engine->currentNote.cents + 1200, engine->config->pitchStandard, engine->config->centsOffset);
        Engine_setStrobes(engine, newNote);
      }
      break;

    case 49:  // SPACEBAR
      engine->mode = (engine->mode == AUTO) ? MANUAL : AUTO;
      break;

    default: // do nothing
      break;
  }

}


@end