#ifndef __BULLET_SHAPE_H
#define __BULLET_SHAPE_H

#include "common.h"
#include "camera.h"

class Camera;

class BulletShape
{
public:
	BulletShape(Shape *shape,
	            const vector2d &v);
	virtual ~BulletShape();

	virtual void move(double dt) = 0;
	virtual void draw(Camera *camera) const = 0;

	Shape *get_shape() const { return m_shape; }
protected:
	Shape *m_shape;
	vector2d m_v;
};

class RectBullet : public BulletShape
{
public:
	RectBullet(const Rect &rect,
	           const vector2d &v);
	~RectBullet();

	void move(double dt);
	void draw(Camera *camera) const;
};

class CircleBullet : public BulletShape
{
public:
	CircleBullet(const Circle &circle,
	             const vector2d &v);
	~CircleBullet();
	
	void move(double dt);
	void draw(Camera *camera) const;
};

class LineBullet : public BulletShape
{
public:
	LineBullet(const Line &line,
	           const vector2d &v);
	~LineBullet();
	
	void move(double dt);
	void draw(Camera *camera) const;
};

#endif
