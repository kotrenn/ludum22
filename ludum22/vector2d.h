#ifndef __VECTOR2D_H
#define __VECTOR2D_H

#include <math.h>
#include <iostream>

#include "common.h"

using namespace std;

class vector2d
{
public:
	vector2d();
	vector2d(double x, double y);
	vector2d(const vector2d &other);
	~vector2d();
    
	const vector2d &operator=(const vector2d &rhs);
	const double &operator[](int i) const;
	double &operator[](int i);
	const vector2d operator+(const vector2d &rhs) const;
	const vector2d operator+=(const vector2d &rhs);
	const vector2d operator-(const vector2d &rhs) const;
	const vector2d operator-=(const vector2d &rhs);
	const vector2d operator*(double rhs) const;
	const vector2d operator*=(double rhs);
	const vector2d operator/(double rhs) const;
	const vector2d operator/=(double rhs);
	double operator*(const vector2d &rhs) const;
	double operator%(const vector2d &rhs) const;
	bool operator==(const vector2d &rhs) const;
	bool operator!=(const vector2d &rhs) const;

	double get_x() const { return m_x; }
	double get_y() const { return m_y; }
	void set_x(double x) { m_x = x; }
	void set_y(double y) { m_y = y; }
	double norm() const;
	void normalize();
private:
	double m_x, m_y;
};

std::ostream &operator<<(std::ostream &out, const vector2d &rhs);
const vector2d operator*(double lhs, const vector2d &rhs);

inline std::ostream &operator<<(std::ostream &out, const vector2d &rhs)
{
	out.precision(3);
	return out << fixed << "[" << rhs[0] << ", " << rhs[1] << "]";
}

inline const double &vector2d::operator[](int i) const
{
	if (i == 0)
		return m_x;
	else
		return m_y;
}

inline double &vector2d::operator[](int i)
{
	if (i == 0)
		return m_x;
	else
		return m_y;
}

inline const vector2d vector2d::operator+(const vector2d &rhs) const
{
	vector2d ret(m_x, m_y);
	ret.m_x += rhs.m_x;
	ret.m_y += rhs.m_y;
	return ret;
}

inline const vector2d vector2d::operator+=(const vector2d &rhs)
{
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	return *this;
}

inline const vector2d vector2d::operator-(const vector2d &rhs) const
{
	vector2d ret(m_x, m_y);
	ret.m_x -= rhs.m_x;
	ret.m_y -= rhs.m_y;
	return ret;
}

inline const vector2d vector2d::operator-=(const vector2d &rhs)
{
	m_x -= rhs.m_x;
	m_y -= rhs.m_y;
	return *this;
}

inline const vector2d vector2d::operator*(double rhs) const
{
	vector2d ret(m_x, m_y);
	ret.m_x *= rhs;
	ret.m_y *= rhs;
	return ret;
}

inline const vector2d operator*(double lhs, const vector2d &rhs)
{
	vector2d ret(lhs * rhs[0], lhs * rhs[1]);
	return ret;
}

inline const vector2d vector2d::operator*=(double rhs)
{
	m_x *= rhs;
	m_y *= rhs;
	return *this;
}

inline const vector2d vector2d::operator/(double rhs) const
{
	vector2d ret(m_x, m_y);
	ret.m_x /= rhs;
	ret.m_y /= rhs;
	return ret;
}

inline const vector2d vector2d::operator/=(double rhs)
{
	m_x /= rhs;
	m_y /= rhs;
	return *this;
}

inline double vector2d::operator*(const vector2d &rhs) const
{
	double dot = m_x * rhs.m_x + m_y * rhs.m_y;
	return dot;
}

inline double vector2d::operator%(const vector2d &rhs) const
{
	double ret = m_x * rhs.m_y - rhs.m_y * m_x;
	return ret;
}

inline bool vector2d::operator==(const vector2d &rhs) const
{
	bool ret =
		fabs(m_x - rhs.m_x) < EPSILON &&
		fabs(m_y - rhs.m_y) < EPSILON;
	return ret;
}

inline bool vector2d::operator!=(const vector2d &rhs) const
{
	bool ret =
		fabs(m_x - rhs.m_x) < EPSILON &&
		fabs(m_y - rhs.m_y) < EPSILON;
	return !ret;
}

inline double vector2d::norm() const
{
	return sqrt(m_x * m_x + m_y * m_y);
}

inline void vector2d::normalize()
{
	double mag = norm();
	if (mag > EPSILON)
	{
		m_x /= mag;
		m_y /= mag;
	}
}

#endif
