#include "terrain.h"

Terrain::Terrain()
	:m_dims(),
	 m_color(mapRGB(0, 0, 255))
{
}

Terrain::Terrain(Uint32 color,
                 const Rect &dims)
	:m_dims(dims),
	 m_y1(dims[1]),
	 m_y2(dims[1]),
	 m_y3(dims[1] + dims[3]),
	 m_y4(dims[1] + dims[3]),
	 m_color(color)
{
}

Terrain::Terrain(Uint32 color,
                 const Rect &dims,
                 double y1, double y2,
                 double y3, double y4)
	:m_dims(dims),
	 m_y1(y1),
	 m_y2(y2),
	 m_y3(y3),
	 m_y4(y4),
	 m_color(color)
{
}

Terrain::~Terrain()
{
}

void Terrain::fix(Actor *actor) const
{
	vector2d v = actor->get_v();
	Circle circle = actor->get_circle();

	vector2d p1(m_dims[0], m_y1);
	vector2d p2(m_dims[0] + m_dims[2], m_y2);
	vector2d p3(m_dims[0] + m_dims[2], m_y4);
	vector2d p4(m_dims[0], m_y3);

	double slope = (p2[1] - p1[1]) / (p2[0] - p1[0]);
	bool jump = fabs(slope) < 1.5;
	line_fix(actor, p1, p2, jump, false);
	line_fix(actor, p2, p3, false, false);
	line_fix(actor, p3, p4, false, true);
	line_fix(actor, p4, p1, false, false);
}

void Terrain::line_fix(Actor *actor, vector2d p1,
                       vector2d p2,
                       bool jump, bool bottom) const
{
	vector2d v = actor->get_v();
	Circle circle = actor->get_circle();
	
	Line line(p1[0], p1[1], p2[0], p2[1]);
	if (line.collides(&circle))
	{
		v.set_y(0);
		vector2d u = p2 - p1;
		if (u.norm() < EPSILON)
			return;
		u.normalize();
		vector2d c(circle[0], circle[1]);
		vector2d p1c = c - p1;
		vector2d x = p1 + (p1c * u) * u;
		if ((x - p1) * u < 0)
			return;
		if ((p2 - x) * u < 0)
			return;
		vector2d xc = c - x;
		double r = circle[2];
		double n = xc.norm();
		if (fabs(n) < EPSILON)
		{
			xc = vector2d(u[1], -u[0]);
			n = xc.norm();
		}
		vector2d cp = c + (r - n) * (xc / n);
		if (bottom || jump)
			actor->set_v(v);
		actor->set_center(cp);
		if (bottom)
			actor->set_falling(true);
		if (jump)
		{
			actor->set_jump(-1);
			actor->set_falling(false);
		}
	}
}

bool Terrain::collides(const Bullet *bullet) const
{
	vector2d p1(m_dims[0], m_y1);
	vector2d p2(m_dims[0] + m_dims[2], m_y2);
	vector2d p3(m_dims[0] + m_dims[2], m_y4);
	vector2d p4(m_dims[0], m_y3);
	
	Line line1(p1, p2);
	Line line2(p2, p3);
	Line line3(p3, p4);
	Line line4(p4, p1);

	Shape *shape = bullet->get_shape();

	if (line1.collides(shape) && line1.len() > EPSILON)
		return true;
	if (line2.collides(shape) && line2.len() > EPSILON)
		return true;
	if (line3.collides(shape) && line3.len() > EPSILON)
		return true;
	if (line4.collides(shape) && line4.len() > EPSILON)
		return true;

	vector2d p = shape->interior();

	vector2d v1 = p2 - p1;
	vector2d v2 = p3 - p2;
	vector2d v3 = p4 - p3;
	vector2d v4 = p1 - p4;

	vector2d i1 = p - p1;
	vector2d i2 = p - p2;
	vector2d i3 = p - p3;
	vector2d i4 = p - p4;

	if (v1 % i1 < 0 || v2 % i2 < 0 ||
	    v3 % i3 < 0 || v4 % i4 < 0)
		return true;

	return false;
}

void Terrain::draw(Camera *camera) const
{
	Surface *dst = camera->get_surface();
	Rect rect(m_dims[0] - camera->get_x(),
	          m_dims[1] - camera->get_y(),
	          m_dims[2], m_dims[3]);

	Rect dims(m_dims[0], min(m_y1, m_y2),
	          m_dims[2], max(m_y3, m_y4) - min(m_y1, m_y2));
	Rect cdims = camera->get_dims();
	if (!cdims.collides(&dims))
		return;

	dst->draw_rect(m_color, rect);

	Uint8 c[3];
	SDL_GetRGB(m_color, dst->get_format(),
	           c, c + 1, c + 2);
	Uint32 color1 = mapRGB((255 + c[0]) / 2,
	                       (255 + c[1]) / 2,
	                       (255 + c[2]) / 2);
	Uint32 color2 = mapRGB(c[0] / 2, c[1] / 2, c[2] / 2);

	int x1 = (int)rect.get_x();
	//int y1 = (int)rect.get_y();
	int x2 = (int)(rect.get_x() + rect.get_w());
	//int y2 = (int)(rect.get_y() + rect.get_h());
	int z1 = (int)(m_y1 - camera->get_y());
	int z2 = (int)(m_y2 - camera->get_y());
	int z3 = (int)(m_y3 - camera->get_y());
	int z4 = (int)(m_y4 - camera->get_y());

	draw_triangle(dst, x1, z1, x2 - 1, z2, true);
	draw_triangle(dst, x1, z3 - 1, x2 - 1, z4 - 1, false);

	// top
	dst->draw_line(color1, x1, z1, x2 - 1, z2);
	dst->draw_line(color1, x1 + 1, z1 + 1, x2 - 2, z2 + 1);

	// bottom
	dst->draw_line(color2, x1, z3 - 1, x2 - 1, z4 - 1);
	dst->draw_line(color2, x1 + 1, z3 - 2, x2 - 2, z4 - 2);

	// left
	dst->draw_line(color1, x1, z1, x1, z3 - 1);
	dst->draw_line(color1, x1 + 1, z1 + 1, x1 + 1, z3 - 2);

	// right
	dst->draw_line(color2, x2 - 1, z2, x2 - 1, z4 - 1);
	dst->draw_line(color2, x2 - 2, z2 - 1, x2 - 2, z4 - 2);

	return;
	
	vector2d p1(m_dims[0], m_y1);
	vector2d p2(m_dims[0] + m_dims[2], m_y2);
	vector2d p3(m_dims[0] + m_dims[2], m_y4);
	vector2d p4(m_dims[0], m_y3);

	vector2d cam(camera->get_x(), camera->get_y());

	p1 -= cam;
	p2 -= cam;
	p3 -= cam;
	p4 -= cam;

	Uint32 red = mapRGB(255, 0, 0);
	dst->draw_line(red, p1[0], p1[1], p2[0], p2[1]);
	dst->draw_line(red, p2[0], p2[1], p3[0], p3[1]);
	dst->draw_line(red, p3[0], p3[1], p4[0], p4[1]);
	dst->draw_line(red, p4[0], p4[1], p1[0], p1[1]);
}

void Terrain::draw_triangle(Surface *dst,
                            int x1, int y1,
                            int x2, int y2,
                            bool top) const
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = ((y1 < y2 &&  top) ||
	          (y1 > y2 && !top))? x1 : x2 - 1;

	if (dx == 0 || dy == 0)
		return;

	if (dx > dy)
	{
		if (x1 > x2)
		{
			x1 ^= x2;
			x2 ^= x1;
			x1 ^= x2;

			y1 ^= y2;
			y2 ^= y1;
			y1 ^= y2;
		}

		int ddy = 1;
		if (y1 >= y2) ddy = -1;
		int y = y1;
		int p = 0;

		for (int x = x1; x <= x2; ++x)
		{
			//draw_point(color, x, y);
			dst->draw_line(m_color, sx, y, x, y);

			p += dy;

			if (p * 2 >= dx)
			{
				y += ddy;
				p -= dx;
			}
		}
	}
	else
	{
		if (y1 > y2)
		{
			x1 ^= x2;
			x2 ^= x1;
			x1 ^= x2;

			y1 ^= y2;
			y2 ^= y1;
			y1 ^= y2;
		}

		int x = x1;
		int p = 0;
		int ddx = 1;
		if (x1 >= x2) ddx = -1;

		for (int y = y1; y <= y2; ++y)
		{
			//draw_point(color, x, y);
			dst->draw_line(m_color, sx, y, x, y);

			p += dx;

			if (p * 2 >= dy)
			{
				x += ddx;
				p -= dy;
			}
		}
	}
}
