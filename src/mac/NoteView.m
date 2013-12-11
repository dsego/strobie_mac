/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "NoteView.h"
#import "shared.h"


@implementation NoteView {

  CGImageRef noteSprites[12];
  NSMenu *noteMenu;

}


- (void)awakeFromNib {

  [self buildStringImages];
  [self buildMenu];

}


- (void)buildStringImages {

  CFAttributedStringRef noteNames[12];

  CFStringRef keys[] = {
    kCTFontAttributeName,
    kCTForegroundColorAttributeName
  };

  CFTypeRef values[] = {
    CTFontCreateWithName(CFSTR("HelveticaNeue-Medium"), 72, NULL),
    CGColorCreateGenericRGB(1, 1, 1, 1)
  };

  CFDictionaryRef attrs = CFDictionaryCreate(
    kCFAllocatorDefault,
    (const void**)&keys,
    (const void**)&values,
    sizeof(keys) / sizeof(keys[0]),
    &kCFTypeDictionaryKeyCallBacks,
    &kCFTypeDictionaryValueCallBacks
  );

  noteNames[0] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("C"), attrs);
  noteNames[1] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("C♯"), attrs);
  noteNames[2] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("D"), attrs);
  noteNames[3] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("D♯"), attrs);
  noteNames[4] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("E"), attrs);
  noteNames[5] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("F"), attrs);
  noteNames[6] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("F♯"), attrs);
  noteNames[7] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("G"), attrs);
  noteNames[8] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("G♯"), attrs);
  noteNames[9] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("A"), attrs);
  noteNames[10] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("A♯"), attrs);
  noteNames[11] = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("B"), attrs);

  CFRelease(attrs);
  CFRelease(values[0]);
  CFRelease(values[1]);


  int width  = _bounds.size.width;
  int height = _bounds.size.height;

  for (int i = 0; i < 12; ++i) {

    CGColorSpaceRef genericRGB = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    CGContextRef context = CGBitmapContextCreate(NULL, width, height, 8, 0, genericRGB, kCGImageAlphaPremultipliedLast);

    CTLineRef line = CTLineCreateWithAttributedString(noteNames[i]);
    float typoWidth = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
    float x = (width - typoWidth) * 0.5; // center horizontally
    CGImageRef mask = maskFromCTLine(line, x, 14, width, height);
    CGImageRef inverted = invertedMaskFromMask(mask);

    CGContextSetRGBFillColor(context, 0.9, 0.9, 0.9, 1);

    CGColorRef shadowColor = CGColorCreateGenericRGB(0, 0, 0, 0.6);
    CGContextSetShadowWithColor(context, CGSizeMake(0, -2), 1, shadowColor);
    CGContextClipToMask(context, _bounds, mask);
    CGContextFillRect(context, _bounds);
    CGContextDrawImage(context, _bounds, inverted);

    noteSprites[i] = CGBitmapContextCreateImage(context);

    CGColorRelease(shadowColor);
    CGContextRelease(context);
    CGImageRelease(mask);
    CGImageRelease(inverted);
    CFRelease(noteNames[i]);

  }


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
  Engine_setStrobes(engine, note, engine->config->samplerate);
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

  CGContextDrawImage(context, rect, noteSprites[note.index]);

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


- (void) dealloc {

  for (int i = 0; i < 12; ++i) {
    CFRelease(noteSprites[i]);
  }

}



static CGImageRef maskFromCTLine(CTLineRef line, float x, float y, int w, int h) {

  CGColorSpaceRef grayscale = CGColorSpaceCreateDeviceGray();
  CGContextRef context = CGBitmapContextCreate(NULL, w, h, 8, 0, grayscale, kCGImageAlphaOnly);

  CGContextSetTextPosition(context, x, y);
  CTLineDraw(line, context);
  CGImageRef mask = CGBitmapContextCreateImage(context);

  CGColorSpaceRelease(grayscale);
  CFRelease(line);
  CGContextRelease(context);

  return mask;

}


static CGImageRef invertedMaskFromMask(CGImageRef mask) {

  int width = CGImageGetWidth(mask);
  int height = CGImageGetHeight(mask);

  CGColorSpaceRef genericRGB = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
  CGContextRef context = CGBitmapContextCreate(NULL, width, height, 8, 0, genericRGB, kCGImageAlphaPremultipliedLast);

  CGRect rect = CGRectMake(0, 0, width, height);
  CGContextSetRGBFillColor(context, 0, 0, 0, 1);
  CGContextFillRect(context, rect);
  CGContextClipToMask(context, rect, mask);
  CGContextClearRect(context, rect);
  CGImageRef inverted = CGBitmapContextCreateImage(context);

  CGContextRelease(context);
  CGColorSpaceRelease(genericRGB);

  return inverted;

}


@end
