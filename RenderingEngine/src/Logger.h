#pragma once

#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h> 

// Custom OutputDebugString functions

static int _vDebugPrintF(const char* format, va_list argList);
static int _debugPrintF(const char* format, ...);