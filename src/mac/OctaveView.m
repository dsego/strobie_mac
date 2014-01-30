/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "OctaveView.h"
#import "shared.h"


@implementation OctaveView

- (void)drawRect:(NSRect)rect {

  CFStringRef keys[] = {
    kCTFontAttributeName,
    kCTForegroundColorAttributeName
  };

  CFTypeRef values[] = {
    CTFontCreateWithName(CFSTR("HelveticaNeue"), 14, NULL),
    CGColorCreateGenericRGB(0.4, 0.4, 0.4, 1)
  };

  CFDictionaryRef attrs = CFDictionaryCreate(
    kCFAllocatorDefault,
    (const void**)&keys,
    (const void**)&values,
    sizeof(keys) / sizeof(keys[0]),
    &kCFTypeDictionaryKeyCallBacks,
    &kCFTypeDictionaryValueCallBacks
  );

  CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
  CGContextSetTextMatrix(context, CGAffineTransformIdentity);

  CGColorRef shadowColor = CGColorCreateGenericRGB(0, 0, 0, 1);
  CGContextSetShadowWithColor(context, CGSizeMake(0, 2), 0, shadowColor);

  float height = rect.size.height / engine->config->strobeCount;
  float y = height * 0.5;

  for (int i = 0; i < engine->config->strobeCount; ++i) {

    CFStringRef formatStr = CFStringCreateWithFormat(
      kCFAllocatorDefault, NULL, CFSTR("%1.0f"),
      engine->config->strobes[i].value
    );

    CFAttributedStringRef str = CFAttributedStringCreate(
      kCFAllocatorDefault,
      formatStr,
      attrs
    );

    CTLineRef line = CTLineCreateWithAttributedString(str);
    CGRect bounds = CTLineGetImageBounds(line, context);
    float width = CTLineGetTypographicBounds(line, NULL, NULL, NULL);

    CGContextSetTextPosition(
      context,
      (rect.size.width - width) * 0.5,
      y - bounds.size.height * 0.5
    );

    CTLineDraw(line, context);
    y += height;

    CFRelease(line);
    CFRelease(str);
    CFRelease(formatStr);

  }

  CFRelease(shadowColor);
  CFRelease(attrs);
  CFRelease(values[0]);
  CFRelease(values[1]);

}

@end
