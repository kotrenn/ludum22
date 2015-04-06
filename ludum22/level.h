#ifndef __LEVEL_H
#define __LEVEL_H

#include "actor.h"
#include "bullet.h"
#include "camera.h"
#include "common.h"
#include "decoration.h"
#include "goal.h"
#include "hazard.h"
#include "particle.h"
#include "terrain.h"

class Actor;
class Bullet;
class Camera;
class Decoration;
class Hazard;
class Particle;
class Terrain;

class Level
{
public:
	Level(int w, int h);
	Level(const char *filename);
	~Level();

	void pause();
	void unpause();

	void update();
	void move(double dt);
	void draw(Camera *camera) const;

	string get_name() const { return m_name; }
	int get_w() const { return m_w; }
	int get_h() const { return m_h; }
	vector2d get_start() const { return m_start; }
	bool goal(Actor *actor, string *next) const;

	void add_actor(Actor *actor);
	void add_bullet(Bullet *bullet);
	void add_particle(Particle *particle);
	
	void remove_actor(Actor *actor);
	void remove_bullet(Bullet *bullet);

	bool out_of_bounds(const Bullet *bullet) const;
	void get_collisions(list<Actor *> *actors,
	                    const Bullet *bullet) const;
	Actor *get_next_target(Actor *src, Actor *target,
	                       double dist) const;
	bool hazard(const Actor *actor) const;
private:
	void add_lists();
	void clear_lists();

	string m_name;
	int m_w;
	int m_h;
	vector2d m_start;
	Uint32 m_background;
	
	set<Actor *> m_actors;
	set<Actor *> m_actors_add;
	set<Actor *> m_actors_remove;
	
	set<Bullet *> m_bullets;
	set<Bullet *> m_bullets_add;
	set<Bullet *> m_bullets_remove;

	list<Terrain> m_terrain;
	list<Decoration *> m_decorations;
	list<Goal *> m_goals;
	list<Hazard *> m_hazards;

	list<Particle *> m_particles;
};

#endif
