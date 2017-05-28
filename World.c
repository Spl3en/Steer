#include "World.h"

// --- Configuration
extern int bNum;
extern int pNum;
extern int oNum;

// --- Constructor
World *
world_new (void)
{
	World *world;

	if ((world = malloc(sizeof(World))) == NULL)
		return NULL;

	world->w = -1;
	world->h = -1;

	return world;
}

void
world_update (World *world)
{
	Agent *agent = NULL;

	// Update agents
	foreach_bbqueue_item (world->boids, agent)
	{
		agent_run (agent, world->boids, world->predators, world->objs);
	}

	foreach_bbqueue_item (world->predators, agent)
	{
		agent_run (agent, world->boids, world->predators, world->objs);
	}
}

void
world_init_agents(World *world)
{
	Agent *agent = NULL;

    // Add boids
    world->boids = bb_queue_new();

	for (int i = 0; i < bNum; i++)
	{
		agent = agent_new(rand() % world->w, rand() % world->h, AGENT_TYPE_BOID);
		bb_queue_add(world->boids, agent);
	}

	// Add predator
	world->predators = bb_queue_new();

	for (int i = 0; i < pNum; i++)
	{
		agent = agent_new(rand() % world->w, rand() % world->h, AGENT_TYPE_PREDATOR);
		bb_queue_add(world->predators, agent);
	}
}

void
world_init_objs (World *world)
{
	// Add objects
	world->objs = bb_queue_new();
	Object *obj = NULL;

	for (int i = 0; i < oNum; i++)
	{
		obj = object_new(random(1, world->w), random(1, world->h), random(30, 100), round(random(1, 2)));
		bb_queue_add(world->objs, obj);
	}
}


void
world_free (World *world)
{
	if (world != NULL)
	{
		free (world);
	}
}
