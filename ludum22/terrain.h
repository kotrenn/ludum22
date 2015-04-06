#ifndef __TERRAIN_H
#define __TERRAIN_H

#include "actor.h"
#include "bullet.h"
#include "camera.h"

class Actor;
class Bullet;
class Camera;

class Terrain
{
public:
	Terrain();
	Terrain(Uint32 color,
	        const Rect &dims);
	Terrain(Uint32 color,
	        const Rect &dims,
	        double y1, double y2,
	        double y3, double y4);
	~Terrain();

	void fix(Actor *actor) const;
	bool collides(const Bullet *bullet) const;

	void draw(Camera *camera) const;
private:
	void line_fix(Actor *actor, vector2d p1,
	              vector2d p2,
	              bool jump, bool bottom) const;
	void draw_triangle(Surface *surface,
	                   int x1, int y1,
	                   int x2, int y2,
	                   bool top) const;
	Rect m_dims;
	double m_y1;
	double m_y2;
	double m_y3;
	double m_y4;
	Uint32 m_color;
};

#endif
