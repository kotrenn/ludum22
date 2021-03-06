#include "common.h"

#include "actorfactory.h"
#include "hazardfactory.h"
#include "datamanager.h"
#include "resourcemanager.h"

#include "manager.h"
#include "surface.h"

int main(int, char **)
{
	srand(time(0));

	int width = 640, height = 480;
	int size[] = { width, height };
	bool fullscreen = false;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
	                    SDL_DEFAULT_REPEAT_INTERVAL);

	int displayFlags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	SDL_Surface *sdl_screen = SDL_SetVideoMode(size[0], size[1], 32,
	                                           displayFlags);
	Surface *screen = new Surface(sdl_screen);
	sdl_screen->refcount--;

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
		exit(1);

	SDL_WM_SetCaption("Varon Sector", 0);

	SDL_Event event;

	DataManager *dm = DataManager::get_instance();
	ResourceManager *rm = ResourceManager::get_instance();
	ActorFactory *af = ActorFactory::get_instance();
	HazardFactory *hf = HazardFactory::get_instance();

	Manager *manager = new Manager();

	int frame = 0;
	Uint32 total = SDL_GetTicks();

	bool running = true;
	while (running)
	{
		if (SDL_GetTicks() - total >= 1000)
		{
			total = SDL_GetTicks();
			frame = 0;
		}
	
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					running = false;
				else if (event.key.keysym.sym == SDLK_RETURN &&
				         (event.key.keysym.mod & (KMOD_LALT |
				                                  KMOD_RALT)))
				{
					int flags = displayFlags;
					fullscreen = !fullscreen;
					if (fullscreen)
						flags |= SDL_FULLSCREEN;
					delete screen;
					SDL_FreeSurface(sdl_screen);
					sdl_screen = SDL_SetVideoMode(size[0], size[1],
					                              32, flags);
					screen = new Surface(sdl_screen);
				}
				manager->key_pressed(event.key.keysym.sym,
				                     event.key.keysym.mod);
			}
		}

		// update here
		manager->update();

		screen->fill(0);

		// draw here
		manager->draw(screen);

		char fps_string[80];
		snprintf(fps_string, 80, "fps: %lf",
		         frame / ((SDL_GetTicks() - total) / 1000.0));
		//screen->draw_string(fps_string, mapRGB(255, 255, 255),
		//                    5, 455);
		frame++;
	
		screen->flip();
	}

	delete manager;
	delete hf;
	delete af;
	delete rm;
	delete dm;
	delete screen;
	SDL_FreeSurface(sdl_screen);
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}
