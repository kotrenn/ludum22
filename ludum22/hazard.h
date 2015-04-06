#ifndef __HAZARD_H
#define __HAZARD_H

#include "camera.h"

class Hazard
{
public:
	Hazard(const char *name);
	virtual ~Hazard();

	virtual Hazard *clone(FILE *in,
	                      map<string, Uint32> &colors);

	string get_name() const { return m_name; }

	virtual void pause() {}
	virtual void unpause() {}
	
	virtual void update();
	virtual void move(double dt);
	virtual void draw(Camera *camera) const;
	
	virtual bool collides(const Shape *shape) const = 0;
protected:
	string m_name;
};

class RectHazard : public Hazard
{
public:
	RectHazard();

	Hazard *clone(FILE *in,
	              map<string, Uint32> &colors);
	
	void draw(Camera *camera) const;

	bool collides(const Shape *shape) const;
private:
	Uint32 m_color;
	Rect m_dims;
};

class SpikeHazard : public Hazard
{
public:
	SpikeHazard();

	Hazard *clone(FILE *in,
	              map<string, Uint32> &colors);

	void move(double dt);
	void draw(Camera *camera) const;

	bool collides(const Shape *shape) const;
private:
	Uint32 m_color;
	vector2d m_pos;
	double m_r1;
	double m_r2;
	double m_theta;
	double m_omega;
};

#endif
