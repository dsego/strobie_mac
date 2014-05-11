// Copyright (c) Davorin Šego. All rights reserved.

#include "Engine.h"


void StrobeDisplay_setup(Engine *engine);
void StrobeDisplay_cleanup(Engine *engine);
// rw & rh are dimensions in real pixels (to support retina screens)
void StrobeDisplay_initScene(Engine *engine, int width, int height, float scaleFactor);
void StrobeDisplay_drawScene(Engine *engine);
