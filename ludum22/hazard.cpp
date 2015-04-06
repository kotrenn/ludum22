#include "hazard.h"

Hazard::Hazard(const char *name)
	:m_name(name)
{
}

Hazard::~Hazard()
{
}

Hazard *Hazard::clone(FILE *,
                      map<string, Uint32> &)
{
	fprintf(stderr, "Actor::clone() default method\n");
	return NULL;
}

void Hazard::update()
{
}

void Hazard::move(double)
{
}

void Hazard::draw(Camera *) const
{
}



RectHazard::RectHazard()
	:Hazard("rect")
{
}

Hazard *RectHazard::clone(FILE *in,
                          map<string, Uint32> &colors)
{
	RectHazard *ret = new RectHazard();
	char color[81];
	fscanf(in, "%80s", color);
	ret->m_color = colors[color];
	int x, y, w, h;
	fscanf(in, "%d %d %d %d", &x, &y, &w, &h);
	ret->m_dims = Rect(x, y, w, h);
	return ret;
}

void RectHazard::draw(Camera *camera) const
{
	Surface *dst = camera->get_surface();
	int x = (int)(m_dims[0] - camera->get_x());
	int y = (int)(m_dims[1] - camera->get_y());
	int w = m_dims[2];
	int h = m_dims[3];
	dst->draw_rect(m_color, Rect(x, y, w, h));
}

bool RectHazard::collides(const Shape *shape) const
{
	return m_dims.collides(shape);
}



SpikeHazard::SpikeHazard()
	:Hazard("spike")
{
}

Hazard *SpikeHazard::clone(FILE *in,
                           map<string, Uint32> &colors)
{
	SpikeHazard *ret = new SpikeHazard();
	char color[81];
	fscanf(in, "%80s", color);
	ret->m_color = colors[color];
	double x, y;
	fscanf(in, "%lf %lf", &x, &y);
	ret->m_pos = vector2d(x, y);
	fscanf(in, "%lf", &(ret->m_r1));
	fscanf(in, "%lf", &(ret->m_r2));
	ret->m_theta = 0;
	fscanf(in, "%lf", &(ret->m_omega));
	return ret;
}

void SpikeHazard::move(double dt)
{
	m_theta += m_omega * dt;
	while (m_theta > 2 * M_PI)
		m_theta -= 2 * M_PI;
}

void SpikeHazard::draw(Camera *camera) const
{
	Surface *dst = camera->get_surface();
	int x = (int)(m_pos[0] - camera->get_x());
	int y = (int)(m_pos[1] - camera->get_y());
	double cx = x + m_r1 * cos(m_theta);
	double cy = y + m_r1 * sin(m_theta);
	dst->draw_line(m_color, x, y, cx, cy);
	dst->draw_circle(m_color, cx, cy, m_r2);
	int n = 20;
	double dt = 1.0 / n;
	for (int i = 0; i < n; ++i)
	{
		double t = dt * i * 2.0 * M_PI;
		double sx1 = cx + 1.1 * m_r2 * cos(t);
		double sy1 = cy + 1.1 * m_r2 * sin(t);
		double sx2 = cx + m_r2 * cos(t - 0.5 * dt);
		double sy2 = cy + m_r2 * sin(t - 0.5 * dt);
		double sx3 = cx + m_r2 * cos(t + 0.5 * dt);
		double sy3 = cy + m_r2 * sin(t + 0.5 * dt);
		dst->draw_line(m_color, sx1, sy1, sx2, sy2);
		dst->draw_line(m_color, sx1, sy1, sx3, sy3);
	}
}

bool SpikeHazard::collides(const Shape *shape) const
{
	Circle circle(m_pos[0] + m_r1 * cos(m_theta),
	              m_pos[1] + m_r1 * sin(m_theta),
	              m_r2);
	return circle.collides(shape);
}
