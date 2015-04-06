#include "manager.h"
#include "datamanager.h"
#include "resourcemanager.h"

Manager::Manager()
	:m_camera(NULL),
	 m_level(NULL),
	 m_player(NULL),
	 m_paused(false),
	 m_prev_move(SDL_GetTicks()),
	 m_pause_ticks(0),
	 m_death(0),
	 m_delay(8.0),
	 m_pause(NULL),
	 m_game_over(NULL),
	 m_animating(true),
	 m_animate(0),
	 m_scenes(),
	 m_scene(0)
{
	DataManager *dm = DataManager::get_instance();

	char level[81];
	dm->get_str("start", level);
	m_level = new Level(level);
	
	vector2d size = dm->get_vec("viewport");
	m_camera = new Camera((int)size[0],
	                      (int)size[1],
	                      m_level->get_w(),
	                      m_level->get_h());
	
	m_player = new PlayerActor(m_level);
	m_player->set_center(m_level->get_start());
	m_level->add_actor(m_player);
	m_camera->set_target(m_player);

	ResourceManager *rm = ResourceManager::get_instance();
	
	m_pause = rm->get_image("paused.png");
	m_game_over = rm->get_image("game_over.png");

	m_scenes.push_back(NULL);
	for (int i = 1; i <= 7; ++i)
	{
		char name[81];
		snprintf(name, 81, "scene%d.png", i);
		m_scenes.push_back(rm->get_image(name));
	}
}

Manager::~Manager()
{
	delete m_level;
	delete m_camera;
	delete m_player;
}

void Manager::key_pressed(SDLKey key, SDLMod)
{
	if (key == SDLK_p && !m_animating)
	{
		if (m_paused)
			unpause();
		else
			pause();
	}
}

void Manager::mouse_event(SDL_MouseButtonEvent)
{
}

void Manager::pause()
{
	m_paused = true;
	m_level->pause();
	m_pause_ticks = SDL_GetTicks() - m_prev_move;
}

void Manager::unpause()
{
	m_paused = false;
	m_level->unpause();
	m_prev_move = SDL_GetTicks() - m_pause_ticks;
	m_pause_ticks = 0;
}

void Manager::update()
{
	if (m_paused)
		return;

	DataManager *dm = DataManager::get_instance();

	if (m_animating)
	{
		Uint8 *keys = SDL_GetKeyState(NULL);
		if (m_animate.done() || keys[SDLK_SPACE])
		{
			m_scene++;
			m_animate.reset(dm->get_dbl("anim_delay"));
			if (m_scene == 6 || m_scene == 8)
			{
				char start[81];
				dm->get_str("start", start);
				if (m_scene == 6)
					switch_level(start);
				else
					switch_level("final");
				m_animating = false;
			}
		}

		m_prev_move = SDL_GetTicks();
		
		return;
	}
	
	m_level->update();

	if (m_level->hazard(m_player))
		m_player->damage();

	string next;
	if (m_level->goal(m_player, &next))
		switch_level(next);

	if (m_level->get_name() == "final")
	{
		if (m_death.done())
		{
			vector2d pos(0, 0);
			if (randint(1, 2) == 1)
				pos = vector2d(70, 20);
			else
				pos = vector2d(580, 20);
			Actor *actor = new StandActor(m_level, "stand",
			                              pos);
			m_level->add_actor(actor);
			m_death.reset(m_delay);
			m_delay = max(1.0, m_delay - 0.3);
		}
	}

	Uint32 cur_time = SDL_GetTicks();
	double dt = (cur_time - m_prev_move) / 1000.0;
	dt = min(dt, 0.05);
	m_level->move(dt);
	m_prev_move = cur_time;

	m_camera->move();
}

void Manager::draw(Surface *dst)
{
	if (m_animating)
	{
		Surface *scene = m_scenes[m_scene];
		dst->blit(scene, 0, 0);

		return;

		// This is stuff for fading in and out
		// of scenes.  Currently alpha blending
		// doesn't work directly on the screen
		// surface.
		DataManager *dm = DataManager::get_instance();
		double delay = dm->get_dbl("anim_delay");
		double lower = 0.1 * delay;
		double upper = 0.9 * delay;
		double t = m_animate.get_time();
		double alpha = 1.0;
		if (t < lower)
			alpha = t / (0.1 * delay);
		if (t > upper)
		{
			alpha = (t - upper) / (0.1 * delay);
			alpha = 1.0 - alpha;
		}
		if (alpha < 1.0)
			dst->draw_rect(mapRGBA(0, 0, 0, (int)(255 * alpha)),
			               Rect(0, 0, dst->get_w(), dst->get_h()));

		return;
	}
	
	m_camera->clear();
	m_level->draw(m_camera);
	m_camera->draw(dst, 0, 0);

	if (m_player->dead())
		dst->blit(m_game_over, 0, 0);
	else if (m_paused)
		dst->blit(m_pause, 0, 0);
}

void Manager::switch_level(const string &next)
{
	DataManager *dm = DataManager::get_instance();
	
	if (next == "final" && !m_animating)
	{
		m_animating = true;
		m_animate.reset(dm->get_dbl("anim_delay"));
	}
	
	delete m_level;
	m_level = new Level(next.c_str());
	m_camera->set_space_w(m_level->get_w());
	m_camera->set_space_h(m_level->get_h());
	m_camera->set_absolute(m_level->get_start());

	m_player->set_level(m_level);
	m_player->set_center(m_level->get_start());
	m_level->add_actor(m_player);

	char start[81];
	dm->get_str("start", start);
	if (next == start)
	{
		ResourceManager *rm = ResourceManager::get_instance();
		rm->play_sound("begin.wav");
	}
}
