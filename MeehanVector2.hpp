#pragma once

namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;

		//Creates a null-vector
		Vector2<T>()
			: x(0), y(0)
		{};

		//Creates a vector (aX, aY, aZ)
		Vector2<T>(const T& aX, const T& aY)
			: x(aX), y(aY)
		{};

		//Copy constructor (compiler generated)
		Vector2<T>(const Vector2<T>& aVector2) = default;

		//Assignment operator (compiler generated)
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		// Equality operator overload
		bool operator==(const Vector2<T>& other) const
		{
			return x == other.x && y == other.y;
		}

		// Returns the vector product of this and another vector (component-wise multiplication)
		Vector2<T> operator*(const Vector2<T>& aVector)
		{
			return Vector2<T>(x * aVector.x, y * aVector.y);
		}

		//Destructor (compiler generated)
		~Vector2<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			T result = (x*x) + (y*y);

			return result;
		}

		//Returns the length of the vector
		T Length() const
		{
			return sqrt(LengthSqr());
		}

		//Returns a normalized copy of this
		Vector2<T> GetNormalized() const
		{
			T tempLength = Length();
			if (tempLength == 0)
			{
				return Vector2<T>();
			}
			return Vector2<T>((x / tempLength), (y / tempLength));
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
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector2<T>& aVector) const
		{
			T product = ((x * aVector.x) + (y * aVector.y));
			return product;
		}

		// Rotates the vector by a given angle in degrees
		Vector2<T> GetRotated(const T& angle)
		{
			// Angle to radians
			T radianAngle = angle * static_cast<T>(3.14159265359 / 180.0);

			// Cosine and sine of the angle
			T cosA = cos(radianAngle);
			T sinA = sin(radianAngle);

			// Rotation matrix
			return Vector2<T>(x * cosA - y * sinA, x * sinA + y * cosA);
		}
	};

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1) 
	{
		Vector2<T> copy(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
		return copy;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1) 
	{
		Vector2<T> copy(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar) 
	{
		Vector2<T> copy(aVector.x * aScalar, aVector.y * aScalar);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector) 
	{
		Vector2<T> copy(aVector.x * aScalar, aVector.y * aScalar);
		return copy;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar) 
	{
		Vector2<T> copy(aVector.x *= (1 / aScalar), aVector.y *= (1 / aScalar));
		return copy;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1) 
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1) 
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar) 
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar) 
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
	}
}
