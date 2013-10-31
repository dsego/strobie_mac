/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "NoteView.h"
#import "shared.h"


@implementation NoteView



- (void)drawRect: (NSRect)rect {


  NSMutableString *string = [NSMutableString stringWithCapacity: 10];

  [string setString: @"hello world"];

  // (NSDictionary *)attributes[]
  [string drawAtPoint: (NSPoint){0,0} withAttributes: nil];


}


@end
