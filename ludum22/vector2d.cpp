#include "vector2d.h"

vector2d::vector2d()
	: m_x(0), m_y(0)
{
}

vector2d::vector2d(double x, double y)
	: m_x(x), m_y(y)
{
}

vector2d::vector2d(const vector2d &other)
	: m_x(other.m_x), m_y(other.m_y)
{
}

vector2d::~vector2d()
{
}

const vector2d &vector2d::operator=(const vector2d &rhs)
{
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	return *this;
}

