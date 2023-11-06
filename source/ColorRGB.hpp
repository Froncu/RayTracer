#pragma once

#include <cmath>

struct ColorRGB
{
public:
	float
		red,
		green,
		blue;

	inline void MaxToOne()
	{
		const float maxValue = std::max(red, std::max(green, blue));
		if (maxValue > 1.0f)
			*this /= maxValue;
	}

	inline ColorRGB operator*(const ColorRGB& color) const
	{
		return ColorRGB(red * color.red, green * color.green, blue * color.blue);
	}

	inline ColorRGB operator*(float scalar) const
	{
		return ColorRGB(red * scalar, green * scalar, blue * scalar);
	}

	inline ColorRGB operator/(const ColorRGB& color) const
	{
		return ColorRGB(red / color.red, green / color.green, blue / color.blue);
	}

	inline ColorRGB operator/(float scalar) const
	{
		return ColorRGB(red / scalar, green / scalar, blue / scalar);
	}

	inline ColorRGB operator+(const ColorRGB& color) const
	{
		return ColorRGB(red + color.red, green + color.green, blue + color.blue);
	}

	inline ColorRGB operator-(const ColorRGB& color) const
	{
		return ColorRGB(red - color.red, green - color.green, blue - color.blue);
	}

	inline const ColorRGB& operator*=(const ColorRGB& color)
	{
		*this = *this * color;
		return *this;
	}

	inline const ColorRGB& operator*=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	inline const ColorRGB& operator/=(const ColorRGB& color)
	{
		*this = *this / color;
		return *this;
	}

	inline const ColorRGB& operator/=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	inline const ColorRGB& operator+=(const ColorRGB& color)
	{
		*this = *this + color;
		return *this;
	}

	inline const ColorRGB& operator-=(const ColorRGB& color)
	{
		*this = *this - color;
		return *this;
	}
};

inline ColorRGB operator*(float scalar, const ColorRGB& color)
{
	return color * scalar;
}

static constexpr ColorRGB
RED{ 1.0f, 0.0f, 0.0f },
BLUE{ 0.0f, 0.0f, 1.0f },
GREEN{ 0.0f, 1.0f, 0.0f },
YELLOW{ 1.0f, 1.0f, 0.0f },
CYAN{ 0.0f, 1.0f, 1.0f },
MAGENTA{ 1.0f, 0.0f, 1.0f },
WHITE{ 1.0f, 1.0f, 1.0f },
BLACK{ 0.0f, 0.0f, 0.0f },
GRAY{ 0.5f ,0.5f ,0.5f };