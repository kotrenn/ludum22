#include "hazardfactory.h"

HazardFactory *HazardFactory::m_instance = NULL;

HazardFactory *HazardFactory::get_instance()
{
	if (m_instance == NULL)
		m_instance = new HazardFactory();
	return m_instance;
}

HazardFactory::HazardFactory()
	:m_hazards()
{
	insert(new RectHazard());
	insert(new SpikeHazard());
}

HazardFactory::~HazardFactory()
{
	map<string, Hazard *>::iterator it;
	for (it = m_hazards.begin(); it != m_hazards.end(); it++)
		delete it->second;
}

void HazardFactory::insert(Hazard *hazard)
{
	m_hazards[hazard->get_name()] = hazard;
}

Hazard *HazardFactory::load_hazard(const char *name, FILE *in,
                                   map<string, Uint32> &colors)
{
	if (m_hazards.find(name) == m_hazards.end())
	{
		fprintf(stderr, "Error: no actor named %s\n", name);
		exit(1);
	}
	return m_hazards[name]->clone(in, colors);
}
