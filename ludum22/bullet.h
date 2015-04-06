#ifndef __BULLET_H
#define __BULLET_H

#include "actor.h"
#include "bulletshape.h"
#include "camera.h"
#include "level.h"

class Actor;
class BulletShape;
class Camera;
class Level;

class Bullet
{
public:
	Bullet(Actor *parent, BulletShape *shape);
	~Bullet();

	void update();
	void move(double dt);
	void draw(Camera *camera) const;
	int get_team() const { return m_team; }
	bool collides(Actor *actor) const;
	void actor_dying(Actor *actor);

	Shape *get_shape() const;
private:
	Level *m_level;
	Actor *m_parent;
	BulletShape *m_shape;
	int m_team;
};

#endif
