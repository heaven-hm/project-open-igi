#pragma once
#include <cstdint>
using HRESULT=long;using PSTR=char*;using LPSTR=char*;using LPCSTR=const char*;using LPVOID=void*;using LPDWORD=unsigned long*;
using DWORD=unsigned long;using UINT=unsigned int;using ULONG=unsigned long;using BOOL=int;using BYTE=unsigned char;using WORD=unsigned short;
using GUID=struct{unsigned char d[16];};using HMODULE=void*;using HANDLE=void*;using HDC=void*;using HGDIOBJ=void*;using HPEN=void*;using HBRUSH=void*;using HFONT=void*;
using HWND=void*;using COLORREF=unsigned int;using ATOM=unsigned short;using LRESULT=long;using LPARAM=long;using WPARAM=unsigned long;using HIC=void*;using HKEY=void*;using HBITMAP=void*;
using PVOID=void*;using LPBYTE=unsigned char*;using DWORD_PTR=unsigned long;using ULONG_PTR=unsigned long;using SIZE_T=unsigned long;using SSIZE_T=long;
struct _EXPT{};using PEXCEPTION_POINTERS=_EXPT*;
// discovered struct placeholders:
struct BITMAPINFOHEADER {};

struct Camera_ProgramNameUnknown {};

struct EXCEPTION_POINTERS {};

struct EXCEPTION_RECORD {};

struct FILETIME {};

struct HINSTANCE {};

struct HMIXEROBJ {};

struct LPDDENUMCALLBACKEXA {};

struct fpu7 {};

struct fpu_val_6_1 {};

struct fpu_val_6_5 {};

struct fpu_val_7_20 {};

struct fpu_val_7_3 {};

struct fpu_val_7_47 {};

struct fpu_val_7_6 {};

struct int80_t {};

struct wchar16 {};

struct weapon_type_context_t {};

