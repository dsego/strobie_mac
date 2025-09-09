// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.


#include <libkern/OSAtomic.h>

#define FullMemoryBarrier()  OSMemoryBarrier()

#define CASB(a,b,c)   OSAtomicCompareAndSwapIntBarrier(a, b, c);
