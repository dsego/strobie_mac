/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "NoteView.h"
#import "shared.h"


@implementation NoteView {

  CFAttributedStringRef noteNames[12];
  CFAttributedStringRef sharpSign;
  NSMenu *noteMenu;

}


- (id)initWithFrame:(NSRect)frameRect {

  self = [super initWithFrame: frameRect];
  [self buildStrings];
  [self buildMenu];
  return self;

}


- (void)buildStrings {

  CFStringRef keys1[] = {
    kCTFontAttributeName,
    kCTForegroundColorAttributeName
  };

  CFTypeRef values1[] = {
    CTFontCreateWithName(CFSTR("HelveticaNeue"), 72, NULL),
    CGColorCreateGenericRGB(0.95, 0.95, 0.95, 1),
  };

  CFDictionaryRef attrs1 = CFDictionaryCreate(
    kCFAllocatorDefault,
    (const void**)&keys1,
    (const void**)&values1,
    sizeof(keys1) / sizeof(keys1[0]),
    &kCFTypeDictionaryKeyCallBacks,
    &kCFTypeDictionaryValueCallBacks
  );

  noteNames[0] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("C"), attrs1);
  noteNames[1] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("C"), attrs1);
  noteNames[2] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("D"), attrs1);
  noteNames[3] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("D"), attrs1);
  noteNames[4] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("E"), attrs1);
  noteNames[5] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("F"), attrs1);
  noteNames[6] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("F"), attrs1);
  noteNames[7] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("G"), attrs1);
  noteNames[8] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("G"), attrs1);
  noteNames[9] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("A"), attrs1);
  noteNames[10] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("A"), attrs1);
  noteNames[11] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("B"), attrs1);

  CFRelease(attrs1);

  CFStringRef keys2[] = {
    kCTFontAttributeName,
    kCTForegroundColorAttributeName
  };

  CFTypeRef values2[] = {
    CTFontCreateWithName(CFSTR("HelveticaNeue"), 36, NULL),
    CGColorCreateGenericRGB(0.95, 0.95, 0.95, 1),
  };

  CFDictionaryRef attrs2 = CFDictionaryCreate(
    kCFAllocatorDefault,
    (const void**)&keys2,
    (const void**)&values2,
    sizeof(keys2) / sizeof(keys2[0]),
    &kCFTypeDictionaryKeyCallBacks,
    &kCFTypeDictionaryValueCallBacks
  );

  sharpSign = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("♯"), attrs2);

  CFRelease(attrs2);

}


- (void)buildMenu {

  noteMenu = [[NSMenu alloc] initWithTitle:@"Note Menu"];
  [noteMenu addItemWithTitle:@"C" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"C♯" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"D" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"D♯" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"E" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"F" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"F♯" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"G" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"G♯" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"A" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"A♯" action:@selector(menuAction:) keyEquivalent:@""];
  [noteMenu addItemWithTitle:@"B" action:@selector(menuAction:) keyEquivalent:@""];


  int tag = 0;

  for (NSMenuItem *item in [noteMenu itemArray]) {
    item.target = self;
    item.tag = tag;
    ++tag;
  }

  [noteMenu setAutoenablesItems:NO];
  [noteMenu setMinimumWidth:100];

}


-(void)menuAction:(id)sender  {

  int noteIndex = [sender tag];
  Note note = Tuning12TET_noteFromIndex(noteIndex, 0, engine->config->pitchStandard, engine->config->centsOffset);
  Engine_setStrobes(engine, note);
  [self setNeedsDisplay: YES];

}


- (BOOL)mouseDownCanMoveWindow {

  return NO;

}


- (void)drawRect:(NSRect)rect {

  CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];

  Note note;

  if (engine->config->transpose == 0) {
    note = engine->currentNote;
  }
  else {
    note = Tuning12TET_transpose(engine->currentNote, engine->config->transpose);
  }

  // note name
  CTLineRef line = CTLineCreateWithAttributedString(noteNames[note.index]);
  float width = CTLineGetTypographicBounds(line, NULL, NULL, NULL);

  // center horizontally
  float x = (rect.size.width - width) * 0.5;

  CGContextSetTextPosition(context, x, 14);
  CTLineDraw(line, context);
  CFRelease(line);

  // draw sharp symbol
  if (note.isSharp) {
    line = CTLineCreateWithAttributedString(sharpSign);
    CGContextSetTextPosition(context, rect.size.width * 0.5 + 25, 45);
    CTLineDraw(line, context);
    CFRelease(line);
  }

}


- (void)mouseDown:(NSEvent *)theEvent {

  if (engine->mode == MANUAL) {

    NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    [noteMenu popUpMenuPositioningItem: [noteMenu itemWithTag: engine->currentNote.index]
      atLocation:NSMakePoint(0, location.y + 10) // dirty hack
      inView:self
    ];

  }

}



@end
