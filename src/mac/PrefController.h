// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.

#import <AppKit/NSWindowController.h>


@interface PrefController : NSWindowController

  @property IBOutlet NSPopUpButton *inputDevicePopup;
  @property IBOutlet NSPopUpButton *sampleratePopup;
  @property IBOutlet NSPopUpButton *bufferSizePopup;
  @property IBOutlet NSPopUpButton *transposePopup;
  @property IBOutlet NSPopUpButton *colorSchemePopup;
  @property IBOutlet NSTextField *concertPitchText;
  @property IBOutlet NSStepper *concertPitchStepper;
  @property IBOutlet NSTextField *centsOffsetText;
  @property IBOutlet NSStepper *centsOffsetStepper;
  @property IBOutlet NSSlider *gainSlider;
  @property IBOutlet NSTextField *gainLabel;
  @property IBOutlet NSButton *highlightCheckbox;
  @property IBOutlet NSColorWell *colorWellA;
  @property IBOutlet NSColorWell *colorWellB;

  - (void)loadFromConfig;

  - (IBAction)inputDeviceChanged: (id)sender;
  - (IBAction)transposeChanged: (id)sender;
  - (IBAction)concertPitchChanged: (id)sender;
  - (IBAction)centsOffsetChanged: (id)sender;
  - (IBAction)gainChanged: (id)sender;
  - (IBAction)highlightChanged: (id)sender;
  - (IBAction)colorSchemeChanged: (id)sender;
  - (IBAction)colorWellChanged: (id)sender;

@end
