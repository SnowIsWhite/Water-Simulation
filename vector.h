#pragma once
#include <math.h>

template <class T>
class VECTOR {
public:
	T x, y, z;
	VECTOR<T>() {
		x = 0;
		y = 0;
		z = 0;
	}
	VECTOR<T>(T a, T b, T c) {
		x = a;
		y = b;
		z = c;
	}
	~VECTOR<T>() {}
	void VECTOR<T>::update(T a, T b, T c){
		x = a;
		y = b;
		z = c;
	}
	T VECTOR<T>::Magnitude() {
		return sqrt(x*x + y*y + z*z);
	}
	T VECTOR<T>::InnerProduct(VECTOR<T> v) {
		return (x*v.x + y*v.y + z*v.z);
	}
	VECTOR<T> VECTOR<T>::CrossProduct(VECTOR<T> v) {
		VECTOR<T> result;
		result.x = y*v.z - z*v.y;
		result.y = z*v.x - x*v.z;
		result.z = x*v.y - y*v.x;
		return result;
	}
	void VECTOR<T>::Normalize() {
		double w = Magnitude();
		if (w < 0.00001) return;
		x /= w;
		y /= w;
		z /= w;
	};
	VECTOR<T> VECTOR<T>::UnitVector(VECTOR<T> v){
		VECTOR<T> result;
		result.x = v.x - x;
		result.y = v.y - y;
		result.z = v.z - z;
		double unit = sqrt(result.x*result.x + result.y*result.y + result.z*result.z);
		result.x = result.x / unit;
		result.y = result.y / unit;
		result.z = result.z / unit;
		return result;
	}

	T VECTOR<T>::distance(VECTOR<T> v){
		VECTOR<T> result;
		result.x = v.x - x;
		result.y = v.y - y;
		result.z = v.z - z;
		return sqrt(result.x*result.x + result.y*result.y + result.z*result.z);
	}
	friend VECTOR operator * (VECTOR v, double val) {
		v.x *= val;
		v.y *= val;
		v.z *= val;
		return v;
	}
	friend VECTOR operator * (double val, VECTOR v) {
		v.x *= val;
		v.y *= val;
		v.z *= val;
		return v;
	}

	VECTOR<T> operator + (VECTOR<T> v) {
		VECTOR<T> result = (*this);
		result.x += v.x;
		result.y += v.y;
		result.z += v.z;
		return result;
	}

	VECTOR<T> operator - (VECTOR<T> v) {
		VECTOR<T> result = (*this);
		result.x -= v.x;
		result.y -= v.y;
		result.z -= v.z;
		return result;
	}

	VECTOR<T> operator * (double val) {
		VECTOR<T> result = (*this);
		result.x *= val;
		result.y *= val;
		result.z *= val;
		return result;
	}

	VECTOR<T> operator / (int val) {
		VECTOR<T> result = (*this);
		result.x /= val;
		result.y /= val;
		result.z /= val;
		return result;
	}
	VECTOR<T> operator / (double val){
		VECTOR<T> result = (*this);
		result.x /= val;
		result.y /= val;
		result.z /= val;
		return result;
	}
};
