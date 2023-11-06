#pragma once

#include "Vector4.hpp"
#include "Vector3.hpp"

class Matrix
{
public:
	Matrix() = default;

	Matrix(const Vector4& xAxis, const Vector4& yAxis, const Vector4& zAxis, const Vector4& translator) :
		m_Data{ xAxis, yAxis, zAxis, translator }
	{
	}

	inline Vector3 TransformVector(float x, float y, float z) const
	{
		return Vector3
		(
			m_Data[0].x * x + m_Data[1].x * y + m_Data[2].x * z,
			m_Data[0].y * x + m_Data[1].y * y + m_Data[2].y * z,
			m_Data[0].z * x + m_Data[1].z * y + m_Data[2].z * z
		);
	}

	inline Vector3 TransformVector(const Vector3& vector) const
	{
		return TransformVector(vector.x, vector.y, vector.z);
	}

	inline Vector3 TransformPoint(float x, float y, float z) const
	{
		return Vector3
		(
			m_Data[0].x * x + m_Data[1].x * y + m_Data[2].x * z + m_Data[3].x,
			m_Data[0].y * x + m_Data[1].y * y + m_Data[2].y * z + m_Data[3].y,
			m_Data[0].z * x + m_Data[1].z * y + m_Data[2].z * z + m_Data[3].z
		);
	}

	inline Vector3 TransformPoint(const Vector3& point) const
	{
		return TransformPoint(point.x, point.y, point.z);
	}

	inline Matrix GetTransposed() const
	{
		Matrix result;
		for (int currentRow{}; currentRow < 4; ++currentRow)
			for (int currentColumn{}; currentColumn < 4; ++currentColumn)
				result[currentRow][currentColumn] = m_Data[currentColumn][currentRow];;

		return result;
	}

	inline const Matrix& Transpose()
	{
		*this = GetTransposed();
		return *this;
	}

	static inline Matrix CreateTranslator(const Vector3& translator)
	{
		return Matrix
		(
			VECTOR4_UNIT_X,
			VECTOR4_UNIT_Y,
			VECTOR4_UNIT_Z,
			translator.GetPoint4()
		);
	}

	static inline Matrix CreateTranslator(float x, float y, float z)
	{
		return CreateTranslator(Vector3(x, y, z));
	}

	static inline Matrix CreateRotorX(float pitch)
	{
		const float
			cosine{ cosf(pitch) },
			sine{ sinf(pitch) };

		return
			Matrix
			(
				VECTOR4_UNIT_X,
				Vector4(0.0f, cosine, sine, 0.0f),
				Vector4(0.0f, -sine, cosine, 0.0f),
				VECTOR4_UNIT_T
			);
	}

	static inline Matrix CreateRotorY(float yaw)
	{
		const float
			cosine{ cosf(yaw) },
			sine{ sinf(yaw) };

		return
			Matrix
			(
				Vector4(cosine, 0.0f, -sine, 0.0f),
				VECTOR4_UNIT_Y,
				Vector4(sine, 0.0f, cosine, 0.0f),
				VECTOR4_UNIT_T
			);
	}

	static inline Matrix CreateRotorZ(float roll)
	{
		const float
			cosine{ cosf(roll) },
			sine{ sinf(roll) };

		return
			Matrix
			(
				Vector4(cosine, sine, 0.0f, 0.0f),
				Vector4(-sine, cosine, 0.0f, 0.0f),
				VECTOR4_UNIT_Z,
				VECTOR4_UNIT_T
			);

	}

	static inline Matrix CreateRotor(float pitch, float yaw, float roll)
	{
		return
			Matrix::CreateRotorZ(roll) *
			Matrix::CreateRotorY(yaw) *
			Matrix::CreateRotorX(pitch);
	}

	static inline Matrix CreateScalar(float scalarX, float scalarY, float scalarZ)
	{
		return
			Matrix
			(
				Vector4(scalarX, 0.0f, 0.0f, 0.0f),
				Vector4(0.0f, scalarY, 0.0f, 0.0f),
				Vector4(0.0f, 0.0f, scalarZ, 0.0f),
				VECTOR4_UNIT_T
			);
	}

	static inline Matrix CreateScalar(float scalar)
	{
		return CreateScalar(scalar, scalar, scalar);
	}

	inline Matrix operator*(const Matrix& matrix) const
	{
		Matrix result;
		const Matrix transposedMatrix{ matrix.GetTransposed() };

		for (int currentRow{}; currentRow < 4; ++currentRow)
			for (int currentColumn{}; currentColumn < 4; ++currentColumn)
				result[currentRow][currentColumn] = Vector4::Dot(m_Data[currentRow], transposedMatrix[currentColumn]);

		return result;
	}

	inline const Matrix& operator*=(const Matrix& matrix)
	{
		*this = *this * matrix;
		return *this;
	}

	inline bool operator==(const Matrix& matrix) const
	{
		for (int index{}; index < 4; ++index)
			if (m_Data[index] != matrix[index])
				return false;

		return true;
	}

	inline Vector4& operator[](int index)
	{
		return m_Data[index];
	}

	inline const Vector4& operator[](int index) const
	{
		return m_Data[index];
	}

private:
	Vector4 m_Data[4];
};

static const Matrix IDENTITY
{
	VECTOR4_UNIT_X,
	VECTOR4_UNIT_Y,
	VECTOR4_UNIT_Z,
	VECTOR4_UNIT_T
};