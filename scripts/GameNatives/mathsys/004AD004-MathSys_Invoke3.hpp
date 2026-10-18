#pragma once
#include <stdint.h>
#include <stdbool.h>

/* 004AD004 MathSys_Invoke3 | gate PASS | openigi True */
int __convention("regparm") MathSys_Invoke3(int arg1, int arg2, int arg3, int* arg4, void* arg5, void* arg6, int arg7);
