// --- Author	: Moreau Cyril - Spl3en
// --- File		: Agent.h
// --- Date		: 2011-11-21-10.44.28
// --- Version	: 1.0

#ifndef Agent_H_INCLUDED
#define Agent_H_INCLUDED

// ---------- Includes ------------
#include <allegro.h>
#include "../Vector/Vector2D.h"
#include "../BbQueue/BbQueue.h"
#include "maths.h"
#include "Object.h"

// ---------- Defines -------------
#define AGENT_TYPE_BOID 		1
#define AGENT_TYPE_PREDATOR 	2

#define AGENT_ACTION_WANDERING	0
#define AGENT_ACTION_FLEE		1
#define AGENT_ACTION_PURSUIT	2
#define AGENT_ACTION_FOLLOW		3

// ------ Class declaration -------
typedef
struct _Agent
{
	float mass;
	float energy;

	Vector2D pos; // Position
	Vector2D vel; // Velocity
	Vector2D acc; // Acceleration

	int type; // Agent type
	float wdelta; // Wander delta
	int action; // Current action

	struct _Agent *target; // Predator's target

	// Weights
	float wArr;
	float wDep;
	float wPur;
	float wEva;
	float wWan;
	float wAvo;
	float wFlo;
	float wCoh;
	float wSep;
	float wAli;
	float wFol;

}	Agent;



// --------- Constructors ---------

Agent *
agent_new 			(float posx, float posy, int type);


// ----------- Methods ------------

void
agent_update_weight (Agent *agent);

void
agent_steer 		(Agent *agent, BbQueue *boids, BbQueue *predators, BbQueue *objs);

void
agent_run 			(Agent *agent, BbQueue *boids, BbQueue *predators, BbQueue *objs);

void
agent_bounding 		(Agent *agent);

// --- Behaviors

void
agent_predator		(Agent *agent, BbQueue *boids);

Vector2D
agent_evade 		(Agent *agent, BbQueue *predators);

Vector2D
agent_seek 			(Agent *agent, Vector2D *target);

Vector2D
agent_flee 			(Agent *agent, Vector2D *target);

Vector2D
agent_arrival		(Agent *agent, Vector2D *target);

Vector2D
agent_departure		(Agent *agent, Vector2D *target);

Vector2D
agent_pursue		(Agent *agent);

Vector2D
agent_wander		(Agent *agent);

Vector2D
agent_avoid			(Agent *agent, BbQueue *objs);

Vector2D
agent_flocking		(Agent *agent, BbQueue *boids);

Vector2D
agent_follow 		(Agent *agent);


// Debug
void
agent_debug (Agent *agent);


// --------- Destructors ----------

void
agent_free 			(Agent *agent);






#endif // Agent_INCLUDED
