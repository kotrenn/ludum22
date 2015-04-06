#include "actor.h"
#include "datamanager.h"
#include "resourcemanager.h"

Actor::Actor(const char *name)
	:m_level(NULL),
	 m_name(name),
	 m_circle(0, 0, 0),
	 m_v(0, 0),
	 m_team(TEAM_ENEMY),
	 m_flying(false),
	 m_jump(0),
	 m_falling(false),
	 m_target(NULL),
	 m_health(1),
	 m_dead(false),
	 m_direction(WEST)
{
}

Actor::Actor(Level *level, const char *name,
             int frames, double delay)
	:m_level(level),
	 m_name(name),
	 m_circle(randint(0, level->get_w()),
	          randint(0, level->get_h()),
	          20),
	 m_v(0, 0),
	 m_team(TEAM_ENEMY),
	 m_flying(false),
	 m_jump(0),
	 m_falling(false),
	 m_target(NULL),
	 m_health(3),
	 m_dead(false),
	 m_direction(WEST),
	 m_frames(frames),
	 m_surfaces(),
	 m_frame(0),
	 m_animate(0),
	 m_anim_delay(delay)
{
	ResourceManager *rm = ResourceManager::get_instance();
	
	for (int i = 0; i < 2 * m_frames; ++i)
	{
		char filename[81];
		snprintf(filename, 80, "%s%d.png", name, i + 1);
		Surface *surface = rm->get_image(filename);
		m_surfaces.push_back(surface);
	}
}

Actor::~Actor()
{
}

Actor *Actor::clone(Level *, const char *, FILE *)
{
	fprintf(stderr, "Actor::clone() default method\n");
	return NULL;
}

void Actor::pause()
{
	m_invincible.pause();
}

void Actor::unpause()
{
	m_invincible.unpause();
}

void Actor::update()
{
	if (m_target == NULL)
		select_target();
	else
	{
		DataManager *dm = DataManager::get_instance();
		int max_dist = dm->get_int("view_dist");
		vector2d center = m_target->get_center();
		int dx = (int)(m_circle[0] - center[0]);
		int dy = (int)(m_circle[1] - center[1]);
		if (dx * dx + dy * dy > max_dist * max_dist)
			m_target = NULL;
		else if (!m_flying)
			m_direction = m_circle[0] < center[0]? EAST : WEST;
	}

	if (m_animate.done())
	{
		bool herp = true;
		if (m_flying)
			herp = true;
		if (m_target != NULL && !m_flying)
			herp = false;
		if (m_jump > 0)
			herp = false;
		if (fabs(m_v[0]) > EPSILON)
		    herp = false;
		if (!herp)
		{
			m_frame = (m_frame + 1) % m_frames;
			m_animate.reset(m_anim_delay);
		}
	}
}

void Actor::move(double dt)
{
	DataManager *dm = DataManager::get_instance();
	int gravity = dm->get_int("gravity");
	int jump = dm->get_int("jump_speed");

	if (!m_flying)
	{
		if (m_jump > 0 && !m_falling)
		{
			m_jump--;
			m_v.set_y(-jump);
		}
		else
		{
			m_v.set_y(gravity);
			m_falling = true;
		}
	}
	
	m_circle[0] += dt * m_v[0];
	m_circle[1] += dt * m_v[1];
	double r = m_circle[2];

	if (m_circle[0] - r < 0)
		m_circle[0] = r;
	if (m_circle[1] - r < 0)
		m_circle[1] = r;
	if (m_circle[0] + r > m_level->get_w())
		m_circle[0] = m_level->get_w() - r;
	if (m_circle[1] + r > m_level->get_h())
		m_circle[1] = m_level->get_h() - r;

	if (m_target == NULL || m_flying)
		m_direction = m_v[0] > 0? EAST : WEST;
}

void Actor::draw(Camera *camera) const
{
	Surface *dst = camera->get_surface();
	int x = (int)(m_circle[0] - camera->get_x());
	int y = (int)(m_circle[1] - camera->get_y());
	int r = (int)m_circle[2];
	if (x + r < 0 ||
	    y + r < 0 ||
	    x - r > camera->get_w() ||
	    y - r > camera->get_h())
		return;

	int t = (int)(5 * m_invincible.get_time());
	if (t % 2 == 0 && !m_invincible.done())
		return;

	if (m_frames == 0)
		dst->draw_circle(mapRGB(255, 255, 255), x, y, r, 1);
	else
	{
		int frame = m_frame;
		if (m_direction == EAST)
			frame += m_frames;
		Surface *surface = m_surfaces[frame];
		y += r;
		y -= surface->get_h();
		x -= 0.5 * surface->get_w();
		dst->blit(surface, x, y);
	}
}

void Actor::explode() const
{
	DataManager *dm = DataManager::get_instance();

	int num = dm->get_int("num_particles");
	for (int i = 0; i < num; ++i)
	{
		double theta = randint(1, 360) * M_PI / 180.0;
		double speed = 30 * 0.2 * randint(2, 8);
		vector2d v(speed * cos(theta), speed * sin(theta));
		Circle pos = m_circle;
		pos[2] = 0.5 * randint(3, 8);
		double time = 0.05 * randint(10, 40);
		new Particle(m_level, mapRGB(0, 0, 255), pos, v, time);
	}
}

void Actor::damage()
{
	if (!m_invincible.done())
		return;

	DataManager *dm = DataManager::get_instance();
	ResourceManager *rm = ResourceManager::get_instance();
	
	int pain = 1;
	if (m_team == TEAM_PLAYER)
		pain = 20;
	m_health -= pain;
	if (m_team == TEAM_PLAYER)
		m_invincible.reset(dm->get_dbl("invincible"));
	if (m_health <= 0 && !m_dead)
	{
		m_level->remove_actor(this);
		explode();
		m_dead = true;
		rm->play_sound("explode.wav");
	}
	else if (!m_dead)
	{
		if (m_team == TEAM_PLAYER)
			rm->play_sound("player_hit.wav");
		else
			rm->play_sound("enemy_hit.wav");
	}
}

vector2d Actor::get_center() const
{
	vector2d ret(m_circle[0],
	             m_circle[1]);
	return ret;
}

void Actor::set_center(const vector2d &center)
{
	m_circle[0] = center[0];
	m_circle[1] = center[1];
}

void Actor::set_level(Level *level)
{
	m_level = level;
	m_level->add_actor(this);
	m_target = NULL;
}

void Actor::actor_dying(Actor *actor)
{
	if (m_target == actor)
		m_target = NULL;
}

void Actor::select_target()
{
	DataManager *dm = DataManager::get_instance();
	int max_dist = dm->get_int("view_dist");
	m_target = m_level->get_next_target(this, m_target, max_dist);
}



StandActor::StandActor()
	:Actor("stand")
{
}

StandActor::StandActor(Level *level, const char *name)
	:Actor(level, name, 3, 0.3)
{
}

StandActor::StandActor(Level *level, const char *name,
                       const vector2d &pos)
	:Actor(level, name, 3, 0.3)
{
	m_circle = Circle(pos[0], pos[1], 20);
	m_start = pos;
	m_health = 3;
	m_speed = 60;
	m_v = vector2d(m_speed, 0);
	if (randint(1, 2) == 1)
		m_v *= -1;
	m_range = 640;
	m_timer.reset(0);
}

Actor *StandActor::clone(Level *level,
                         const char *name,
                         FILE *in)
{
	StandActor *ret = new StandActor(level, name);
	int x, y;
	fscanf(in, "%d %d", &x, &y);
	ret->m_circle = Circle(x, y, 20);
	ret->m_start = vector2d(x, y);
	ret->m_health = 3;
	fscanf(in, "%lf", &(ret->m_speed));
	ret->m_v = vector2d(ret->m_speed, 0);
	if (randint(1, 2) == 1)
		ret->m_v *= -1;
	fscanf(in, "%lf", &(ret->m_range));
	ret->m_timer.reset(0);
	return ret;
}

void StandActor::update()
{
	Actor::update();
	
	if (m_target != NULL)
	{
		m_v = vector2d(0, 0);
		if (m_timer.done())
		{
			create_bullet();
			m_timer.reset(0.6);
		}
	}
	else
	{
		if (fabs(m_v[0]) < EPSILON)
			m_v[0] = randint(1, 2) == 1? m_speed : -m_speed;
		if (fabs(m_circle[0] - m_start[0]) > m_range)
			m_v[0] *= -1;
	}
}

void StandActor::create_bullet()
{
	DataManager *dm = DataManager::get_instance();
	vector2d m_center = get_center();
	vector2d center = m_target->get_center();

	vector2d dirs[] = { vector2d(1, 0), vector2d(-1, 0),
	                    vector2d(0, 1), vector2d(0, -1),
	                    vector2d(1, 1), vector2d(-1, 1),
	                    vector2d(1, -1), vector2d(-1, -1) };
	for (int i = 0; i < 8; ++i)
		dirs[i].normalize();

	vector2d dir = center - m_center;

	int max_i = 0;
	double max_dot = dir * dirs[0];
	for (int i = 1; i < 8; ++i)
	{
		double dot = dir * dirs[i];
		if (dot > max_dot)
		{
			max_dot = dot;
			max_i = i;
		}
	}
	dir = dirs[max_i];
	int x = m_circle[0];
	int y = m_circle[1];
	Circle circle(x, y, 5);
	double s = dm->get_dbl("pbullet_speed");
	vector2d v = s * dir;
	new Bullet(this, new CircleBullet(circle, v));

	ResourceManager *rm = ResourceManager::get_instance();
	rm->play_sound("stand.wav");
}



FlyActor::FlyActor()
	:Actor("fly")
{
}

FlyActor::FlyActor(Level *level, const char *name)
	:Actor(level, name, 3, 0.3)
{
}

Actor *FlyActor::clone(Level *level,
                         const char *name,
                         FILE *in)
{
	FlyActor *ret = new FlyActor(level, name);
	int x, y;
	fscanf(in, "%d %d", &x, &y);
	ret->m_circle = Circle(x, y, 20);
	ret->m_start = vector2d(x, y);
	ret->m_health = 3;
	fscanf(in, "%lf", &(ret->m_speed));
	ret->m_v = vector2d(ret->m_speed, 0);
	if (randint(1, 2) == 1)
		ret->m_v *= -1;
	fscanf(in, "%lf", &(ret->m_range));
	ret->m_timer.reset(0);
	ret->m_flying = true;
	return ret;
}

void FlyActor::update()
{
	Actor::update();

	if (m_target != NULL)
	{
		if (m_timer.done())
		{
			create_bullet();
			m_timer.reset(0.3);
		}
	}
	if (fabs(m_v[0]) < EPSILON)
		m_v[0] = randint(1, 2) == 1? m_speed : -m_speed;
	if (fabs(m_circle[0] - m_start[0]) > m_range)
		m_v[0] *= -1;
}

void FlyActor::create_bullet()
{
	DataManager *dm = DataManager::get_instance();
	vector2d m_center = get_center();
	vector2d center = m_target->get_center();

	vector2d dir = center - m_center;
	dir.normalize();

	int x = m_circle[0];
	int y = m_circle[1];
	Circle circle(x, y, 5);
	double s = dm->get_dbl("pbullet_speed");
	vector2d v = s * dir;
	new Bullet(this, new CircleBullet(circle, v));

	ResourceManager *rm = ResourceManager::get_instance();
	rm->play_sound("fly.wav");
}



WallActor::WallActor()
	:Actor("wall")
{
}

WallActor::WallActor(Level *level, const char *name)
	:Actor(level, name, 1, 1.0)
{
}

Actor *WallActor::clone(Level *level,
                         const char *name,
                         FILE *in)
{
	WallActor *ret = new WallActor(level, name);
	int x, y;
	fscanf(in, "%d %d", &x, &y);
	ret->m_circle = Circle(x, y, 5);
	ret->m_health = 10;
	ret->m_v = vector2d(0, 0);
	ret->m_timer.reset(0);
	ret->m_flying = true;
	return ret;
}

void WallActor::update()
{
	Actor::update();

	if (m_target != NULL)
	{
		if (m_timer.done())
		{
			create_bullet();
			m_timer.reset(2.0);
		}
	}
}

void WallActor::create_bullet()
{
	DataManager *dm = DataManager::get_instance();
	vector2d m_center = get_center();
	vector2d center = m_target->get_center();

	vector2d dir = center - m_center;
	dir.normalize();

	double x = m_circle[0];
	double y = m_circle[1];
	Circle circle(x, y, 3);
	double speed = dm->get_dbl("pbullet_speed");
	vector2d v = speed * dir;
	int theta[] = { -15, 0, 15 };
	for (int i = 0; i < 3; ++i)
	{
		double c = cos(theta[i] * M_PI / 180.0);
		double s = sin(theta[i] * M_PI / 180.0);
		vector2d vp(c * v[0] - s * v[1],
		            s * v[0] + c * v[1]);
		new Bullet(this, new CircleBullet(circle, vp));
	}

	ResourceManager *rm = ResourceManager::get_instance();
	rm->play_sound("wall.wav");
}



PlayerActor::PlayerActor(Level *level)
	:Actor(level, "player", 4, 0.2),
	 m_fire_rate(0),
	 m_fire_timer(0),
	 m_select_timer(0),
	 m_jump_timer(0),
	 m_health_timer(0),
	 m_health_max(0)
{
	DataManager *dm = DataManager::get_instance();
	
	m_team = TEAM_PLAYER;
	m_circle[0] = 200;
	m_circle[1] = m_level->get_h() - 100;
	m_circle[2] = 15;

	m_fire_rate = dm->get_dbl("pbullet_delay");
	m_health_max = dm->get_int("player_health");
	m_health = m_health_max;
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::pause()
{
	Actor::pause();

	m_fire_timer.pause();
	m_select_timer.pause();
	m_jump_timer.pause();
	m_health_timer.pause();
}

void PlayerActor::unpause()
{
	Actor::unpause();

	m_fire_timer.unpause();
	m_select_timer.unpause();
	m_jump_timer.unpause();
	m_health_timer.unpause();
}

void PlayerActor::update()
{
	Actor::update();

	m_v[0] = 0;
	m_v[1] = 0;

	DataManager *dm = DataManager::get_instance();

	Uint8 *keys = SDL_GetKeyState(NULL);
	int speed = dm->get_int("player_speed");
	if (keys[SDLK_UP])
		m_v[1] -= speed;
	if (keys[SDLK_DOWN])
		m_v[1] += speed;
	if (keys[SDLK_LEFT])
		m_v[0] -= speed;
	if (keys[SDLK_RIGHT])
		m_v[0] += speed;

	// jump
	/*
	if (keys[SDLK_x])
	{
		if (m_jump < 0)
			m_jump = dm->get_int("jump_time");
	}
	else
		m_jump = 0;
	*/
	if (keys[SDLK_x] && !m_falling)
	{
		if (m_jump < 0)
		{
			double jump_time = dm->get_dbl("jump_time");
			m_jump_timer.reset(jump_time);
			m_jump = 1000;
		}
	}
	else
		m_jump = 0;
	if (m_jump_timer.done())
		m_jump = 0;

	// shoot
	if (keys[SDLK_z] && m_target != NULL)
	{
		if (m_fire_timer.done())
		{
			m_fire_timer.reset(m_fire_rate);
			create_bullet();
		}
	}

	// select
	if (keys[SDLK_c])
	{
		if (m_select_timer.done())
		{
			select_target();
			if (m_target != NULL)
				m_select_timer.reset(0.33);
		}
	}

	// heal
	if (m_health_timer.done() && m_health < m_health_max)
	{
		m_health++;
		m_health_timer.reset(dm->get_dbl("heal_delay"));
	}
}

void PlayerActor::draw(Camera *camera) const
{
	Actor::draw(camera);

	Surface *dst = camera->get_surface();
	
	// health bar
	int health_w = 20;
	int health_h = dst->get_h() * 0.8;
	int health_x = dst->get_w() - 2 * health_w;
	int health_y = 0.1 * dst->get_h();
	int buffer = 2;

	Uint32 black = mapRGB(0, 0, 0);
	Uint32 red = mapRGB(255, 0, 0);

	double p = (1.0 * m_health) / m_health_max;

	dst->draw_rect(black,
	               Rect(health_x - buffer, health_y - buffer,
	                    health_w + 2 * buffer,
	                    health_h + 2 * buffer),
	               buffer);
	dst->draw_rect_3d(red,
	                  Rect(health_x, (1 - p) * health_h + health_y,
	                       health_w, p * health_h));
	if (m_target == NULL)
		return;

	Circle circle = m_target->get_circle();
	int x = (int)(circle[0] - camera->get_x());
	int y = (int)(circle[1] - camera->get_y());
	int r = (int)(circle[2] * 1.5);
	dst->draw_circle(red, x, y, r, 1);
	dst->draw_line(red, x - r, y, x + r, y);
	dst->draw_line(red, x, y - r, x, y + r);

}

void PlayerActor::create_bullet()
{
	DataManager *dm = DataManager::get_instance();
	vector2d m_center = get_center();
	vector2d center = m_target->get_center();

	vector2d dirs[] = { vector2d(1, 0), vector2d(-1, 0),
	                    vector2d(0, 1), vector2d(0, -1),
	                    vector2d(1, 1), vector2d(-1, 1),
	                    vector2d(1, -1), vector2d(-1, -1) };
	for (int i = 0; i < 8; ++i)
		dirs[i].normalize();

	vector2d dir = center - m_center;

	int max_i = 0;
	double max_dot = dir * dirs[0];
	for (int i = 1; i < 8; ++i)
	{
		double dot = dir * dirs[i];
		if (dot > max_dot)
		{
			max_dot = dot;
			max_i = i;
		}
	}
	dir = dirs[max_i];
	int x = m_circle[0];
	int y = m_circle[1];
	Circle circle(x, y, 5);
	double s = dm->get_dbl("pbullet_speed");
	vector2d v = s * dir;
	new Bullet(this, new CircleBullet(circle, v));

	ResourceManager *rm = ResourceManager::get_instance();
	rm->play_sound("player.wav");
}
