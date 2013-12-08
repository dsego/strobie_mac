/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#import "StrobeView.h"
#import "../engine/StrobeDisplay.h"
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

}


- (void)prepareOpenGL {

  [super prepareOpenGL];

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];

  // Synchronize buffer swaps with vertical refresh rate
  GLint swapInt = 1;
  [context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

  StrobeDisplay_setup(engine);
  StrobeDisplay_initScene(engine, _bounds.size.width, _bounds.size.height);

  CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
  CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, (__bridge void *)self);
  CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, [context CGLContextObj], cglPixelFormat);
  CVDisplayLinkStart(displayLink);

  // Register to be notified when the window closes so we can stop the displaylink
  [[NSNotificationCenter defaultCenter]
    addObserver:self
    selector:@selector(windowWillClose:)
    name:NSWindowWillCloseNotification
    object:self.window
  ];

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
static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext) {

  @autoreleasepool {
    [(__bridge StrobeView*)displayLinkContext redraw];
  }
  return kCVReturnSuccess;

}


- (void)redraw {

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];

  CGLContextObj cglContext = [context CGLContextObj];
  CGLLockContext(cglContext);

  StrobeDisplay_drawScene(engine);

  CGLFlushDrawable(cglContext);
  CGLUnlockContext(cglContext);

}


-(void)drawRect:(NSRect)bounds {

  [self redraw];

}


- (void)update {

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];
  CGLLockContext([context CGLContextObj]);
  [context update];
  CGLUnlockContext([context CGLContextObj]);

}


-(void)reshape {

  NSOpenGLContext* context = [self openGLContext];
  [context makeCurrentContext];
  CGLLockContext([context CGLContextObj]);
  StrobeDisplay_initScene(engine, _bounds.size.width, _bounds.size.height);
  CGLUnlockContext([context CGLContextObj]);

}


- (BOOL)mouseDownCanMoveWindow {

  return YES;

}


-(void)dealloc {

  [[NSNotificationCenter defaultCenter] removeObserver:self];
  CVDisplayLinkStop(displayLink);
  CVDisplayLinkRelease(displayLink);
  StrobeDisplay_cleanup();

}



@end
