#pragma once

namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		//Creates a null-vector
		Vector3<T>()
			: x(0), y(0), z(0)
		{};

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const T& aX, const T& aY, const T& aZ)
			: x(aX), y(aY), z(aZ)
		{};

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector3) = default;

		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		//Destructor (compiler generated)
		~Vector3<T>() = default;

		bool IsZero() const
		{
			return x == 0 && y == 0 && z == 0;
		}

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			T result = ((x * x) + (y * y) + (z * z));

			return result;
		}

		//Returns the length of the vector
		T Length() const
		{
			return sqrt(LengthSqr());
		}

		//Returns a normalized copy of this
		Vector3<T> GetNormalized() const
		{
			T tempLength = Length();
			if (tempLength == 0)
			{
				return Vector3<T>();
			}
			return Vector3<T>((x / tempLength), (y / tempLength), (z / tempLength));
		}

		//Normalizes the vector
		void Normalize()
		{
			T tempLength = Length();
			if (x != 0)
			{
				x /= tempLength;
			}
			else
			{
				x = 0.0f;
			}
			if (y != 0)
			{
				y /= tempLength;
			}
			else
			{
				y = 0.0f;
			}
			if (y != 0)
			{
				z /= tempLength;
			}
			else
			{
				z = 0.0f;
			}
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const
		{
			T product = ((x * aVector.x) + (y * aVector.y) + (z * aVector.z));
			return product;
		}

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const
		{
			Vector3 vector3;
			vector3.x = (y * aVector.z - z * aVector.y);
			vector3.y = (z * aVector.x - x * aVector.z);
			vector3.z = (x * aVector.y - y * aVector.x);
			return vector3;
		}

		// Rotates the vector by given angles in degrees around each axis
		Vector3<T> GetRotated(const T& angleX, const T& angleY, const T& angleZ) const
		{
			// Angles to radians
			T radianAngleX = angleX * static_cast<T>(3.14159265359 / 180.0);
			T radianAngleY = angleY * static_cast<T>(3.14159265359 / 180.0);
			T radianAngleZ = angleZ * static_cast<T>(3.14159265359 / 180.0);

			// Cosines and sines of the angles
			T cosX = cos(radianAngleX);
			T sinX = sin(radianAngleX);
			T cosY = cos(radianAngleY);
			T sinY = sin(radianAngleY);
			T cosZ = cos(radianAngleZ);
			T sinZ = sin(radianAngleZ);

			// Rotation matrices
			T newX = x * (cosY * cosZ) + y * (cosX * sinZ + sinX * sinY * cosZ) + z * (sinX * sinZ - cosX * sinY * cosZ);
			T newY = -x * (cosY * sinZ) + y * (cosX * cosZ - sinX * sinY * sinZ) + z * (sinX * cosZ + cosX * sinY * sinZ);
			T newZ = x * sinY + y * (-sinX * cosY) + z * (cosX * cosY);

			return Vector3<T>(newX, newY, newZ);
		}
	};

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> copy(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
		return copy;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> copy(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> copy(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		Vector3<T> copy(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
		return copy;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> copy(aVector.x *= (1 / aScalar), aVector.y *= (1 / aScalar), aVector.z *= (1 / aScalar));
		return copy;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
	}
}
