#include "level.h"
#include "actorfactory.h"
#include "hazardfactory.h"

Level::Level(int w, int h)
	:m_name(""),
	 m_w(w), m_h(h),
	 m_start(0, 0),
	 m_background(mapRGB(0, 0, 0)),
	 m_actors(), m_actors_add(), m_actors_remove(),
	 m_bullets(), m_bullets_add(), m_bullets_remove(),
	 m_terrain(),
	 m_decorations(),
	 m_goals(),
	 m_hazards(),
	 m_particles()
{
	m_terrain.push_back(Terrain(mapRGB(0, 0, 255),
	                            Rect(0, m_h - 20, m_w, 20),
	                            m_h - 20, m_h - 20, m_h, m_h));
}

Level::Level(const char *filename)
	:m_name(filename),
	 m_w(0), m_h(0),
	 m_start(0, 0),
	 m_background(mapRGB(0, 0, 0)),
	 m_actors(), m_actors_add(), m_actors_remove(),
	 m_bullets(), m_bullets_add(), m_bullets_remove(),
	 m_terrain(),
	 m_decorations(),
	 m_goals(),
	 m_hazards(),
	 m_particles()
{
	char fullname[81];
	snprintf(fullname, 80, "levels/%s.txt", filename);
	FILE *in;

	if ((in = fopen(fullname, "r")) == NULL)
	{
		fprintf(stderr, "Error: Could not open %s for reading\n",
		        fullname);
		exit(1);
	}

	fscanf(in, "%d %d", &m_w, &m_h);

	map<string, Uint32> colors;

	char buf[81];
	while (fscanf(in, "%80s", buf) > 0)
	{
		if (strcmp(buf, "color") == 0)
		{
			char color[81];
			int r, g, b;
			fscanf(in, "%s %d %d %d", color, &r, &g, &b);
			Uint32 c = mapRGB(r, g, b);
			colors[color] = c;
		}
		if (strcmp(buf, "terrain") == 0)
		{
			char color[81];
			fscanf(in, "%s", color);
			Uint32 c = mapRGB(255, 255, 255);
			if (colors.find(color) == colors.end())
				fprintf(stderr, "Could not find color %s\n", color);
			else
				c = colors[color];
			double x, w, h, y1, y2, y3, y4;
			fscanf(in, "%lf %lf %lf %lf %lf %lf %lf",
			       &x, &w, &h, &y1, &y2, &y3, &y4);
			Rect dims(x, max(y1, y2), w, h);
			m_terrain.push_back(Terrain(c, dims, y1, y2, y3, y4));
		}
		else if (strcmp(buf, "actor") == 0)
		{
			char name[81];
			fscanf(in, "%80s", name);
			ActorFactory *af = ActorFactory::get_instance();
			Actor *actor = af->load_actor(this, name, in);
			add_actor(actor);
		}
		else if (strcmp(buf, "start") == 0)
		{
			double x, y;
			fscanf(in, "%lf %lf", &x, &y);
			m_start = vector2d(x, y);
		}
		else if (strcmp(buf, "goal") == 0)
		{
			Goal *goal = new Goal(in);
			m_goals.push_back(goal);
		}
		else if (strcmp(buf, "decoration") == 0)
		{
			Decoration *deco = new Decoration(in);
			m_decorations.push_back(deco);
		}
		else if (strcmp(buf, "hazard") == 0)
		{
			char name[81];
			fscanf(in, "%80s", name);
			HazardFactory *hf = HazardFactory::get_instance();
			Hazard *hazard = hf->load_hazard(name, in, colors);
			m_hazards.push_back(hazard);
		}
		if (strcmp(buf, "background") == 0)
		{
			char color[81];
			fscanf(in, "%s", color);
			if (colors.find(color) == colors.end())
				fprintf(stderr, "Could not find color %s\n", color);
			else
				m_background = colors[color];
		}
	}

	fclose(in);
}

Level::~Level()
{
	add_lists();

	set<Actor *>::iterator a;
	set<Bullet *>::iterator b;
	
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		remove_actor(*a);
	for (b = m_bullets.begin(); b != m_bullets.end(); b++)
		remove_bullet(*b);

	m_actors.clear();
	m_bullets.clear();

	clear_lists();

	while (m_decorations.size() > 0)
	{
		delete m_decorations.back();
		m_decorations.pop_back();
	}
	
	while (m_goals.size() > 0)
	{
		delete m_goals.back();
		m_goals.pop_back();
	}
	
	while (m_hazards.size() > 0)
	{
		delete m_hazards.back();
		m_hazards.pop_back();
	}
	
	while (m_particles.size() > 0)
	{
		delete m_particles.back();
		m_particles.pop_back();
	}
}

void Level::pause()
{
	set<Actor *>::iterator a;
	list<Decoration *>::iterator d;
	list<Hazard *>::iterator h;
	list<Particle *>::iterator p;
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		(*a)->pause();
	for (d = m_decorations.begin(); d != m_decorations.end(); d++)
		(*d)->pause();
	for (h = m_hazards.begin(); h != m_hazards.end(); h++)
		(*h)->pause();
	for (p = m_particles.begin(); p != m_particles.end(); p++)
		(*p)->pause();
}

void Level::unpause()
{
	set<Actor *>::iterator a;
	list<Decoration *>::iterator d;
	list<Hazard *>::iterator h;
	list<Particle *>::iterator p;
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		(*a)->unpause();
	for (d = m_decorations.begin(); d != m_decorations.end(); d++)
		(*d)->pause();
	for (h = m_hazards.begin(); h != m_hazards.end(); h++)
		(*h)->unpause();
	for (p = m_particles.begin(); p != m_particles.end(); p++)
		(*p)->unpause();
}

void Level::update()
{
	set<Actor *>::iterator a;
	set<Bullet *>::iterator b;
	
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		(*a)->update();
	for (b = m_bullets.begin(); b != m_bullets.end(); b++)
		(*b)->update();
	
	// check for stuff out of bounds here
	for (b = m_bullets.begin(); b != m_bullets.end(); b++)
		if (out_of_bounds(*b))
			remove_bullet(*b);

	add_lists();
	clear_lists();

	list<Decoration *>::iterator d;
	for (d = m_decorations.begin(); d != m_decorations.end(); d++)
		(*d)->update();

	list<Hazard *>::iterator h;
	for (h = m_hazards.begin(); h != m_hazards.end(); h++)
		(*h)->update();

	// clear out dead particles
	list<Particle *>::iterator p = m_particles.begin();
	while (p != m_particles.end())
	{
		(*p)->update();
		if ((*p)->dead())
		{
			Particle *particle = *p;
			p = m_particles.erase(p);
			delete particle;
		}
		else
			p++;
	}
}

void Level::add_lists()
{
	set<Actor *>::iterator a;
	set<Bullet *>::iterator b;
	
	for (a = m_actors_add.begin(); a != m_actors_add.end(); a++)
		m_actors.insert(*a);
	for (b = m_bullets_add.begin(); b != m_bullets_add.end(); b++)
		m_bullets.insert(*b);

	m_actors_add.clear();
	m_bullets_add.clear();
}

void Level::clear_lists()
{
	set<Actor *>::iterator a;
	set<Bullet *>::iterator b;
	
	for (a = m_actors_remove.begin();
	     a != m_actors_remove.end(); a++)
	{
		set<Actor *>::iterator a2;
		for (a2 = m_actors.begin(); a2 != m_actors.end(); a2++)
			(*a2)->actor_dying(*a);
		for (b = m_bullets.begin(); b != m_bullets.end(); b++)
			(*b)->actor_dying(*a);
	}
	
	for (a = m_actors_remove.begin();
	     a != m_actors_remove.end(); a++)
	{
		m_actors.erase(*a);
		if ((*a)->get_team() != TEAM_PLAYER)
			delete *a;
	}

	for (b = m_bullets_remove.begin();
	     b != m_bullets_remove.end(); b++)
	{
		m_bullets.erase(*b);
		delete *b;
	}

	m_actors_remove.clear();
	m_bullets_remove.clear();
}

void Level::move(double dt)
{
	set<Actor *>::iterator a;
	set<Bullet *>::iterator b;
	
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		(*a)->move(dt);
	for (b = m_bullets.begin(); b != m_bullets.end(); b++)
		(*b)->move(dt);

	list<Terrain>::iterator t;
	for (t = m_terrain.begin(); t != m_terrain.end(); t++)
	{
		for (a = m_actors.begin(); a != m_actors.end(); a++)
			(*t).fix(*a);
		for (b = m_bullets.begin(); b != m_bullets.end(); b++)
			if ((*t).collides(*b))
				remove_bullet(*b);
	}

	list<Hazard *>::iterator h;
	for (h = m_hazards.begin(); h != m_hazards.end(); h++)
		(*h)->move(dt);

	list<Particle *>::iterator p;
	for (p = m_particles.begin(); p != m_particles.end(); p++)
		(*p)->move(dt);
}

void Level::draw(Camera *camera) const
{
	Surface *dst = camera->get_surface();
	dst->fill(m_background);
	
	list<Hazard *>::const_iterator h;
	for (h = m_hazards.begin(); h != m_hazards.end(); h++)
		(*h)->draw(camera);
	
	list<Decoration *>::const_iterator d;
	for (d = m_decorations.begin(); d != m_decorations.end(); d++)
		(*d)->draw(camera);
	
	list<Terrain>::const_iterator t;
	for (t = m_terrain.begin(); t != m_terrain.end(); t++)
		t->draw(camera);
	
	set<Actor *>::const_iterator a;
	set<Bullet *>::const_iterator b;

	for (a = m_actors.begin(); a != m_actors.end(); a++)
		(*a)->draw(camera);
	for (b = m_bullets.begin(); b != m_bullets.end(); b++)
		(*b)->draw(camera);

	list<Particle *>::const_iterator p;
	for (p = m_particles.begin(); p != m_particles.end(); p++)
		(*p)->draw(camera);
}

bool Level::goal(Actor *actor, string *next) const
{
	Circle circle = actor->get_circle();
	list<Goal *>::const_iterator g;
	for (g = m_goals.begin(); g != m_goals.end(); g++)
	{
		if ((*g)->collides(&circle))
		{
			*next = (*g)->get_next();
			return true;
		}
	}
	
	return false;
}

void Level::add_actor(Actor *actor)
{
	m_actors_add.insert(actor);
}

void Level::add_bullet(Bullet *bullet)
{
	m_bullets_add.insert(bullet);
}

void Level::add_particle(Particle *particle)
{
	m_particles.push_back(particle);
}

void Level::remove_actor(Actor *actor)
{
	m_actors_remove.insert(actor);
}

void Level::remove_bullet(Bullet *bullet)
{
	m_bullets_remove.insert(bullet);
}

bool Level::out_of_bounds(const Bullet *bullet) const
{
	Rect bounds(0, 0, m_w, m_h);
	Shape *shape = bullet->get_shape();
	return !bounds.collides(shape);
}

void Level::get_collisions(list<Actor *> *actors,
                           const Bullet *bullet) const
{
	set<Actor *>::const_iterator a;
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		if ((*a)->get_team() != bullet->get_team())
			if (bullet->collides(*a))
				actors->push_back(*a);
}

Actor *Level::get_next_target(Actor *src, Actor *target,
                              double dist) const
{
	Actor *ret = NULL;
	set<Actor *>::const_iterator a;

	bool enemies = false;
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		if ((*a)->get_team() != src->get_team())
			if ((src->get_center() - (*a)->get_center()).norm()
			    < dist)
				enemies = true;

	if (!enemies)
		return NULL;

	bool has_target = false;
	for (a = m_actors.begin(); a != m_actors.end(); a++)
		if (*a == target)
			has_target = true;

	if (!has_target)
		target = NULL;

	double cur_dist;
	if (target == NULL)
	{
		a = m_actors.begin();
		bool first = true;
		do
		{
			if (!first)
				a++;
			first = false;
			ret = *a;
			vector2d disp = src->get_center() -	(*a)->get_center();
			cur_dist = disp.norm();
		} while ((*a)->get_team() == src->get_team() ||
		         cur_dist > dist);
	}
	else
	{
		set<Actor *>::const_iterator target_it;
		for (a = m_actors.begin(); a != m_actors.end(); a++)
			if (*a == target)
				target_it = a;

		a = target_it;
		do
		{
			a++;
			if (a == m_actors.end())
				a = m_actors.begin();
			vector2d disp = src->get_center() - (*a)->get_center();
			cur_dist = disp.norm();
		} while ((*a)->get_team() == src->get_team() ||
		         cur_dist > dist);
		ret = *a;
	}
	return ret;
}

bool Level::hazard(const Actor *actor) const
{
	list<Hazard *>::const_iterator h;
	Circle circle = actor->get_circle();
	for (h = m_hazards.begin(); h != m_hazards.end(); h++)
		if ((*h)->collides(&circle))
			return true;
	return false;
}
