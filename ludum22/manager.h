#ifndef __MANAGER_H
#define __MANAGER_H

#include "camera.h"
#include "level.h"
#include "actor.h"

class Manager
{
public:
	Manager();
	~Manager();

	void key_pressed(SDLKey key, SDLMod mod);
	void mouse_event(SDL_MouseButtonEvent event);

	void pause();
	void unpause();

	void update();
	void draw(Surface *dst);
private:
	void switch_level(const string &next);
	
	Camera *m_camera;
	Level *m_level;
	PlayerActor *m_player;
	bool m_paused;
	Uint32 m_prev_move;
	Uint32 m_pause_ticks;
	Timer m_death;
	double m_delay;
	Surface *m_pause;
	Surface *m_game_over;

	bool m_animating;
	Timer m_animate;
	vector<Surface *> m_scenes;
	int m_scene;
};

#endif
