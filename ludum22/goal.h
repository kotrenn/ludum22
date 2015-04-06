#ifndef __GOAL_H
#define __GOAL_H

#include "common.h"

class Goal
{
public:
	Goal(FILE *in);
	
	bool collides(const Shape *shape) const;
	string get_next() const { return m_next; }
private:
	string m_next;
	Rect m_dims;
};

#endif
