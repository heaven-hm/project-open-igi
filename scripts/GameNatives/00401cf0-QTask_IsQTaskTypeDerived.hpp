#pragma once
#include <stdint.h>

/* 0x00401cf0 QTask_IsQTaskTypeDerived | BN signature: int32_t(int32_t arg1, int16_t arg2) | readable identifiers, logic per 00401cf0-QTask_IsQTaskTypeDerived.cpp */
extern "C" int32_t QTask_IsQTaskTypeDerived(int32_t type_id, int16_t base_type_id);
