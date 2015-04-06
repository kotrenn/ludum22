#include "goal.h"

Goal::Goal(FILE *in)
	:m_next(),
	 m_dims()
{
	char next[81];
	fscanf(in, "%80s", next);
	m_next = next;
	
	double x, y, w, h;
	fscanf(in, "%lf %lf %lf %lf",
	       &x, &y, &w, &h);
	m_dims = Rect(x, y, w, h);
}

bool Goal::collides(const Shape *shape) const
{
	return m_dims.collides(shape);
}
