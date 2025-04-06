#pragma once
#include <array>
#include <cmath>

#include "MeehanVector.h"


namespace CommonUtilities
{
	
	template <class T>
	class Matrix3x3
	{
	public:
		// Creates the identity matrix.
		Matrix3x3<T>();

		// Copy Constructor.
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);

		// Copies the top left 3x3 part of the Matrix4x4.
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		void operator= (const Matrix3x3<T> aMatrix);

		bool operator== (const Matrix3x3<T> aMatrix) const;

		// + and += operators for 3x3 matrices
		Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix);
		void operator+=(const Matrix3x3<T>& aMatrix);

		// - and -= operators for 3x3 matrices
		Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix);
		void operator-=(const Matrix3x3<T>& aMatrix);

		// * and *= operators for 3x3 matrices
		Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix);
		void operator*=(const Matrix3x3<T>& aMatrix);

		// Static functions for creating rotation matrices.
		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);

		// Static function for creating a transpose of a matrix.
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

		// Static function for Adding and Subtracting two matrices
		static Matrix3x3<T> Add(const Matrix3x3<T>& aMatrix, const Matrix3x3<T>& aSecondMatrix);
		static Matrix3x3<T> Subtract(const Matrix3x3<T>& aMatrix, const Matrix3x3<T>& aSecondMatrix);

		static Matrix3x3<T> Multiply(const Matrix3x3<T>& aMatrix, const Matrix3x3<T>& aSecondMatrix);
		static Vector3<T> Multiply(const Matrix3x3<T>& aMatrix, const Vector3<T>& aVector);

	private:
		std::array<std::array<T, 3>, 3> myValues;
	};

	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (i == j)
				{
					myValues[i][j] = 1;
				}
				else
				{
					myValues[i][j] = 0;
				}
			}
		}
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix):
		myValues(aMatrix.myValues)
	{}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (int i = 0; i < myValues.size(); i++)
		{
			for (int j = 0; j < myValues.size(); j++)
			{
				myValues[i][j] = aMatrix(i + 1, j + 1);
			}
		}
	}

	template<class T>
	inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		return myValues[aRow - 1][aColumn - 1];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		return myValues[aRow - 1][aColumn - 1];
	}

	template<class T>
	inline void Matrix3x3<T>::operator=(const Matrix3x3<T> aMatrix)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				myValues[i][j] = aMatrix.myValues[i][j];
			}
		}
	}

	template<class T>
	inline bool Matrix3x3<T>::operator==(const Matrix3x3<T> aMatrix) const
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (myValues[i][j] != aMatrix.myValues[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix)
	{
		return Matrix3x3<T>::Add(*this, aMatrix);
	}

	template<class T>
	inline void Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		*this = *this + aMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix)
	{
		return Matrix3x3<T>::Subtract(*this, aMatrix);
	}

	template<class T>
	inline void Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		*this = *this - aMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix)
	{
		return Matrix3x3<T>().Multiply(*this, aMatrix);
	}

	template<class T>
	inline void Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		*this = *this * aMatrix;
	}

	template<class T>
	inline Vector3<T> operator*(const Matrix3x3<T>& aMatrix, const Vector3<T>& aVector)
	{
		return Matrix3x3<T>().Multiply(aMatrix, aVector);
	}

	template<class T>
	inline Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		return Matrix3x3<T>().Multiply(aMatrix, aVector);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> temp = Matrix3x3<T>();
		temp.myValues[0][0] = 1;
		temp.myValues[1][1] = cos(aAngleInRadians);
		temp.myValues[2][2] = cos(aAngleInRadians);
		temp.myValues[1][2] = sin(aAngleInRadians);
		temp.myValues[2][1] = -sin(aAngleInRadians);
		return temp;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3<T> temp = Matrix3x3<T>();
		temp.myValues[0][0] = cos(aAngleInRadians);
		temp.myValues[2][2] = cos(aAngleInRadians);
		temp.myValues[2][0] = sin(aAngleInRadians);
		temp.myValues[0][2] = -sin(aAngleInRadians);
		temp.myValues[1][1] = 1;
		return temp;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3<T> temp = Matrix3x3<T>();
		temp.myValues[2][2] = 1;
		temp.myValues[0][0] = cos(aAngleInRadians);
		temp.myValues[1][1] = cos(aAngleInRadians);
		temp.myValues[0][1] = sin(aAngleInRadians);
		temp.myValues[1][0] = -sin(aAngleInRadians);
		return temp;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> temp = Matrix3x3<T>();
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				temp.myValues[i][j] = aMatrixToTranspose.myValues[j][i];
			}
		}
		return temp;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Add(const Matrix3x3<T>& aMatrix, const Matrix3x3<T>& aSecondMatrix)
	{
		Matrix3x3<T> temp = Matrix3x3<T>();
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				temp.myValues[i][j] = aMatrix.myValues[i][j] + aSecondMatrix.myValues[i][j];
			}
		}
		return temp;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Subtract(const Matrix3x3<T>& aMatrix, const Matrix3x3<T>& aSecondMatrix)
	{
		Matrix3x3<T> temp = Matrix3x3<T>();
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				temp.myValues[i][j] = aMatrix.myValues[i][j] - aSecondMatrix.myValues[i][j];
			}
		}
		return temp;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Multiply(const Matrix3x3<T>& aMatrix, const Matrix3x3<T>& aSecondMatrix)
	{
		Matrix3x3<T> temp = Matrix3x3<T>();
		Vector3<T> tempVector = Vector3<T>();
		Vector3<T> tempVector2 = Vector3<T>();
		for (int i = 0; i < 3; i++)
		{
			tempVector.x = aMatrix.myValues[i][0];
			tempVector.y = aMatrix.myValues[i][1];
			tempVector.z = aMatrix.myValues[i][2];
			for (int j = 0; j < 3; j++)
			{
				tempVector2.x = aSecondMatrix.myValues[0][j];
				tempVector2.y = aSecondMatrix.myValues[1][j];
				tempVector2.z = aSecondMatrix.myValues[2][j];

				temp.myValues[i][j] = tempVector2.Dot(tempVector);
			}
		}
		return temp;
	}

	template<class T>
	inline Vector3<T> Matrix3x3<T>::Multiply(const Matrix3x3<T>& aMatrix, const Vector3<T>& aVector)
	{
		Vector3<T> temp = Vector3<T>();

		temp.x = Vector3<T>(aMatrix.myValues[0][0], aMatrix.myValues[1][0], aMatrix.myValues[2][0]).Dot(aVector);
		temp.y = Vector3<T>(aMatrix.myValues[0][1], aMatrix.myValues[1][1], aMatrix.myValues[2][1]).Dot(aVector);
		temp.z = Vector3<T>(aMatrix.myValues[0][2], aMatrix.myValues[1][2], aMatrix.myValues[2][2]).Dot(aVector);
		return temp;
	}
}