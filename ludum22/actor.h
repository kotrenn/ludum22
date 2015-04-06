#ifndef __ACTOR_H
#define __ACTOR_H

#include "common.h"
#include "camera.h"
#include "level.h"
#include "timer.h"

#define TEAM_ENEMY 0
#define TEAM_PLAYER 1

typedef enum
{
	WEST = 0,
	EAST = 1
} Direction;

class Camera;
class Level;

class Actor
{
public:
	Actor(const char *name);
	Actor(Level *level, const char *name,
	      int frames, double delay);
	virtual ~Actor();

	virtual Actor *clone(Level *level, const char *name,
	                     FILE *in);

	string get_name() const { return m_name; }

	virtual void pause();
	virtual void unpause();
	
	virtual void update();
	virtual void move(double dt);
	virtual void draw(Camera *camera) const;

	void explode() const;
	void damage();
	bool dead() const { return m_dead; }

	void set_level(Level *level);
	void set_target(Actor *target) { m_target = target; }
	void actor_dying(Actor *actor);

	Level *get_level() const { return m_level; }
	int get_team() const { return m_team; }
	Circle get_circle() const { return m_circle; }
	vector2d get_center() const;
	vector2d get_v() const { return m_v; }

	void set_center(const vector2d &center);
	void set_v(const vector2d &v) { m_v = v; }
	void set_jump(int jump) { m_jump = jump; }
	void set_falling(bool fall) { m_falling = fall; }
protected:
	void select_target();

	Level *m_level;
	string m_name;
	Circle m_circle;
	vector2d m_v;
	int m_team;
	bool m_flying;
	int m_jump;
	bool m_falling;
	Actor *m_target;
	int m_health;
	bool m_dead;
	Direction m_direction;
	int m_frames;
	vector<Surface *> m_surfaces;
	int m_frame;
	Timer m_animate;
	double m_anim_delay;
	Timer m_invincible;
};

class StandActor : public Actor
{
public:
	StandActor();
	StandActor(Level *level, const char *name);
	StandActor(Level *level, const char *name,
	           const vector2d &pos);

	Actor *clone(Level *level,
	             const char *name,
	             FILE *in);

	void update();
private:
	void create_bullet();
	vector2d m_start;
	double m_speed;
	Timer m_timer;
	double m_range;
};

class FlyActor : public Actor
{
public:
	FlyActor();
	FlyActor(Level *level, const char *name);

	Actor *clone(Level *level,
	             const char *name,
	             FILE *in);

	void update();
private:
	void create_bullet();
	vector2d m_start;
	double m_speed;
	Timer m_timer;
	double m_range;
};

class WallActor : public Actor
{
public:
	WallActor();
	WallActor(Level *level, const char *name);

	Actor *clone(Level *level,
	             const char *name,
	             FILE *in);

	void update();
private:
	void create_bullet();
	Timer m_timer;
};

class PlayerActor : public Actor
{
public:
	PlayerActor(Level *level);
	~PlayerActor();

	void pause();
	void unpause();

	void update();
	void draw(Camera *camera) const;

	void create_bullet();
private:
	double m_fire_rate;
	Timer m_fire_timer;
	Timer m_select_timer;
	Timer m_jump_timer;
	Timer m_health_timer;
	int m_health_max;
};

#endif
