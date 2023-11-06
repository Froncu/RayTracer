#pragma once

#include "BRDFs.hpp"
#include "DataTypes.hpp"

class Material
{
public:
	Material(float roughness) : 
		m_Roughness{ roughness }
	{
	}

	virtual ~Material() = default;

	Material(const Material&) = delete;
	Material(Material&&) noexcept = delete;
	Material& operator=(const Material&) = delete;
	Material& operator=(Material&&) noexcept = delete;

	virtual ColorRGB Shade(const HitRecord& hitRecord, const Vector3& lightDirection, const Vector3& viewDirection) const = 0;

	const float m_Roughness;
};

class SolidColorMaterial final : public Material
{
public:
	SolidColorMaterial(const ColorRGB& color = WHITE, float roughness = 1.0f) :
		Material(roughness),

		m_Color(color)
	{
	}

	virtual ~SolidColorMaterial() override = default;

	SolidColorMaterial(const SolidColorMaterial&) = delete;
	SolidColorMaterial(SolidColorMaterial&&) noexcept = delete;
	SolidColorMaterial& operator=(const SolidColorMaterial&) = delete;
	SolidColorMaterial& operator=(SolidColorMaterial&&) noexcept = delete;

	virtual ColorRGB Shade([[maybe_unused]] const HitRecord& hitRecord = HitRecord(), [[maybe_unused]] const Vector3& lightDirection = Vector3(), [[maybe_unused]] const Vector3& viewDirection = Vector3()) const override
	{
		return m_Color;
	}

private:
	ColorRGB m_Color;
};

class LambertMaterial : public Material
{
public:
	LambertMaterial(const ColorRGB& diffuseColor = WHITE, float diffuseReflectance = 1.0f, float roughness = 1.0f) :
		Material(roughness),

		m_DiffuseColor(diffuseColor),
		m_DiffuseReflectance(diffuseReflectance)
	{
	}

	virtual ~LambertMaterial() override = default;

	LambertMaterial(const LambertMaterial&) = delete;
	LambertMaterial(LambertMaterial&&) noexcept = delete;
	LambertMaterial& operator=(const LambertMaterial&) = delete;
	LambertMaterial& operator=(LambertMaterial&&) noexcept = delete;

	virtual ColorRGB Shade([[maybe_unused]] const HitRecord& hitRecord = HitRecord(), [[maybe_unused]] const Vector3& lightDirection = Vector3(), [[maybe_unused]] const Vector3& viewDirection = Vector3()) const override
	{
		return Lambert(m_DiffuseReflectance, m_DiffuseColor);
	}

protected:
	ColorRGB m_DiffuseColor;
	float m_DiffuseReflectance;
};

class LambertPhongMaterial final : public LambertMaterial
{
public:
	LambertPhongMaterial(const ColorRGB& diffuseColor = WHITE, float diffuseReflectance = 0.5f, float specularReflectance = 0.5f, float phongExponent = 1.0f, float roughness = 1.0f) :
		LambertMaterial(diffuseColor, diffuseReflectance, roughness),

		m_SpecularReflectance{ specularReflectance },
		m_PhongExponent(phongExponent)
	{
	}

	virtual ~LambertPhongMaterial() override = default;

	LambertPhongMaterial(const LambertPhongMaterial&) = delete;
	LambertPhongMaterial(LambertPhongMaterial&&) noexcept = delete;
	LambertPhongMaterial& operator=(const LambertPhongMaterial&) = delete;
	LambertPhongMaterial& operator=(LambertPhongMaterial&&) noexcept = delete;

	virtual ColorRGB Shade(const HitRecord& hitRecord, const Vector3& lightDirection, const Vector3& viewDirection) const override
	{
		return
			Lambert(m_DiffuseReflectance, m_DiffuseColor) +
			Phong(m_SpecularReflectance, m_PhongExponent, lightDirection, viewDirection, hitRecord.normal);
	}

private:
	float 
		m_SpecularReflectance,
		m_PhongExponent;
};

class CookTorrenceMaterial final : public Material
{
public:
	CookTorrenceMaterial(const ColorRGB& albedo = ColorRGB(0.955f, 0.637f, 0.538f), float metalness = 1.0f, float roughness = 1.0f) :
		Material(roughness),

		m_Albedo(albedo), 
		m_Metalness(metalness)
	{
	}

	virtual ~CookTorrenceMaterial() override = default;

	CookTorrenceMaterial(const CookTorrenceMaterial&) = delete;
	CookTorrenceMaterial(CookTorrenceMaterial&&) noexcept = delete;
	CookTorrenceMaterial& operator=(const CookTorrenceMaterial&) = delete;
	CookTorrenceMaterial& operator=(CookTorrenceMaterial&&) noexcept = delete;

	virtual ColorRGB Shade(const HitRecord& hitRecord, const Vector3& lightDirection, const Vector3& viewDirection) const override
	{
		const Vector3
			negativeViewDirection{ -viewDirection },
			h{ (negativeViewDirection + lightDirection).GetNormalized() };

		const ColorRGB
			f0{ m_Metalness == 0.0f ? ColorRGB(0.04f, 0.04f, 0.04f) : m_Albedo },
			f{ FresnelFunctionSchlick(h, negativeViewDirection, f0) };

		const float
			d{ NormalDistributionGGX(hitRecord.normal, h, m_Roughness) },
			g{ GeometryFunctionSmith(hitRecord.normal, negativeViewDirection, lightDirection, m_Roughness) };

		const ColorRGB
			specular{ (f * d * g) / (4.0f * Vector3::Dot(negativeViewDirection, hitRecord.normal) * Vector3::Dot(lightDirection, hitRecord.normal)) },
			kd{ m_Metalness == 0.0f ? (WHITE - f) : BLACK },
			diffuse{ Lambert(kd, m_Albedo) };

		return specular + diffuse;
	}

private:
	ColorRGB m_Albedo;
	float m_Metalness;
};