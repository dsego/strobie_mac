/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */


#import "ModeButton.h"
#import "shared.h"


@implementation ModeButton

- (void)awakeFromNib {

  NSMutableParagraphStyle *centered = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
  centered.alignment = NSCenterTextAlignment;


  NSShadow *letterpress = [[NSShadow alloc] init];
  [letterpress setShadowBlurRadius:0];
  [letterpress setShadowOffset:NSMakeSize(0, 1)];
  [letterpress setShadowColor:[NSColor colorWithCalibratedWhite:0 alpha:1]];

  NSDictionary *attrs = @{
    NSFontAttributeName: [NSFont fontWithName: @"HelveticaNeue" size: 14],
    NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.4 green:0.4 blue:0.4 alpha:1],
    NSParagraphStyleAttributeName: centered,
    NSShadowAttributeName:  letterpress
  };

  NSShadow *shadow = [[NSShadow alloc] init];
  [shadow setShadowBlurRadius:8];
  [shadow setShadowOffset:NSMakeSize(0.0, 0.0)];
  [shadow setShadowColor:[NSColor colorWithCalibratedWhite:0.8 alpha:0.6]];

  NSDictionary *subAttrs = @{
    NSFontAttributeName: [NSFont fontWithName: @"HelveticaNeue" size: 14],
    NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:1],
    NSParagraphStyleAttributeName: centered,
    NSShadowAttributeName:  shadow
  };


  NSMutableAttributedString *titleString =
    [[NSMutableAttributedString alloc] initWithString: @"auto / manual"
      attributes: attrs];

  NSMutableAttributedString *alternateString =
    [[NSMutableAttributedString alloc] initWithString: @"auto / manual"
      attributes: attrs];

  [titleString setAttributes:subAttrs range:NSMakeRange(0,4)];
  [alternateString setAttributes:subAttrs range:NSMakeRange(7,6)];
  [self setAttributedTitle: titleString];
  [self setAttributedAlternateTitle: alternateString];

}


@end
