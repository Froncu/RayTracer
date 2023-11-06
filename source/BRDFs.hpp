#pragma once

#include <cmath>

#include "Constants.hpp"

static inline ColorRGB Lambert(float diffuseReflectance, const ColorRGB& diffuseColor)
{
	return diffuseReflectance * diffuseColor / PI;
}

static inline ColorRGB Lambert(const ColorRGB& diffuseReflectance, const ColorRGB& diffuseColor)
{
	return diffuseReflectance * diffuseColor / PI;
}

static inline ColorRGB Phong(float specularReflectance, float phongExponent, const Vector3& lightDirection, const Vector3& viewDirection, const Vector3& normal)
{
	const Vector3 reflectedLightDirection{ Vector3::Reflect(lightDirection, normal) };
	const float dot{ Vector3::Dot(reflectedLightDirection, viewDirection) };
	if (dot <= 0.0f)
		return BLACK;

	return WHITE * specularReflectance * powf(dot, phongExponent);
}

static inline ColorRGB FresnelFunctionSchlick(const Vector3& h, const Vector3& viewDirection, const ColorRGB& f0)
{
	const float inverseDothv{ 1.0f - Vector3::Dot(h, viewDirection) };
	return f0 + (WHITE - f0) * inverseDothv * inverseDothv * inverseDothv * inverseDothv * inverseDothv;
}

static inline float NormalDistributionGGX(const Vector3& normal, const Vector3& h, float roughness)
{
	const float
		aSquared{ roughness * roughness * roughness * roughness },
		dot{ Vector3::Dot(normal, h) },
		denominator{ dot * dot * (aSquared - 1.0f) + 1.0f };

	return aSquared / (PI * denominator * denominator);
}

static inline float GeometryFunctionSchlickGGX(const Vector3& normal, const Vector3& viewDirection, float roughness)
{
	const float dot{ Vector3::Dot(normal, viewDirection) };
	if (dot <= 0.0f)
		return 0.0f;

	const float
		nominatorRooted{ roughness * roughness + 1.0f },
		k{ nominatorRooted * nominatorRooted * 0.125f };
	return dot / (dot * (1.0f - k) + k);
}

static inline float GeometryFunctionSmith(const Vector3& normal, const Vector3& viewDirection, const Vector3& lightDirection, float roughness)
{
	return
		GeometryFunctionSchlickGGX(normal, viewDirection, roughness) *
		GeometryFunctionSchlickGGX(normal, lightDirection, roughness);
}