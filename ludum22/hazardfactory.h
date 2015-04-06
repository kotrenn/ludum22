#ifndef __HAZARD_FACTORY_H
#define __HAZARD_FACTORY_H

#include "hazard.h"

class HazardFactory
{
public:
	static HazardFactory *get_instance();
	~HazardFactory();

	Hazard *load_hazard(const char *name, FILE *in,
	                    map<string, Uint32> &colors);
private:
	HazardFactory();
	static HazardFactory *m_instance;

	void insert(Hazard *hazard);

	map<string, Hazard *> m_hazards;
};

#endif
