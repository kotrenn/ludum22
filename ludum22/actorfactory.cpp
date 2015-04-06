#include "actorfactory.h"

ActorFactory *ActorFactory::m_instance = NULL;

ActorFactory *ActorFactory::get_instance()
{
	if (m_instance == NULL)
		m_instance = new ActorFactory();
	return m_instance;
}

ActorFactory::ActorFactory()
	:m_actors()
{
	insert(new StandActor());
	insert(new FlyActor());
	insert(new WallActor());
}

ActorFactory::~ActorFactory()
{
	map<string, Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
		delete it->second;
}

void ActorFactory::insert(Actor *actor)
{
	m_actors[actor->get_name()] = actor;
}

Actor *ActorFactory::load_actor(Level *level,
                                const char *name,
                                FILE *in)
{
	if (m_actors.find(name) == m_actors.end())
	{
		fprintf(stderr, "Error: no actor named %s\n", name);
		exit(1);
	}
	return m_actors[name]->clone(level, name, in);
}
