#include "bullet.h"

Bullet::Bullet(Actor *parent, BulletShape *shape)
	:m_level(parent->get_level()),
	 m_parent(parent),
	 m_shape(shape),
	 m_team(parent->get_team())
{
	m_level->add_bullet(this);
}

Bullet::~Bullet()
{
	delete m_shape;
}

void Bullet::update()
{
	list<Actor *> actors;
	m_level->get_collisions(&actors, this);
	list<Actor *>::iterator a;
	for (a = actors.begin(); a != actors.end(); a++)
		(*a)->damage();
	if (actors.size() > 0)
		m_level->remove_bullet(this);
}

void Bullet::move(double dt)
{
	m_shape->move(dt);
}

void Bullet::draw(Camera *camera) const
{
	m_shape->draw(camera);
}

bool Bullet::collides(Actor *actor) const
{
	Circle circle = actor->get_circle();
	return circle.collides(get_shape());
}

void Bullet::actor_dying(Actor *actor)
{
	if (m_parent == actor)
		m_parent = NULL;
}

Shape *Bullet::get_shape() const
{
	return m_shape->get_shape();
}
