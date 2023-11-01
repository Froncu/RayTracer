#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
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

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp;
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
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

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp;
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			const Vector3
				& normal{ triangle.normal },
				& rayDirection{ ray.direction };

			const float dotNormalRayDirection{ Vector3::Dot(normal, rayDirection) };

			switch (triangle.cullMode)
			{	
			case TriangleCullMode::FrontFaceCulling:
				if (dotNormalRayDirection <= 0.0f)
					return false;
				break;

			case TriangleCullMode::BackFaceCulling:
				if (dotNormalRayDirection >= 0.0f)
					return false;
				break;

			case TriangleCullMode::NoCulling:
				if (dae::AreEqual(dotNormalRayDirection, 0.0f, FLT_EPSILON))
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

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			bool didHit{};
			Triangle triangle;
			triangle.cullMode = mesh.cullMode;
			triangle.materialIndex = mesh.materialIndex;
			for (size_t index{}; index < mesh.indices.size();)
			{
				triangle.normal = mesh.transformedNormals[index / 3];
				triangle.v0 = mesh.transformedPositions[mesh.indices[index++]];
				triangle.v1 = mesh.transformedPositions[mesh.indices[index++]];
				triangle.v2 = mesh.transformedPositions[mesh.indices[index++]];

				if (HitTest_Triangle(triangle, ray, hitRecord, ignoreHitRecord))
					didHit = true;
			}

			return didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			return 
				light.origin - origin;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			const Vector3 targetToOrigin{ light.origin - target };
			return light.color * (light.intensity / Vector3::Dot(targetToOrigin, targetToOrigin));
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}