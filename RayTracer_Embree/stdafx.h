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
#include <sstream>
#include <vector>

// Engine headers
#include "Ray.h"
#include "VectorMath.h"