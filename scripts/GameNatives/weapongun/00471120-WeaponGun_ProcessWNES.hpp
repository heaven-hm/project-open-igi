#pragma once
#include <stdint.h>
#include <stdbool.h>

/* 00471120 WeaponGun_ProcessWNES | gate PASS | openigi True */
int __convention("regparm") WeaponGun_ProcessWNES(int arg1, int arg2, int arg3, void* arg4);
