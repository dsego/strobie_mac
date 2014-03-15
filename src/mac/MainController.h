// Copyright (c) Davorin Šego. All rights reserved.

#import <AppKit/NSWindowController.h>
#import "StrobeView.h"
#import "NoteView.h"


@interface MainController : NSWindowController <NSWindowDelegate>

  @property IBOutlet StrobeView *strobeView;
  @property IBOutlet NoteView *noteView;
  @property IBOutlet NSButton *modeButton;

  - (IBAction)modeButtonOnClick: (id)sender;

@end
