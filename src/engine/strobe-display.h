// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.

#include "Engine.h"


void StrobeDisplay_setup(Engine *engine);
void StrobeDisplay_cleanup(Engine *engine);
// rw & rh are dimensions in real pixels (to support retina screens)
void StrobeDisplay_initScene(Engine *engine, int width, int height, float scaleFactor);
void StrobeDisplay_drawScene(Engine *engine);
