// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"


// TODO: reference additional headers your program requires here

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <ppl.h>

// Standard headers
#include <atomic>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <sstream>
#include <vector>

// Engine defines
#define USE_SSE4 1
#define USE_AVX (1 && USE_SSE4)
#define USE_AVX2 (1 && USE_AVX)

// Engine headers
#include "Alloc.h"
#include "Enums.h"
#include "VectorMath.h"
#include "Simd\Simd.h"
#include "Ray.h"