// IGI native provenance: Light_CalculateAttenuation_10 - 0x0052D440 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D440-Light_CalculateAttenuation_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_CalculateAttenuation_10.hpp"
unsigned int* Light_CalculateAttenuation_10(unsigned int* context)

{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    WORD wYear = systemTime.wYear;
    *(&wYear + 2) = systemTime.wMonth;
    WORD wMonth = systemTime.wMonth;
    *(&wMonth + 2) = systemTime.wDayOfWeek;
    *context = wYear;
    WORD wDay = systemTime.wDay;
    *(&wDay + 2) = systemTime.wHour;
    context[1] = wMonth;
    WORD wHour = systemTime.wHour;
    *(&wHour + 2) = systemTime.wMinute;
    context[2] = wDay;
    WORD wMinute = systemTime.wMinute;
    *(&wMinute + 2) = systemTime.wSecond;
    context[3] = wHour;
    WORD wSecond = systemTime.wSecond;
    *(&wSecond + 2) = systemTime.wMilliseconds;
    context[4] = wMinute;
    unsigned int entity_4 = *(&systemTime + 0xe);
    context[5] = wSecond;
    context[6] = entity_4;
    return context;
}
