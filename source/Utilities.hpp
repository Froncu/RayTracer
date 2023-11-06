#pragma once

#include <float.h>

#include "DataTypes.hpp"

template<typename Type>
inline Type Square(const Type& a)
{
	return a * a;
}

template<typename Type>
inline Type Lerp(const Type& a, const Type& b, float smoothFactor)
{
	return a + smoothFactor * (b - a);
}

inline bool AreEqual(float a, float b, float epsilon = FLT_EPSILON)
{
	return abs(a - b) < epsilon;
}

inline bool HitTestSphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
{
#ifdef SphereHitTestGeometric
	const Vector3 L{ sphere.origin - ray.origin };

	const float
		& sphereRadius{ sphere.radius },
		tca{ Vector3::Dot(L, ray.direction) },
		odSquared{ Vector3::Dot(L, L) - (tca * tca) },
		sphereRadiusSquared{ sphereRadius * sphereRadius };

	if (odSquared > sphereRadiusSquared)
		return false;

	const float thc{ sqrtf(sphereRadiusSquared - odSquared) };

	float t{ tca - thc };
	if (t < ray.min)
		t = tca + thc;
#else
	const Vector3 deltaOrigin{ ray.origin - sphere.origin };

	const float
		& sphereRadius{ sphere.radius },
		b{ Vector3::Dot(ray.direction, deltaOrigin) },
		c{ Vector3::Dot(deltaOrigin, deltaOrigin) - sphereRadius * sphereRadius },
		discriminant{ b * b - c };

	if (discriminant <= 0)
		return false;

	const float squareRootedDiscriminant{ sqrtf(discriminant) };

	float t{ (-b - squareRootedDiscriminant) };
	if (t < ray.min)
		t = (-b + squareRootedDiscriminant);
#endif
	if (t < ray.min || t > ray.max)
		return false;

	if (ignoreHitRecord)
		return true;

	if (t < hitRecord.t)
	{
		hitRecord.t = t;

		hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
		hitRecord.normal = (hitRecord.origin - sphere.origin) / sphere.radius;

		hitRecord.didHit = true;
		hitRecord.materialIndex = sphere.materialIndex;
		return true;
	}

	return false;
}

inline bool HitTestSphere(const Sphere& sphere, const Ray& ray)
{
	HitRecord temporary;
	return HitTestSphere(sphere, ray, temporary, true);
}

inline bool HitTestPlane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
{
	const float t{ Vector3::Dot(plane.origin - ray.origin, plane.normal) / Vector3::Dot(ray.direction, plane.normal) };

	if (t < ray.min || t > ray.max)
		return false;

	if (ignoreHitRecord)
		return true;

	if (t < hitRecord.t)
	{
		hitRecord.t = t;

		hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
		hitRecord.normal = plane.normal;

		hitRecord.didHit = true;
		hitRecord.materialIndex = plane.materialIndex;
		return true;
	}

	return false;
}

inline bool HitTestPlane(const Plane& plane, const Ray& ray)
{
	HitRecord temporary;
	return HitTestPlane(plane, ray, temporary, true);
}

inline bool SlabTestTriangleMesh(const TriangleMesh& triangleMesh, const Ray& ray)
{
	const Vector3
		smallestAABBTransformed{ triangleMesh.smallestAABBTransformed },
		largestAABBTransformed{ triangleMesh.largestAABBTransformed },
		rayOrigin{ ray.origin },
		rayDirection{ ray.direction };

	const float
		tx1{ (smallestAABBTransformed.x - rayOrigin.x) / rayDirection.x },
		tx2{ (largestAABBTransformed.x - rayOrigin.x) / rayDirection.x },

		tyl{ (smallestAABBTransformed.y - rayOrigin.y) / rayDirection.y },
		ty2{ (largestAABBTransformed.y - rayOrigin.y) / rayDirection.y },

		tzl{ (smallestAABBTransformed.z - rayOrigin.z) / rayDirection.z },
		tz2{ (largestAABBTransformed.z - rayOrigin.z) / rayDirection.z };

	float 
		tMin{ std::min(tx1, tx2) },
		tMax{ std::max(tx1, tx2) };

	tMin = std::max(tMin, std::min(tyl, ty2));
	tMax = std::min(tMax, std::max(tyl, ty2));

	tMin = std::max(tMin, std::min(tzl, tz2));
	tMax = std::min(tMax, std::max(tzl, tz2));

	return tMax > 0 && tMax >= tMin;
}

inline bool HitTestTriangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
{
	const Vector3
		& normal{ triangle.normal },
		& rayDirection{ ray.direction };

	const float dotNormalRayDirection{ Vector3::Dot(normal, rayDirection) };

	switch (triangle.cullMode)
	{
	case Triangle::CullMode::frontFace:
		if (dotNormalRayDirection <= 0.0f)
			return false;
		break;

	case Triangle::CullMode::backFace:
		if (dotNormalRayDirection >= 0.0f)
			return false;
		break;

	case Triangle::CullMode::none:
		if (AreEqual(dotNormalRayDirection, 0.0f))
			return false;
		break;
	}

	const Vector3
		& v0{ triangle.v0 },
		& v1{ triangle.v1 },
		& v2{ triangle.v2 },
		& rayOrigin{ ray.origin },
		L{ v0 - rayOrigin };

	const float
		dotRayDirectionNormal{ Vector3::Dot(rayDirection, normal) },
		t{ Vector3::Dot(L, normal) / dotRayDirectionNormal };
	if (t < ray.min || t > ray.max)
		return false;

	const Vector3
		P{ rayOrigin + rayDirection * t },
		* apVertices[]{ &v0, &v1, &v2 };
	for (int index{}; index < 3; ++index)
	{
		const Vector3
			& startingVertex{ *apVertices[index] },
			& endingVertex{ *apVertices[(index + 1) % 3] },
			a{ endingVertex - startingVertex },
			c{ P - startingVertex };

		if (Vector3::Dot(Vector3::Cross(a, c), normal) < 0.0f)
			return false;
	}

	if (ignoreHitRecord)
		return true;

	if (t < hitRecord.t)
	{
		hitRecord.t = t;

		hitRecord.origin = P;
		hitRecord.normal = normal;

		hitRecord.didHit = true;
		hitRecord.materialIndex = triangle.materialIndex;
		return true;
	}

	return false;
}

inline bool HitTestTriangle(const Triangle& triangle, const Ray& ray)
{
	HitRecord temporary;
	return HitTestTriangle(triangle, ray, temporary, true);
}

inline bool HitTestTriangleMesh(const TriangleMesh& triangleMesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
{
	if (!SlabTestTriangleMesh(triangleMesh, ray))
		return false;

	bool didHit{};
	for (int index{}; index < triangleMesh.vIndices.size(); index += 3)
	{
		if (HitTestTriangle(
			Triangle(
				triangleMesh.vPositionsTransformed[triangleMesh.vIndices[index]],
				triangleMesh.vPositionsTransformed[triangleMesh.vIndices[index + 1]],
				triangleMesh.vPositionsTransformed[triangleMesh.vIndices[index + 2]],
				triangleMesh.vNormalsTransformed[index / 3],
				triangleMesh.materialIndex,
				triangleMesh.cullMode), ray, hitRecord, ignoreHitRecord))
		{
			if (ignoreHitRecord)
				return true;
			
			didHit = true;
		}
	}

	return didHit;
}

inline bool HitTestTriangleMesh(const TriangleMesh& mesh, const Ray& ray)
{
	HitRecord temporary;
	return HitTestTriangleMesh(mesh, ray, temporary, true);
}

inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
{
	return light.origin - origin;
}

inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
{
	const Vector3 targetToOrigin{ light.origin - target };
	return light.color * (light.intensity / Vector3::Dot(targetToOrigin, targetToOrigin));
}