/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "NoteView.h"
#import "shared.h"


@implementation NoteView {

  NSTextStorage *textStorage;
  NSLayoutManager *layoutManager;
  NSTextContainer *textContainer;

}


- (id)initWithFrame:(NSRect)frameRect {

  self = [super initWithFrame: frameRect];
  textStorage = [[NSTextStorage alloc] initWithString:@"      "];
  [textStorage setFont: [NSFont fontWithName:@"Menlo" size: 72]];

  layoutManager = [[NSLayoutManager alloc] init];
  textContainer = [[NSTextContainer alloc] init];
  [layoutManager addTextContainer:textContainer];
  [textStorage addLayoutManager:layoutManager];

  return self;

}


- (void)drawRect: (NSRect)rect {

  NSString *str = [NSString stringWithFormat: @"%c%u%C", engine->currentNote.letter, engine->currentNote.octave, engine->currentNote.accidental];
  [textStorage replaceCharactersInRange: NSMakeRange(0, [str length]) withString: str];
  NSRange glyphRange = [layoutManager glyphRangeForTextContainer: textContainer];
  [layoutManager drawGlyphsForGlyphRange: glyphRange atPoint: (NSPoint) {0, -52}];

}


@end
