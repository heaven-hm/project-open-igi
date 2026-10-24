#pragma once
#include <stdint.h>

/* 0x0048f260 AppContext_SetTerrainLightmapsUsed | BN signature: int32_t(char arg1) | readable identifiers, logic per 0048f260-AppContext_SetTerrainLightmapsUsed.cpp */
extern "C" int32_t AppContext_SetTerrainLightmapsUsed(char used);
