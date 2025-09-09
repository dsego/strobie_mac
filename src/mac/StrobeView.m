// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.


#import "StrobeView.h"
#import "../engine/strobe-display.h"
#import "shared.h"



@implementation StrobeView {

  CVDisplayLinkRef     displayLink;
  NSOpenGLContext*     context;
  NSOpenGLPixelFormat* pixelFormat;

}


- (id)initWithFrame:(NSRect)frameRect {

  self = [self initWithFrame:frameRect shareContext:nil];
  return self;

}


- (id) initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)aContext {

  self = [super initWithFrame:frameRect];

  if (self) {
// - (void)awakeFromNib {

    NSOpenGLPixelFormatAttribute attributes[] = {
      NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,   // Core Profile !
      NSOpenGLPFADoubleBuffer,
      0
    };

    pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: nil];
    [self prepareOpenGL];

    // Register to be notified when the window closes so we can stop the display link
    [[NSNotificationCenter defaultCenter] addObserver:self
      selector:@selector(windowWillClose:)
      name:NSWindowWillCloseNotification
      object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
      selector:@selector(reshape)
      name:NSViewGlobalFrameDidChangeNotification
      object:self];

    [self setWantsBestResolutionOpenGLSurface: YES];

  }

  // [self setPixelFormat: pixelFormat];
  // [self setOpenGLContext: context];
  return self;

}


- (BOOL)isOpaque {

  return YES;

}


- (void)prepareOpenGL {


  CGLContextObj cglContext = [context CGLContextObj];
  [context makeCurrentContext];

  // Synchronize buffer swaps with vertical refresh rate
  GLint swapInt = 1;
  [context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

  CGLLockContext(cglContext);

  // support retina
  NSRect backing = [self convertRectToBacking:[self bounds]];
  CGFloat scale = [self convertSizeToBacking:CGSizeMake(1,1)].width;
  [self setupCVDisplayLink];
  StrobeDisplay_setup(engine);
  StrobeDisplay_initScene(engine, backing.size.width, backing.size.height, scale);

  CGLUnlockContext(cglContext);

}


- (void)setupCVDisplayLink {

  CGLContextObj cglContext = [context CGLContextObj];
  CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
  CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, (__bridge void *)self);
  CGLPixelFormatObj cglPixelFormat = [pixelFormat CGLPixelFormatObj];
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

}


// - (void)update {

//   CGLContextObj cglContext = [context CGLContextObj];
//   [context makeCurrentContext];
//   CGLLockContext(cglContext);
//   [context update];
//   CGLUnlockContext(cglContext);

// }


- (void) windowWillClose:(NSWindow*)window {

  // The notification object is the NSWindow object that is about to close.
  if (window == self.window) {

    // Stop the display link when the window is closing because default
    // OpenGL render buffers will be destroyed.  If display link continues to
    // fire without render buffers, OpenGL draw calls will set errors.
    CVDisplayLinkStop(displayLink);
  }

}


// - (void)renewGState {

//   // Called whenever graphics state updated (such as window resize)

//   // OpenGL rendering is not synchronous with other rendering on the OSX.
//   // Therefore, call disableScreenUpdatesUntilFlush so the window server
//   // doesn't render non-OpenGL content in the window asynchronously from
//   // OpenGL content, which could cause flickering.  (non-OpenGL content
//   // includes the title bar and drawing done by the app with other APIs)
//   [[self window] disableScreenUpdatesUntilFlush];
//   [super renewGState];

// }


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

  CGLContextObj cglContext = [context CGLContextObj];
  [context makeCurrentContext];
  CGLLockContext(cglContext);

  StrobeDisplay_drawScene(engine);

  [context flushBuffer];
  CGLUnlockContext(cglContext);

}


- (void)drawRect:(NSRect)bounds {

  if (context.view != self) {
    [context setView: self];
  }
  // [self redraw];

}

- (void)reshape {

  CGLContextObj cglContext = [context CGLContextObj];
  CGLLockContext(cglContext);
  [context makeCurrentContext];

  // support retina
  NSRect backing = [self convertRectToBacking:[self bounds]];
  CGFloat scale = [self convertSizeToBacking:CGSizeMake(1,1)].width;
  StrobeDisplay_initScene(engine, backing.size.width, backing.size.height, scale);

  [context update];
  StrobeDisplay_drawScene(engine);
  [context flushBuffer];

  CGLUnlockContext(cglContext);

}


- (void)startAnimation {

  if (displayLink && !CVDisplayLinkIsRunning(displayLink)) {
    CVDisplayLinkStart(displayLink);
  }

}


- (void)stopAnimation {

  if (displayLink && CVDisplayLinkIsRunning(displayLink)) {
    CVDisplayLinkStop(displayLink);
  }

}


// - (void)unlockFocus {

// }


// - (void)lockFocus {



// }


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
