#pragma once

constexpr char CONTROLS[]
{
	"--------\n"
	"CONTROLS:\n"
	"WASD:	 Move Camera\n"
	"F2:	 Toggle Shadows\n"
	"F3:	 Cycle Lighting Modes\n"
	"F6:      Start Benchmark\n"
#ifdef REFLECT
	"UP/DOWN: In-/decrement Reflection Bounces\n"
#endif
	"SCROLL:  In-/decrease Field Of View\n"
	"X:       Take Screenshot\n"
};

constexpr float
PI{ 3.14159265358979323846f },
HALF_PI{ 1.57079632679489661923f },
QUARTER_PI{ 0.785398163397448309616f },
DOUBLE_PI{ 6.283185307179586476925f },
QUADRUPLE_PI{ 12.56637061435917295385f },
TO_DEGREES{ 180.0f / PI },
TO_RADIANS{ PI / 180.0f };