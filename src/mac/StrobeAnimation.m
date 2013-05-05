/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#import <OpenGL/OpenGL.h>
#import "StrobeAnimation.h"


@implementation StrobeAnimation


-(void)drawInCGLContext:(CGLContextObj)glContext
            pixelFormat:(CGLPixelFormatObj)pixelFormat
           forLayerTime:(CFTimeInterval)timeInterval
            displayTime:(const CVTimeStamp *)timeStamp {

  // Set the current context to the one given to us.
  CGLSetCurrentContext(glContext);

  // smooth lines
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  return;

  // // int len = 735;
  // int len = 441;

  // AudioFeed_read(
  //   _engine->audioFeed,
  //   _engine->audioBuffer,
  //   len
  // );

  // double dx = 2.0 / len;
  // double x  = -1;

  // glColor3ub(20, 190, 250);
  // glBegin(GL_LINE_STRIP);
  // // glPointSize(2);
  // // glBegin(GL_POINTS);
  // for (int i = 0; i < len; ++i) {
  //   // glVertex2d(x, _engine->audioFeed->filteredData[i]);
  //   glVertex2d(x, _engine->audioBuffer[i]);
  //   x += dx;
  // }
  // glEnd();


  // glColor3ub(190, 230, 20);
  // glPointSize(4);
  // glBegin(GL_POINTS);
  //   glVertex2d(_engine->pitchEstimation->fundamental / len, 0);
  // glEnd();






  // int len = _engine->pitchEstimation->fftLength / 4;
  // double dx = 2.0 / len;
  // // double dx = 2.0 / len;
  // double x  = -1;

  // glColor3ub(20, 190, 250);
  // glPointSize(4);
  // // glBegin(GL_LINE_STRIP);
  // glBegin(GL_POINTS);
  // for (int i = 0; i < len; ++i) {
  //   glVertex2d(x, _engine->pitchEstimation->autocorrData[i] * 0.00000005);
  //   x += dx;
  // }
  // glEnd();


  // glColor3ub(190, 230, 20);
  // glPointSize(4);
  // glBegin(GL_POINTS);
  //   glVertex2d(_engine->pitchEstimation->fundamental / len, 0);
  // glEnd();

  // printf("%f\n", _engine->pitchEstimation->autocorrData[0]);


  // Call super to finalize the drawing. By default all it does is call glFlush().
  [super drawInCGLContext:glContext
         pixelFormat:pixelFormat
         forLayerTime:timeInterval
         displayTime:timeStamp];

}


@end