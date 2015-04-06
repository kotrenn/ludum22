#include "decoration.h"
#include "resourcemanager.h"

Decoration::Decoration(FILE *in)
	:m_pos(0, 0),
	 m_num_frames(0),
	 m_cur_frame(0),
	 m_frames(),
	 m_delay(0),
	 m_timer(0)
{
	char name[81];
	fscanf(in, "%s", name);
	
	int x, y;
	fscanf(in, "%d %d", &x, &y);
	m_pos = vector2d(x, y);

	ResourceManager *rm = ResourceManager::get_instance();
	
	fscanf(in, "%d", &m_num_frames);
	for (int i = 0; i < m_num_frames; ++i)
	{
		char filename[81];
		snprintf(filename, 80, "%s%d.png", name, i + 1);
		m_frames.push_back(rm->get_image(filename));
	}

	fscanf(in, "%lf", &m_delay);
	m_timer.reset(m_delay);
}

Decoration::~Decoration()
{
}

void Decoration::pause()
{
	m_timer.pause();
}

void Decoration::unpause()
{
	m_timer.unpause();
}

void Decoration::update()
{
	if (m_timer.done())
	{
		m_cur_frame = (m_cur_frame + 1) % m_num_frames;
		m_timer.reset(m_delay);
	}
}

void Decoration::draw(Camera *camera) const
{
	Surface *dst = camera->get_surface();
	int x = (int)(m_pos[0] - camera->get_x());
	int y = (int)(m_pos[1] - camera->get_y());
	Surface *frame = m_frames[m_cur_frame];
	dst->blit(frame, x, y);
}
