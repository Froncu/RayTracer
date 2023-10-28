#pragma once
#include "Math.h"
#include "DataTypes.h"
#include "BRDFs.h"

namespace dae
{
#pragma region Material BASE
	class Material
	{
	public:
		Material(float roughness) : m_Roughness{ roughness } {};
		virtual ~Material() = default;

		Material(const Material&) = delete;
		Material(Material&&) noexcept = delete;
		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) noexcept = delete;

		/**
		 * \brief Function used to calculate the correct color for the specific material and its parameters
		 * \param hitRecord current hitrecord
		 * \param l light direction
		 * \param v view direction
		 * \return color
		 */
		virtual ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) const = 0;

		const float m_Roughness;
	};
#pragma endregion

#pragma region Material SOLID COLOR
	//SOLID COLOR
	//===========
	class Material_SolidColor final : public Material
	{
	public:
		Material_SolidColor(const ColorRGB& color, float roughness = 1.0f) :
			Material(roughness), 
			m_Color(color)
		{
		}

		ColorRGB Shade(const HitRecord& hitRecord, const Vector3& l, const Vector3& v) const override
		{
			return m_Color;
		}

	private:
		ColorRGB m_Color{colors::White};
	};
#pragma endregion

#pragma region Material LAMBERT
	//LAMBERT
	//=======
	class Material_Lambert final : public Material
	{
	public:
		Material_Lambert(const ColorRGB& diffuseColor, float diffuseReflectance, float roughness = 1.0f) :
			Material(roughness),
			m_DiffuseColor(diffuseColor), m_DiffuseReflectance(diffuseReflectance){}

		ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) const override
		{
			//todo: W3
			return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor);
		}

	private:
		ColorRGB m_DiffuseColor{colors::White};
		float m_DiffuseReflectance{1.f}; //kd
	};
#pragma endregion

#pragma region Material LAMBERT PHONG
	//LAMBERT-PHONG
	//=============
	class Material_LambertPhong final : public Material
	{
	public:
		Material_LambertPhong(const ColorRGB& diffuseColor, float kd, float ks, float phongExponent, float roughness = 1.0f) :
			Material(roughness),
			m_DiffuseColor(diffuseColor), m_DiffuseReflectance(kd), m_SpecularReflectance(ks),
			m_PhongExponent(phongExponent)
		{
		}

		ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) const override
		{
			//todo: W3
			return 
				BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor) +
				BRDF::Phong(m_SpecularReflectance, m_PhongExponent, l, -v, hitRecord.normal);
		}

	private:
		ColorRGB m_DiffuseColor{colors::White};
		float m_DiffuseReflectance{0.5f}; //kd
		float m_SpecularReflectance{0.5f}; //ks
		float m_PhongExponent{1.f}; //Phong Exponent
	};
#pragma endregion

#pragma region Material COOK TORRENCE
	//COOK TORRENCE
	class Material_CookTorrence final : public Material
	{
	public:
		Material_CookTorrence(const ColorRGB& albedo, float metalness, float roughness = 1.0f) :
			Material(roughness),
			m_Albedo(albedo), m_Metalness(metalness)
		{
		}

		ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) const override
		{
			//todo: W3
			const Vector3
				negativeViewDirection{ -v },
				h{ (negativeViewDirection + l).Normalized() };

			const ColorRGB
				f0{ m_Metalness == 0.0f ? ColorRGB(0.04f, 0.04f, 0.04f) : m_Albedo },
				f{ BRDF::FresnelFunction_Schlick(h, negativeViewDirection, f0) };

			const float
				d{ BRDF::NormalDistribution_GGX(hitRecord.normal, h, m_Roughness) },
				g{ BRDF::GeometryFunction_Smith(hitRecord.normal, negativeViewDirection, l, m_Roughness) };

			const ColorRGB
				specular{ (f * d * g) / (4.0f * Vector3::Dot(negativeViewDirection, hitRecord.normal) * Vector3::Dot(l, hitRecord.normal)) },
				kd{ m_Metalness == 0.0f ? (colors::White - f) : colors::Black },
				diffuse{ BRDF::Lambert(kd, m_Albedo) };

			return specular + diffuse;
		}

	private:
		ColorRGB m_Albedo{0.955f, 0.637f, 0.538f}; //Copper
		float m_Metalness{1.0f};
	};
#pragma endregion
}
