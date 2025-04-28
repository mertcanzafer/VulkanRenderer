#include "Logger.h"
#include "DefinesAndMacros.h"

int _vDebugPrintF(const char* format, va_list argList)
{
    const uint32_t MAX_CHARS = 1023U;
    static char s_buffer[MAX_CHARS + 1]; // 1KB

    auto charsWritten = vsnprintf_s(s_buffer, MAX_CHARS, format, argList
    );

    // We have a formatted string
    // Call Win32 API
    OutputDebugStringA(s_buffer);
    return charsWritten;
}

int _debugPrintF(const char* format, ...)
{
    va_list argList;

    va_start(argList, format);

    auto charsWritten = _vDebugPrintF(format, argList);

    va_end(argList);
    return charsWritten;
}
