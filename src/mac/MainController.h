// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.

#import <AppKit/NSWindowController.h>
#import "StrobeView.h"
#import "NoteView.h"


@interface MainController : NSWindowController <NSWindowDelegate>

  @property IBOutlet StrobeView *strobeView;
  @property IBOutlet NoteView *noteView;
  @property IBOutlet NSButton *modeButton;

  - (IBAction)modeButtonOnClick: (id)sender;

@end
