#pragma once
#include <stdint.h>

/* 0x00401530 QTask_LinkQTaskEventHandlerNoContextFn | BN signature: int32_t(uint16_t event_id, int32_t context, uint32_t flags) | readable identifiers, logic per 00401530-QTask_LinkQTaskEventHandlerNoContextFn.cpp */
extern "C" int32_t QTask_LinkQTaskEventHandlerNoContextFn(uint16_t event_id, int32_t context, uint32_t flags);
