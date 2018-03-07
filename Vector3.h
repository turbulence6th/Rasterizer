#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H

#include <iostream>
#include <math.h>

class Vector3 {
public:
	float _data[3];

	friend std::istream &operator>>(std::istream &stream, Vector3 &vertex) {
		return stream >> vertex._data[0] >> vertex._data[1] >> vertex._data[2];
	}

	Vector3 operator+(const Vector3& vector3) const {
		Vector3 result;
		result._data[0] = this->_data[0] + vector3._data[0];
		result._data[1] = this->_data[1] + vector3._data[1];
		result._data[2] = this->_data[2] + vector3._data[2];
		return result;
	}

	Vector3 operator-(const Vector3& vector3) const {
		Vector3 result;
		result._data[0] = this->_data[0] - vector3._data[0];
		result._data[1] = this->_data[1] - vector3._data[1];
		result._data[2] = this->_data[2] - vector3._data[2];
		return result;
	}

	float operator*(const Vector3& vector3) const {
		float result = 0;
		result += this->_data[0] * vector3._data[0];
		result += this->_data[1] * vector3._data[1];
		result += this->_data[2] * vector3._data[2];
		return result;
	}

	Vector3 operator*(float i) const {
		Vector3 result;
		result._data[0] = this->_data[0] * i;
		result._data[1] = this->_data[1] * i;
		result._data[2] = this->_data[2] * i;
		return result;
	}

	Vector3 operator/(float i) const {
		Vector3 result;
		result._data[0] = this->_data[0] / i;
		result._data[1] = this->_data[1] / i;
		result._data[2] = this->_data[2] / i;
		return result;
	}

	float operator/(Vector3 vector3) const {
		return this->_data[0] / vector3._data[0];
	}

	Vector3 crossProduct(Vector3 vector3) {
		Vector3 result;
		result._data[0] = this->_data[1] * vector3._data[2]
				- this->_data[2] * vector3._data[1];
		result._data[1] = this->_data[2] * vector3._data[0]
				- this->_data[0] * vector3._data[2];
		result._data[2] = this->_data[0] * vector3._data[1]
				- this->_data[1] * vector3._data[0];
		return result;
	}

	Vector3 makeUnit() {
		return *this / this->length();
	}

	void print() {
		std::cout << _data[0] << " " << _data[1] << " " << _data[2]
				<< std::endl;
	}

	float length() {
		return sqrt(
				_data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2]);
	}

	float distance(Vector3 vector3) {
		return sqrt(
				(this->_data[0] - vector3._data[0])
						* (this->_data[0] - vector3._data[0])
						+ (this->_data[1] - vector3._data[1])
								* (this->_data[1] - vector3._data[1])
						+ (this->_data[2] - vector3._data[2])
								* (this->_data[2] - vector3._data[2]));
	}
};

#endif //MATH_VECTOR3_H
