//
//  AppDelegate.m
//  Strobie
//
//  Created by Davorin Šego on 4/13/13.
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#import "engine.h"
#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    Engine* engine = Engine_create();
    Engine_init_audio(engine);
    Note note;
    note.frequency = 440;
}

@end
