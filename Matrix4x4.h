#pragma once
#include <array>
#include <cmath>
#include <iostream>
#include <string>

#include <sstream>

#include "MeehanVector.h"

namespace CommonUtilities
{
	template <class T>
	class Quaternion;

	template <class T>
	class Matrix4x4
	{
	public:
		// Creates the identity matrix.
		Matrix4x4<T>();

		// Creates the identity matrix.
		Matrix4x4<T>(T anX,T aY,T aZ,T aW);

		// Copy Constructor.
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		void operator= (const Matrix4x4<T> aMatrix);
		bool operator== (const Matrix4x4<T> aMatrix) const;
		Matrix4x4<T> operator*(const Matrix4x4& aMatrix4x4) const;
		Matrix4x4<float> GetRotationMatrix();

		// Static functions for creating rotation matrices.
		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> CreateRotation(T aAngleInRadians, const Vector3<T>& aRotationAxis);
		static Matrix4x4<T> CreateRotation(T anX,T aY, T aZ);

		// + and += operators for 4x4 matrices
		Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix);
		void operator+=(const Matrix4x4<T>& aMatrix);

		// - and -= operators for 4x4 matrices
		Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix);
		void operator-=(const Matrix4x4<T>& aMatrix);

		// * and *= operators for 4x4 matrices
		Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix);
		void operator*=(const Matrix4x4<T>& aMatrix);

        // Getters for the values of the matrix.
        Vector4<T> GetTranslationV4() const
        {
			return Vector4<T>(myValues[0][3], myValues[1][3], myValues[2][3], myValues[3][3]);
        }

        // Getters for the values of the matrix.
        Vector3<T> GetTranslationV3() const
        {
			return Vector3<T>(myValues[0][3], myValues[1][3], myValues[2][3]);
        }

        // Getters for the values of the matrix.
        Vector2<T> GetTranslationV2() const
        {
			return Vector2<T>(myValues[0][3], myValues[1][3]);
        }

		// Get forward vector from the matrix.
		Vector3<T> GetForward() const
		{
			return Vector3<T>(myValues[2][0], myValues[2][1], myValues[2][2]).GetNormalized();
		}

		// Get right vector from the matrix.
		Vector3<T> GetRight() const
		{
			return Vector3<T>(myValues[0][0], myValues[0][1], myValues[0][2]).GetNormalized();
		}

		std::array<std::array<T, 4>, 4> GetValues()
		{
			return myValues;
		}

		CommonUtilities::Quaternion<T> GetRotation()
		{
			CommonUtilities::Quaternion<T> temp;
			temp.SetFromMatrix(*this);
			return temp;
		}

		void SetValues(std::array<std::array<T, 4>, 4>& someValues) const
		{
			myValues = someValues;
		}

		void SetTranslation(const Vector3<T>& aTranslation)
		{
			myValues[0][3] = aTranslation.x;
			myValues[1][3] = aTranslation.y;
			myValues[2][3] = aTranslation.z;
		}

		void SetRotation(CommonUtilities::Quaternion<T> aRotation)
		{
			Matrix4x4<T> temp = aRotation.GetMatrix();
			myValues[0][0] = temp.GetValues()[0][0];
			myValues[0][1] = temp.GetValues()[0][1];
			myValues[0][2] = temp.GetValues()[0][2];
			myValues[1][0] = temp.GetValues()[1][0];
			myValues[1][1] = temp.GetValues()[1][1];
			myValues[1][2] = temp.GetValues()[1][2];
			myValues[2][0] = temp.GetValues()[2][0];
			myValues[2][1] = temp.GetValues()[2][1];
			myValues[2][2] = temp.GetValues()[2][2];
		}

		void SetRotationMatrix(CommonUtilities::Matrix4x4<T> aMatrix)
        {
			myValues[0][0] = aMatrix.GetValues()[0][0];
			myValues[0][1] = aMatrix.GetValues()[0][1];
			myValues[0][2] = aMatrix.GetValues()[0][2];
			myValues[1][0] = aMatrix.GetValues()[1][0];
			myValues[1][1] = aMatrix.GetValues()[1][1];
			myValues[1][2] = aMatrix.GetValues()[1][2];
			myValues[2][0] = aMatrix.GetValues()[2][0];
			myValues[2][1] = aMatrix.GetValues()[2][1];
			myValues[2][2] = aMatrix.GetValues()[2][2];
        }

		void SetScale(const Vector3<T>& aScale)
		{
			myValues[0][0] = aScale.x;
			myValues[1][1] = aScale.y;
			myValues[2][2] = aScale.z;
		}

		void SetPosition(const Vector3<T>& aPosition)
		{
			myValues[0][3] = aPosition.x;
			myValues[1][3] = aPosition.y;
			myValues[2][3] = aPosition.z;
		}

		void SetPosition(const Vector4<T>& aPosition)
		{
			myValues[0][3] = aPosition.x;
			myValues[1][3] = aPosition.y;
			myValues[2][3] = aPosition.z;
			myValues[3][3] = aPosition.w;
		}

		Matrix4x4<T> GetTransform() const
		{
			return *this;
		}

		Matrix4x4<T> Reflect(float aPlaneHeight) const
		{
			Matrix4x4<T> identityMatrix = Matrix4x4<T>(); // Initialize as identity matrix

			// Construct reflection matrix
			identityMatrix(2, 2) = -1.0f; // Invert Y-axis
			identityMatrix(4, 2) = 2.0f * aPlaneHeight;

			// Combine with current transformation
			return *this * identityMatrix;
		}
		Matrix4x4<T> ReflectUsingNormal(const Vector3<T>& aNormal, float aDistance) const
		{
			T a = aNormal.x, b = aNormal.y, c = aNormal.z;
			Matrix4x4<T> reflectionMatrix;

			reflectionMatrix(1, 1) = 1 - 2 * a * a;
			reflectionMatrix(1, 2) = -2 * a * b;
			reflectionMatrix(1, 3) = -2 * a * c;
			reflectionMatrix(1, 4) = -2 * a * aDistance;

			reflectionMatrix(2, 1) = -2 * b * a;
			reflectionMatrix(2, 2) = 1 - 2 * b * b;
			reflectionMatrix(2, 3) = -2 * b * c;
			reflectionMatrix(2, 4) = -2 * b * aDistance;

			reflectionMatrix(3, 1) = -2 * c * a;
			reflectionMatrix(3, 2) = -2 * c * b;
			reflectionMatrix(3, 3) = 1 - 2 * c * c;
			reflectionMatrix(3, 4) = -2 * c * aDistance;

			reflectionMatrix(4, 1) = 0;
			reflectionMatrix(4, 2) = 0;
			reflectionMatrix(4, 3) = 0;
			reflectionMatrix(4, 4) = 1;

			return reflectionMatrix * *this;
		}
		operator std::string() const
		{
			std::string result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result += std::to_string(myValues[i][j]) + " ";
				}
				result += "\n";
			}
			return result;
		}

		// Static function for creating a transpose of a matrix.
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
		//static Matrix4x4<T> CreateProjectionMatrixLH(T aNearZ, T aFarZ, T aAspectRatio, T aFovAngle);
		//static Matrix4x4<T> CreateOrthographicMatrixLH(T aWidth, T aHeight, T aNearZ, T aFarZ);
		//static Matrix4x4<T> CreateLookAtMatrixLH(const Vector4<T>& aEye, const Vector4<T>& aTarget, const Vector4<T>& aUp);
		static Matrix4x4<T> CreateScaleMatrix(T aXScale, T aYScale, T aZScale);
		static Matrix4x4<T> CreateTranslationMatrix(T aX, T aY, T aZ);

		// Static function for Adding and Subtracting two matrices
		static Matrix4x4<T> Add(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& aSecondMatrix);
		static Matrix4x4<T> Subtract(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& aSecondMatrix);

		static Matrix4x4<T> Multiply(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& aSecondMatrix);
		static Vector4<T> Multiply(const Matrix4x4<T>& aMatrix, const Vector4<T>& aVector);

	private:
		std::array<std::array<T, 4>, 4> myValues;
	};

	template <typename T>
	Matrix4x4<T> CalculateModelMatrix(const Vector3<T>& aPosition, const Quaternion<T>& aRotation, const Vector3<T>& aScale) {
		// Start with identity matrix
		Matrix4x4<T> modelMatrix;

		// Set translation in the model matrix
		modelMatrix.SetPosition(aPosition);

		// Set rotation in the model matrix
		modelMatrix.SetRotation(aRotation);

		// Set scaling in the model matrix
		modelMatrix.SetScale(aScale);

		return modelMatrix;
	}

	template <typename T>
	void PrintTransform(const Matrix4x4<T>& aTransform)
	{
		std::ostringstream oss;
		for (int i = 1; i <= 4; ++i)
		{
			for (int j = 1; j <= 4; ++j)
			{
				oss << aTransform(i, j) << " ";
			}
			oss << std::endl;
		}
		OutputDebugStringA(oss.str().c_str());
	}

	template <typename T>
	Matrix4x4<T> LookAt(const Vector3<T>& anEye, const Vector3<T>& aTarget, const Vector3<T>& anUpVector) {
		Vector3<T> zAxis = (aTarget - anEye).Normalized();
		Vector3<T> xAxis = anUpVector.Cross(zAxis).Normalized();
		Vector3<T> yAxis = zAxis.Cross(xAxis);

		Matrix4x4<T> lookAtMatrix;

		lookAtMatrix(1, 1) = xAxis.x;
		lookAtMatrix(1, 2) = xAxis.y;
		lookAtMatrix(1, 3) = xAxis.z;

		lookAtMatrix(2, 1) = yAxis.x;
		lookAtMatrix(2, 2) = yAxis.y;
		lookAtMatrix(2, 3) = yAxis.z;

		lookAtMatrix(3, 1) = zAxis.x;
		lookAtMatrix(3, 2) = zAxis.y;
		lookAtMatrix(3, 3) = zAxis.z;

		lookAtMatrix(1, 4) = -xAxis.Dot(anEye);
		lookAtMatrix(2, 4) = -yAxis.Dot(anEye);
		lookAtMatrix(3, 4) = -zAxis.Dot(anEye);

		return lookAtMatrix;
	}

	template <typename T>
	Matrix4x4<T> Perspective(T fov, T aspect, T nearPlane, T farPlane) {
		T tanHalfFov = std::tan(fov / 2.0);

		Matrix4x4<T> perspectiveMatrix;
		perspectiveMatrix(1, 1) = 1.0 / (aspect * tanHalfFov);
		perspectiveMatrix(2, 2) = 1.0 / tanHalfFov;
		perspectiveMatrix(3, 3) = farPlane / (farPlane - nearPlane);
		perspectiveMatrix(3, 4) = -(farPlane * nearPlane) / (farPlane - nearPlane);
		perspectiveMatrix(4, 3) = 1.0;
		perspectiveMatrix(4, 4) = 0.0;

		return perspectiveMatrix;
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
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
	inline CommonUtilities::Matrix4x4<T>::Matrix4x4(T anX, T aY, T aZ, T aW)
	{
		myValues[0][0] = anX;
		myValues[1][1] = aY;
		myValues[2][2] = aZ;
		myValues[3][3] = aW;
	}


	template<class T>
	std::ostream& operator<<(std::ostream& anOStream, CommonUtilities::Matrix4x4<T>& aMatrix)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				anOStream << aMatrix.GetValues()[i][j] << " ";
			}
			anOStream << "\n";
		}
		return anOStream;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		myValues = aMatrix.myValues;
	}

	template<class T>
	inline T& CommonUtilities::Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		return myValues[aRow - 1][aColumn - 1];
	}

	template<class T>
	inline const T& CommonUtilities::Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		return myValues[aRow - 1][aColumn - 1];
	}

	template<class T>
	inline void CommonUtilities::Matrix4x4<T>::operator=(const Matrix4x4<T> aMatrix)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				myValues[i][j] = aMatrix.myValues[i][j];
			}
		}
	}

	template<class T>
	inline bool CommonUtilities::Matrix4x4<T>::operator==(const Matrix4x4<T> aMatrix) const
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
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
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix4x4) const
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		Vector4<T> tempVector;
		Vector4<T> tempVector2;
		for (int i = 0; i < 4; i++)
		{
			tempVector.x = myValues[i][0];
			tempVector.y = myValues[i][1];
			tempVector.z = myValues[i][2];
			tempVector.w = myValues[i][3];
			for (int j = 0; j < 4; j++)
			{
				tempVector2.x = aMatrix4x4.myValues[0][j];
				tempVector2.y = aMatrix4x4.myValues[1][j];
				tempVector2.z = aMatrix4x4.myValues[2][j];
				tempVector2.w = aMatrix4x4.myValues[3][j];

				temp.myValues[i][j] = tempVector2.Dot(tempVector);
			}
		}
		return temp;
	}

	template <class T>
	Matrix4x4<float> Matrix4x4<T>::GetRotationMatrix()
	{
		Matrix4x4<float> temp = Matrix4x4<float>();
		temp.GetValues()[0][0] = myValues[0][0];
		temp.GetValues()[0][1] = myValues[0][1];
		temp.GetValues()[0][2] = myValues[0][2];
		temp.GetValues()[1][0] = myValues[1][0];
		temp.GetValues()[1][1] = myValues[1][1];
		temp.GetValues()[1][2] = myValues[1][2];
		temp.GetValues()[2][0] = myValues[2][0];
		temp.GetValues()[2][1] = myValues[2][1];
		temp.GetValues()[2][2] = myValues[2][2];
		return temp;
	}


	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		temp.myValues[0][0] = 1;
		temp.myValues[3][3] = 1;
		temp.myValues[1][1] = cos(aAngleInRadians);
		temp.myValues[2][2] = cos(aAngleInRadians);
		temp.myValues[1][2] = sin(aAngleInRadians);
		temp.myValues[2][1] = -sin(aAngleInRadians);
		return temp;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		temp.myValues[0][0] = cos(aAngleInRadians);
		temp.myValues[2][2] = cos(aAngleInRadians);
		temp.myValues[2][0] = sin(aAngleInRadians);
		temp.myValues[0][2] = -sin(aAngleInRadians);
		temp.myValues[1][1] = 1;
		temp.myValues[3][3] = 1;
		return temp;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		temp.myValues[2][2] = 1;
		temp.myValues[3][3] = 1;
		temp.myValues[0][0] = cos(aAngleInRadians);
		temp.myValues[1][1] = cos(aAngleInRadians);
		temp.myValues[0][1] = sin(aAngleInRadians);
		temp.myValues[1][0] = -sin(aAngleInRadians);
		return temp;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotation(T aAngleInRadians, const Vector3<T>& aRotationAxis)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		T c = cos(aAngleInRadians);
		T s = sin(aAngleInRadians);
		T t = 1 - c;
		Vector3<T> axis = aRotationAxis.GetNormalized();
		temp.myValues[0][0] = t * axis.x * axis.x + c;
		temp.myValues[0][1] = t * axis.x * axis.y + s * axis.z;
		temp.myValues[0][2] = t * axis.x * axis.z - s * axis.y;
		temp.myValues[1][0] = t * axis.x * axis.y - s * axis.z;
		temp.myValues[1][1] = t * axis.y * axis.y + c;
		temp.myValues[1][2] = t * axis.y * axis.z + s * axis.x;
		temp.myValues[2][0] = t * axis.x * axis.z + s * axis.y;
		temp.myValues[2][1] = t * axis.y * axis.z - s * axis.x;
		temp.myValues[2][2] = t * axis.z * axis.z + c;
		temp.myValues[3][3] = 1;
		return temp;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotation(T anX, T aY, T aZ)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		temp.myValues[0][0] = cos(aY) * cos(aZ);
		temp.myValues[0][1] = cos(aY) * sin(aZ);
		temp.myValues[0][2] = -sin(aY);
		temp.myValues[1][0] = sin(anX) * sin(aY) * cos(aZ) - cos(anX) * sin(aZ);
		temp.myValues[1][1] = sin(anX) * sin(aY) * sin(aZ) + cos(anX) * cos(aZ);
		temp.myValues[1][2] = sin(anX) * cos(aY);
		temp.myValues[2][0] = cos(anX) * sin(aY) * cos(aZ) + sin(anX) * sin(aZ);
		temp.myValues[2][1] = cos(anX) * sin(aY) * sin(aZ) - sin(anX) * cos(aZ);
		temp.myValues[2][2] = cos(anX) * cos(aY);
		temp.myValues[3][3] = 1;
		return temp;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix)
	{
		return Matrix4x4<T>::Add(*this, aMatrix);
	}

	template<class T>
	inline void CommonUtilities::Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		*this = *this + aMatrix;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix)
	{
		return Matrix4x4<T>::Subtract(*this, aMatrix);
	}

	template<class T>
	inline void CommonUtilities::Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		*this = *this - aMatrix;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix)
	{
		return Matrix4x4<T>().Multiply(*this, aMatrix);
	}

	template<class T>
	inline void CommonUtilities::Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		*this = *this * aMatrix;
	}

	template<class T>
	inline CommonUtilities::Vector4<T> operator*(const Matrix4x4<T>& aMatrix, const Vector4<T>& aVector)
	{
		return Matrix4x4<T>().Multiply(aMatrix, aVector);
	}

	template<class T>
	inline CommonUtilities::Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		return Matrix4x4<T>().Multiply(aMatrix, aVector);
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp.myValues[i][j] = aMatrixToTranspose.myValues[j][i];
			}
		}
		return temp;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::CreateTranslationMatrix(T aX, T aY, T aZ)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		temp.myValues[3][0] = aX;
		temp.myValues[3][1] = aY;
		temp.myValues[3][2] = aZ;
		return temp;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::Add(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& aSecondMatrix)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp.myValues[i][j] = aMatrix.myValues[i][j] + aSecondMatrix.myValues[i][j];
			}
		}
		return temp;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::Subtract(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& aSecondMatrix)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp.myValues[i][j] = aMatrix.myValues[i][j] - aSecondMatrix.myValues[i][j];
			}
		}
		return temp;
	}

	template<class T>
	inline CommonUtilities::Matrix4x4<T> CommonUtilities::Matrix4x4<T>::Multiply(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& aSecondMatrix)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		Vector4<T> tempVector = Vector4<T>();
		Vector4<T> tempVector2 = Vector4<T>();
		for (int i = 0; i < 4; i++)
		{
			tempVector.x = aMatrix.myValues[i][0];
			tempVector.y = aMatrix.myValues[i][1];
			tempVector.z = aMatrix.myValues[i][2];
			tempVector.w = aMatrix.myValues[i][3];
			for (int j = 0; j < 4; j++)
			{
				tempVector2.x = aSecondMatrix.myValues[0][j];
				tempVector2.y = aSecondMatrix.myValues[1][j];
				tempVector2.z = aSecondMatrix.myValues[2][j];
				tempVector2.w = aSecondMatrix.myValues[3][j];

				temp.myValues[i][j] = tempVector2.Dot(tempVector);
			}
		}
		return temp;
	}

	template<class T>
	inline CommonUtilities::Vector4<T> CommonUtilities::Matrix4x4<T>::Multiply(const Matrix4x4<T>& aMatrix, const Vector4<T>& aVector)
	{
		Vector4<T> temp = Vector4<T>();

		temp.x = Vector4<T>(aMatrix.myValues[0][0], aMatrix.myValues[1][0], aMatrix.myValues[2][0], aMatrix.myValues[3][0]).Dot(aVector);
		temp.y = Vector4<T>(aMatrix.myValues[0][1], aMatrix.myValues[1][1], aMatrix.myValues[2][1], aMatrix.myValues[3][1]).Dot(aVector);
		temp.z = Vector4<T>(aMatrix.myValues[0][2], aMatrix.myValues[1][2], aMatrix.myValues[2][2], aMatrix.myValues[3][2]).Dot(aVector);
		temp.w = Vector4<T>(aMatrix.myValues[0][3], aMatrix.myValues[1][3], aMatrix.myValues[2][3], aMatrix.myValues[3][3]).Dot(aVector);
		return temp;
	}

	//----------------------------------------------------------------


	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		Matrix4x4<T> temp(aTransform);

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				temp.myValues[i][j] = aTransform.myValues[j][i];
			}
		}

		T x = -temp.myValues[3][0];
		T y = -temp.myValues[3][1];
		T z = -temp.myValues[3][2];

		temp.myValues[3][0] = x * temp.myValues[0][0] + y * temp.myValues[1][0] + z * temp.myValues[2][0];
		temp.myValues[3][1] = x * temp.myValues[0][1] + y * temp.myValues[1][1] + z * temp.myValues[2][1];
		temp.myValues[3][2] = x * temp.myValues[0][2] + y * temp.myValues[1][2] + z * temp.myValues[2][2];

		return temp;
	}

	template <class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(T aXScale, T aYScale, T aZScale)
	{
		Matrix4x4<T> temp = Matrix4x4<T>();
		temp.myValues[0][0] = aXScale;
		temp.myValues[1][1] = aYScale;
		temp.myValues[2][2] = aZScale;
		return temp;
	}

	template <class T>
	inline bool Inverse(Matrix4x4<T>& aOutMatrix, const Matrix4x4<T>& aMatrix)
	{
		// Create an augmented matrix with the original matrix on the left and the identity matrix on the right
		T augmentedMatrix[4][8]; // 4x8 matrix (4x4 for the original matrix, 4x4 for the identity matrix)

		// Initialize the augmented matrix with aMatrix and identity matrix
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				augmentedMatrix[row][col] = aMatrix(row + 1, col + 1);
				augmentedMatrix[row][col + 4] = (row == col) ? 1 : 0;
			}
		}

		// Perform Gauss-Jordan elimination
		for (int col = 0; col < 4; col++)
		{
			// Find the pivot element (maximum absolute value in the column)
			int pivotRow = col;
			for (int row = col + 1; row < 4; row++)
			{
				if (std::abs(augmentedMatrix[row][col]) > std::abs(augmentedMatrix[pivotRow][col]))
				{
					pivotRow = row;
				}
			}

			// Swap the current row with the pivot row
			if (pivotRow != col)
			{
				for (int k = 0; k < 8; k++)
				{
					std::swap(augmentedMatrix[col][k], augmentedMatrix[pivotRow][k]);
				}
			}

			// Make sure the pivot element is non-zero
			T pivotElement = augmentedMatrix[col][col];
			if (std::abs(pivotElement) < std::numeric_limits<T>::epsilon())
			{
				// The matrix is singular and cannot be inverted
				return false;
			}

			// Normalize the pivot row
			for (int k = 0; k < 8; k++)
			{
				augmentedMatrix[col][k] /= pivotElement;
			}

			// Eliminate the other rows
			for (int row = 0; row < 4; row++)
			{
				if (row != col)
				{
					T factor = augmentedMatrix[row][col];
					for (int k = 0; k < 8; k++)
					{
						augmentedMatrix[row][k] -= factor * augmentedMatrix[col][k];
					}
				}
			}
		}

		// Extract the inverse matrix from the augmented matrix
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				aOutMatrix(row + 1, col + 1) = augmentedMatrix[row][col + 4];
			}
		}

		return true;
	}
}
