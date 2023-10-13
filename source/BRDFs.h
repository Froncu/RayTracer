#pragma once
#include <cassert>
#include "Math.h"
#include <iostream>

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			//todo: W3
			return kd * cd / float(M_PI);
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			//todo: W3
			return kd * cd / float(M_PI);
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			//todo: W3
			const Vector3 r{ -l + 2.0f * Vector3::Dot(n, l) * n };
			const float dotrv{ std::max(Vector3::Dot(r, v), 0.0f) };

			return colors::White * ks * powf(dotrv, exp);
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			//todo: W3
			return f0 + (colors::White - f0) * powf(1.0f - Vector3::Dot(h, v), 5.0f);
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			//todo: W3
			const float a{ powf(roughness, 4.0f) };
			return a / (float(M_PI) * powf(powf(Vector3::Dot(n, h), 2.0f) * (a - 1.0f) + 1.0f, 2.0f));
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			//todo: W3
			const float
				dotnv{ std::max(Vector3::Dot(n, v), 0.0f) },
				k{ powf(powf(roughness, 2.0f) + 1.0f, 2.0f) / 8.0f };
			return dotnv / (dotnv * (1.0f - k) + k);
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			//todo: W3
			return 
				GeometryFunction_SchlickGGX(n, v, roughness) * 
				GeometryFunction_SchlickGGX(n, l, roughness);
		}
	}
}