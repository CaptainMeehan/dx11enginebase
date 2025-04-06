#pragma once

namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		//Creates a null-vector
		Vector4<T>()
			: x(0), y(0), z(0), w(0)
		{};

		//Creates a vector (aX, aY, aZ)
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW)
			: x(aX), y(aY), z(aZ), w(aW)
		{};

		//Copy constructor (compiler generated)
		Vector4<T>(const Vector4<T>& aVector4) = default;

		//Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;

		//Destructor (compiler generated)
		~Vector4<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			// Add squared xyz
			T result = ((x * x) + (y * y) + (z * z) + (w * w));

			return result;
		}

		//Returns the length of the vector
		T Length() const
		{
			return sqrt(LengthSqr());
		}

		//Returns a normalized copy of this
		Vector4<T> GetNormalized() const
		{
			T tempLength = Length();
			if (tempLength == 0)
			{
				return Vector4<T>();
			}
			return Vector4((x / tempLength), (y / tempLength), (z / tempLength), (w / tempLength));
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
			if (y != 0)
			{
				w /= tempLength;
			}
			else
			{
				w = 0.0f;
			}
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const
		{
			T product = ((x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w));
			return product;
		}
	};

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> copy(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
		return copy;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> copy(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> copy(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		Vector4<T> copy(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
		return copy;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> copy(aVector.x *= (1 / aScalar), aVector.y *= (1 / aScalar), aVector.z *= (1 / aScalar), aVector.w *= (1 / aScalar));
		return copy;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
		aVector0.w -= aVector1.w;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
	}
}
