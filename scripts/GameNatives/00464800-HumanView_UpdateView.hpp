#pragma once
#include <stdint.h>

/* 0x00464800 HumanView_UpdateView | BN signature: float(float arg1, float arg2) __location("eax") | readable identifiers, logic per 00464800-HumanView_UpdateView.cpp */
extern "C" float HumanView_UpdateView(float yaw, float pitch);
