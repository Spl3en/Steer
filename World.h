// --- Author	: Moreau Cyril - Spl3en
// --- File		: World.h
// --- Date		: 2011-11-21-13.26.29
// --- Version	: 1.0

#ifndef World_H_INCLUDED
#define World_H_INCLUDED

// ---------- Includes ------------
#include <stdlib.h>
#include "../BbQueue/BbQueue.h"
#include "../Vector/Vector2D.h"
#include "maths.h"

#include "Agent.h"


// ---------- Defines -------------


// ------ Class declaration -------
typedef
struct _World
{
	BbQueue *boids;
	BbQueue *predators;
	BbQueue *objs;

	int w, h;

}	World;



// --------- Constructors ---------

World *
world_new 			(void);


// ----------- Methods ------------

void
world_update 		(World *world);

void
world_init_agents 	(World *world);

void
world_init_objs 	(World *world);


// --------- Destructors ----------

void
world_free ();






#endif // World_INCLUDED
