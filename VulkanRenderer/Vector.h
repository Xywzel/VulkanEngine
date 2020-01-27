#pragma once

#include "Types.h"

/*
This header defines math vector types and their arithmetic operations
*/

struct Vec2u
{
	Vec2u();
	Vec2u(uint32_t x, uint32_t y);

	uint32_t x = 0u;
	uint32_t y = 0u;
};
