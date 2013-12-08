/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "ModeButton.h"
#import "shared.h"


@implementation ModeButton

- (void)awakeFromNib {

  NSMutableParagraphStyle *centered = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
  centered.alignment = NSRightTextAlignment;

  NSDictionary *attrs = @{
    NSFontAttributeName: [NSFont fontWithName: @"HelveticaNeue" size: 14],
    NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.4 green:0.4 blue:0.4 alpha:1.0],
    NSParagraphStyleAttributeName: centered
  };
  NSAttributedString *titleString = [[NSAttributedString alloc] initWithString: @"auto" attributes: attrs];
  NSAttributedString *alternateString = [[NSAttributedString alloc] initWithString: @"manual" attributes: attrs];
  [self setAttributedTitle: titleString];
  [self setAttributedAlternateTitle: alternateString];

}


@end
