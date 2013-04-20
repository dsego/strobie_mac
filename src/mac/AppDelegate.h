//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (strong) IBOutlet NSWindow *window;
@property (strong) IBOutlet NSWindowController *windowController;
// @property (weak) IBOutlet StrobeView *strobeView;
// @property (weak) IBOutlet NSTextField *label;
//
-(IBAction) openOnlineDocumentation:(id)sender;

@end
