#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "Matrix.hpp"
#include "ColorRGB.hpp"

struct Sphere
{
public:
	Vector3 origin;
	float radius;

	unsigned char materialIndex;
};

struct Plane
{
public:
	Vector3 
		origin,
		normal;

	unsigned char materialIndex;
};

struct Triangle
{
public:
	enum class CullMode
	{
		frontFace,
		backFace,
		none
	};

	const Vector3
		& v0,
		& v1,
		& v2,
		& normal;

	unsigned char materialIndex;
	CullMode cullMode;

};

struct TriangleMesh
{
public:
	TriangleMesh() = default;

	TriangleMesh(unsigned char materialIndex, Triangle::CullMode cullMode = Triangle::CullMode::backFace) :
		vPositions{},
		vNormals{},

		vIndices{},

		materialIndex{ materialIndex },
		cullMode{ cullMode },

		translator{ IDENTITY },
		rotor{ IDENTITY },
		scalar{ IDENTITY }
	{
		CalculateNormals();
		UpdateTransforms();
	}

	TriangleMesh(const std::string& OBJFilePath, unsigned char materialIndex, Triangle::CullMode cullMode = Triangle::CullMode::backFace) :
		vPositions{},
		vNormals{},

		vIndices{},

		materialIndex{ materialIndex },
		cullMode(cullMode),

		translator{ IDENTITY },
		rotor{ IDENTITY },
		scalar{ IDENTITY }
	{
		ParseOBJ(OBJFilePath);
		UpdateTransforms();
	}

	inline void UpdateTransforms(int startingIndex = 0)
	{
		const Matrix finalTransform{ scalar * rotor * translator };

		vPositionsTransformed.resize(vPositions.size());
		for (int index{ startingIndex }; index < vPositions.size(); ++index)
			vPositionsTransformed[index] = finalTransform.TransformPoint(vPositions[index]);

		vNormalsTransformed.resize(vNormals.size());
		for (int index{ startingIndex }; index < vNormals.size(); ++index)
			vNormalsTransformed[index] = finalTransform.TransformVector(vNormals[index]).GetNormalized();
	}

	inline void AppendTriangle(const Triangle& triangle)
	{
		const int startingIndex{ static_cast<int>(vPositions.size()) };

		vPositions.push_back(triangle.v0);
		vPositions.push_back(triangle.v1);
		vPositions.push_back(triangle.v2);

		vIndices.push_back(startingIndex);
		vIndices.push_back(startingIndex + 1);
		vIndices.push_back(startingIndex + 2);

		CalculateNormals();
		UpdateTransforms(startingIndex);
	}

	inline void SetTranslator(const Vector3& _translator)
	{
		this->translator = Matrix::CreateTranslator(_translator);
	}

	inline void SetRotor(float pitch, float yaw, float roll)
	{
		rotor = Matrix::CreateRotor(pitch, yaw, roll);
	}

	inline void SetScalar(float _scalar)
	{
		this->scalar = Matrix::CreateScalar(_scalar);
	}

	std::vector<Vector3>
		vPositionsTransformed,
		vNormalsTransformed;

	std::vector<int> vIndices;

	unsigned char materialIndex;
	Triangle::CullMode cullMode;

private:
	inline void CalculateNormals()
	{
		for (int index{}; index < vIndices.size();)
		{
			const Vector3&
				v0{ vPositions[vIndices[index++]] },
				v1{ vPositions[vIndices[index++]] },
				v2{ vPositions[vIndices[index++]] };

			vNormals.push_back(Vector3::Cross((v1 - v0), (v2 - v0)).GetNormalized());
		}
	}

	inline bool ParseOBJ(const std::string& OBJFilePath)
	{
		std::ifstream file(OBJFilePath);
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
				vPositions.push_back({ x, y, z });
			}
			else if (sCommand == "f")
			{
				float i0, i1, i2;
				file >> i0 >> i1 >> i2;

				vIndices.push_back((int)i0 - 1);
				vIndices.push_back((int)i1 - 1);
				vIndices.push_back((int)i2 - 1);
			}
			//read till end of line and ignore all remaining chars
			file.ignore(1000, '\n');

			if (file.eof())
				break;
		}

		CalculateNormals();

		return true;
	}

	std::vector<Vector3>
		vPositions,
		vNormals;

	Matrix
		translator,
		rotor,
		scalar;
};

struct Light
{
public:
	Vector3 origin;

	float intensity;

	ColorRGB color;
};

static constexpr float RAY_EPSILON{ 0.001f };
struct Ray
{
public:
	Vector3 
		origin,
		direction;

	float 
		min{ RAY_EPSILON },
		max{ FLT_MAX };
};

struct HitRecord
{
public:
	Vector3 
		origin,
		normal;

	float t{ FLT_MAX };
	bool didHit{};

	unsigned char materialIndex;
};