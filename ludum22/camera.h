#ifndef __CAMERA_H
#define __CAMERA_H

#include "common.h"
#include "surface.h"
#include "actor.h"

class Actor;

class Camera
{
public:
	Camera(int w, int h, int space_w, int space_h,
	       Surface *surface = NULL);
	~Camera();

	void move();
	void clear();
	void draw(Surface *dst, int x, int y) const;

	Surface *get_surface() const { return m_surface; }
	int get_x() const { return m_x; }
	int get_y() const { return m_y; }
	int get_w() const { return m_w; }
	int get_h() const { return m_h; }
	const vector2d get_pos() const;
	const vector2d get_size() const;
	const Rect get_dims() const;
	void set_surface(Surface *surface);
	void set_x(int x) { m_tx = x; }
	void set_y(int y) { m_tx = y; }
	void set_w(int w) { m_w = w; }
	void set_h(int h) { m_h = h; }
	void set_space_w(int space_w) { m_space_w = space_w; }
	void set_space_h(int space_h) { m_space_h = space_h; }
	void set_pos(const vector2d &pos);
	void set_absolute(const vector2d &pos);
	void set_target(Actor *target) { m_target = target; }
private:
	Surface *m_surface;
	int m_x;
	int m_y;
	int m_w;
	int m_h;
	int m_tx;
	int m_ty;
	int m_space_w;
	int m_space_h;
	Actor *m_target;
};

#endif
