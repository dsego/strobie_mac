// Copyright (c) Davorin Šego. All rights reserved.


#import "StrobeView.h"
#import "../engine/strobe-display.h"
#import "shared.h"




@implementation StrobeView {

  CVDisplayLinkRef displayLink;

}


- (void)awakeFromNib {

  NSOpenGLPixelFormatAttribute attributes[] = {
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,   // Core Profile !
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFAColorSize, 24,
    NSOpenGLPFAAlphaSize, 8,
    NSOpenGLPFAAllowOfflineRenderers,
    0
  };

  NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
  NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:format shareContext: nil];
  [self setPixelFormat: format];
  [self setOpenGLContext: context];
  [self setWantsBestResolutionOpenGLSurface: YES];

}


- (void)prepareOpenGL {

  [super prepareOpenGL];

  // Register to be notified when the window closes so we can stop the displaylink
  [[NSNotificationCenter defaultCenter]
    addObserver:self
    selector:@selector(windowWillClose:)
    name:NSWindowWillCloseNotification
    object:self.window
  ];

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];

  // Synchronize buffer swaps with vertical refresh rate
  GLint swapInt = 1;
  [context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

  // support retina
  NSRect backingBounds = [self convertRectToBacking:[self bounds]];
  CGFloat scaleFactor = [self convertSizeToBacking:CGSizeMake(1,1)].width;
  StrobeDisplay_setup(engine);
  StrobeDisplay_initScene(
    engine,
    backingBounds.size.width,
    backingBounds.size.height,
    scaleFactor
  );

  CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
  CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, (__bridge void *)self);
  CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(
    displayLink,
    [context CGLContextObj],
    cglPixelFormat
  );
  CVDisplayLinkStart(displayLink);

}


- (void) windowWillClose:(NSNotification*)notification {

  // Stop the display link when the window is closing because default
  // OpenGL render buffers will be destroyed.  If display link continues to
  // fire without renderbuffers, OpenGL draw calls will set errors.
  CVDisplayLinkStop(displayLink);

}

- (void)renewGState {

  // Called whenever graphics state updated (such as window resize)

  // OpenGL rendering is not synchronous with other rendering on the OSX.
  // Therefore, call disableScreenUpdatesUntilFlush so the window server
  // doesn't render non-OpenGL content in the window asynchronously from
  // OpenGL content, which could cause flickering.  (non-OpenGL content
  // includes the title bar and drawing done by the app with other APIs)
  [[self window] disableScreenUpdatesUntilFlush];

  [super renewGState];

}


// This is the renderer output callback function
static CVReturn displayLinkCallback(
  CVDisplayLinkRef displayLink,
  const CVTimeStamp* now,
  const CVTimeStamp* outputTime,
  CVOptionFlags flagsIn,
  CVOptionFlags* flagsOut,
  void* displayLinkContext) {

  @autoreleasepool {
    [(__bridge StrobeView*)displayLinkContext redraw];
  }
  return kCVReturnSuccess;

}


- (void)redraw {

  NSOpenGLContext* context = [self openGLContext];
  CGLContextObj cglContext = [context CGLContextObj];
  [context makeCurrentContext];
  CGLLockContext(cglContext);

  StrobeDisplay_drawScene(engine);

  [context flushBuffer];
  CGLUnlockContext(cglContext);

}


-(void)drawRect:(NSRect)bounds {

  [self redraw];

}


// - (void)update {

//   NSOpenGLContext* context = [self openGLContext];
//   CGLContextObj cglContext = [context CGLContextObj];
//   [context makeCurrentContext];
//   CGLLockContext(cglContext);
//   [context update];
//   CGLUnlockContext(cglContext);

// }


-(void)reshape {

  NSOpenGLContext* context = [self openGLContext];
  CGLContextObj cglContext = [context CGLContextObj];
  [context makeCurrentContext];
  CGLLockContext(cglContext);

  // support retina
  NSRect backing = [self convertRectToBacking:[self bounds]];
  CGFloat scaleFactor = [self convertSizeToBacking:CGSizeMake(1,1)].width;

  StrobeDisplay_initScene(engine, backing.size.width, backing.size.height, scaleFactor);
  CGLUnlockContext(cglContext);

}


- (BOOL)mouseDownCanMoveWindow {

  return YES;

}


-(void)dealloc {

  [[NSNotificationCenter defaultCenter] removeObserver:self];
  CVDisplayLinkStop(displayLink);
  CVDisplayLinkRelease(displayLink);
  StrobeDisplay_cleanup(engine);

}



@end
