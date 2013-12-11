/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "ModeButton.h"
#import "shared.h"


@implementation ModeButton

- (void)awakeFromNib {

  NSMutableParagraphStyle *centered = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
  centered.alignment = NSCenterTextAlignment;

  NSDictionary *attrs = @{
    NSFontAttributeName: [NSFont fontWithName: @"HelveticaNeue" size: 14],
    NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.4 green:0.4 blue:0.4 alpha:1.0],
    NSParagraphStyleAttributeName: centered
  };

  NSShadow *shadow = [[NSShadow alloc] init];
  [shadow setShadowBlurRadius:6.0];
  [shadow setShadowOffset:NSMakeSize(0.0, 0.0)];
  [shadow setShadowColor:[NSColor colorWithCalibratedWhite:0.8 alpha:0.8]];

  NSDictionary *subAttrs = @{
    NSFontAttributeName: [NSFont fontWithName: @"HelveticaNeue" size: 14],
    NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:1.0],
    NSParagraphStyleAttributeName: centered,
    NSShadowAttributeName:  shadow
  };


  NSMutableAttributedString *titleString = [[NSMutableAttributedString alloc] initWithString: @"auto / manual" attributes: attrs];
  NSMutableAttributedString *alternateString = [[NSMutableAttributedString alloc] initWithString: @"auto / manual" attributes: attrs];
  [titleString setAttributes:subAttrs range:NSMakeRange(0,4)];
  [alternateString setAttributes:subAttrs range:NSMakeRange(7,6)];
  [self setAttributedTitle: titleString];
  [self setAttributedAlternateTitle: alternateString];

}


@end
