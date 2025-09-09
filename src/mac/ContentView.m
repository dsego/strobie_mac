// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.


#import "ContentView.h"

@implementation ContentView {

  NSColor *startColor;
  NSColor *endColor;

}

- (void)awakeFromNib {

  startColor = [NSColor colorWithCalibratedRed:21.0/255.0 green:21.0/255.0 blue:21.0/255.0 alpha:1.0];
  endColor =[NSColor colorWithCalibratedRed:27.0/255.0 green:27.0/255.0 blue:27.0/255.0 alpha:1.0];

}


- (void)drawRect:(NSRect)dirtyRect {

  NSGradient *gradient = [[NSGradient alloc] initWithColorsAndLocations:
    startColor, 0.0,
    endColor, 100.0 / self.frame.size.height, nil];

  [gradient drawInRect:self.frame angle:90];

}

@end
