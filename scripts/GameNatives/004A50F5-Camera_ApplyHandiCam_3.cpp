// IGI native provenance: Camera_ApplyHandiCam_3 - 0x004A50F5 - retail WinMainCRTStartup (CRT entry point); label is the catalog name only.
// Transcribed from retail igi.exe 0x004A50F5 (radare2). SEH frame: scope table 0x534308, handler 0x4a67ec.
#include <windows.h>
#include <stdio.h>
#include "004A50F5-Camera_ApplyHandiCam_3.hpp"

extern unsigned int g_93607c, g_936078, g_936074, g_936070, g_936054;
extern LPSTR g_c32554;
extern char* g_93604c;
int Camera_ApplyPostProcess_8(int context);              // 0x004A6787
void Camera_ResetMatrix_3(int context);                  // 0x004A5210
unsigned int Camera_ApplyHandiCam_8(void);               // 0x004A6467
PSTR Camera_ApplyBandFilter_8(void);                     // 0x004A6335
void Camera_SetFieldOfView_8_cdecl(void);                // 0x004A60E8 (retail passes no argument)
int Camera_UpdateTransform_8(void);                      // 0x004A602F
int Camera_ApplyLineFilter_6(void);                      // 0x004A5AA8
char* Camera_ResetMatrix_7(void);                        // 0x004A5FD7
int Camera_ApplyBandFilter_6(unsigned int exitCode);     // 0x004A5AD5
int Camera_ApplyPostProcess_7(int code, EXCEPTION_POINTERS* ptrs); // 0x004A5D72
void Camera_Exit_5A05(int code);                         // 0x004A5A05
int main(HINSTANCE, HINSTANCE, char*, int);              // retail 'main' call at 0x004A51BB (WinMain)

int Camera_ApplyHandiCam_3()
{
    int rc = 0;
    int xcode = 0;
    try {
        DWORD ver = GetVersion();
        g_93607c = (ver >> 8) & 0xff;
        g_936078 = ver & 0xff;
        g_936074 = (g_936078 << 8) + g_93607c;
        g_936070 = ver >> 16;

        if (!Camera_ApplyPostProcess_8(0))
            Camera_ResetMatrix_3(0x1c);

        Camera_ApplyHandiCam_8();
        g_c32554 = GetCommandLineA();
        g_93604c = Camera_ApplyBandFilter_8();
        Camera_SetFieldOfView_8_cdecl();
        Camera_UpdateTransform_8();
        Camera_ApplyLineFilter_6();

        STARTUPINFOA si;
        si.dwFlags = 0;
        GetStartupInfoA(&si);
        char* cmdLine = Camera_ResetMatrix_7();
        int nShow = (si.dwFlags & 1) ? si.wShowWindow : 10;

        rc = main(GetModuleHandleA(nullptr), nullptr, cmdLine, nShow);
        Camera_ApplyBandFilter_6((unsigned int)rc);
    } catch (...) {
        // retail handler: _XcptFilter(code, ptrs) then _exit(code); exception code is not recoverable from a C++ catch
        fprintf(stderr, "Camera_ApplyHandiCam_3: exception in CRT entry path\n");
        Camera_Exit_5A05(xcode);
    }
    return rc;
}
