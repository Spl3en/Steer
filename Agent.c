#include "Agent.h"

// World
int bNum = 200;
int pNum = 10;
int oNum = 20;

// Canvas
int bounded = 0; // Bounded within world

// Agent
float Rn 		= 1.0f; // R neighborhood
float maxspeed  = 5.0f; // Maximum speed
float maxforce  = 3.0f; // Maximum steering force
float maxpursue = 4.0f; // Maximum steering force
float maxevade  = 2.0f; // Maximum steering force
float maxfollow	= 2.0f; // Maximum steering force

// Steering
float ARadius 	= 100.0f; // Arrival: max distance at which the agent may begin to slow down
float ERadius 	= 50.0f; // Evade: radius of evade range
float WRadius 	= 40.0f; // Wander: radius of wandering circle
float NRadius 	= 0.3f; // Wander: radius of wander noise circle
float BRadius 	= 40.0f; // Avoid: radius of agent bounding sphere
float ORadius 	= 10.0f; // Avoid: radius of object bounding sphere
float CStep 	= 1.0f / 100.0f; // Cohesion: move it #% of the way towards the center
float SDistance = 80.0f; // Separation: small separation distance
float AVelocity = 1.0f / 8.0f; // Alignment: add a small portion to the velocity

// Weights
float KArrive 	 = 1.0f;
float KDepart 	 = 1.0f;
float KPursue 	 = 1.0f;
float KEvade  	 = 5.0f;
float KWander 	 = 3.0f;
float KAvoid  	 = 10.0f;
float KFlock  	 = 1.0f;
float KCohesion  = 3.0f;
float KSeparate  = 1.0f;
float KAlignment = 1.0f;
float KFollow	 = 1.0f;

Agent *
agent_new (float posx, float posy, int type)
{
	Agent *agent;

	if ((agent = malloc(sizeof(Agent))) == NULL)
		return NULL;

    agent->mass = 6.0;
    agent->energy = 10 * ceil(random(0, 20));

    agent->pos = vector2D_pos_new(posx, posy);
    agent->vel = vector2D_pos_new(random(-5, 5), random(-5, 5));

    agent->acc = vector2D_new();

    agent->type = type;
    agent->wdelta = 0.0;
    agent->action = AGENT_ACTION_WANDERING;

    agent->target = NULL;

    agent_update_weight(agent);

	return agent;
}

void
agent_steer (Agent *agent, BbQueue *boids, BbQueue *predators, BbQueue *objs)
{
	if (agent->type == AGENT_TYPE_PREDATOR)
		agent_predator(agent, boids); // Determine current action

	// Initialize steering forces
	Vector2D arr = vector2D_new();
	Vector2D dep = vector2D_new();
	Vector2D pur = vector2D_new();
	Vector2D eva = vector2D_new();
	Vector2D wan = vector2D_new();
	Vector2D flo = vector2D_new();
	Vector2D avo = vector2D_new();
	Vector2D fol = vector2D_new();
	Vector2D mouse = vector2D_new();

	// Calculate steering forces
	switch (agent->action)
	{
		// Evading
		case AGENT_ACTION_FLEE:
			avo = agent_avoid(agent, objs);
			eva = agent_evade(agent, predators);
		break;

		// Pursuing
		case AGENT_ACTION_PURSUIT:
			avo = agent_avoid(agent, objs);
			pur = agent_pursue(agent);
		break;

		case AGENT_ACTION_FOLLOW:
			avo = agent_avoid(agent, objs);
			fol = agent_follow(agent);
		break;

		// Wandering
		default:
			if (agent->type == AGENT_TYPE_BOID)
			{
				wan = agent_wander(agent);
				avo = agent_avoid(agent, objs);
				flo = agent_flocking(agent, boids);
				eva = agent_evade(agent, predators);
			}
			else if (agent->type == AGENT_TYPE_PREDATOR)
			{
				wan = agent_wander(agent);
				avo = agent_avoid(agent, objs);
			}
		break;
	}

	// User interaction
	if (mouse_b != 0 && agent->type == AGENT_TYPE_BOID)
	{
		// Left mouse button - Arrival
		if (mouse_b & 1)
		{
			mouse = vector2D_pos_new(mouse_x, mouse_y);
			arr = agent_arrival(agent, &mouse);
		}

		// Right mouse button - Departure
		else if (mouse_b & 2)
		{
			mouse = vector2D_pos_new(mouse_x, mouse_y);
			dep = agent_departure(agent, &mouse);
			vector2D_sscalar(&dep, maxevade);
		}
	}

	// Apply weights
	vector2D_sscalar(&arr, agent->wArr);
	vector2D_sscalar(&dep, agent->wDep);
	vector2D_sscalar(&pur, agent->wPur);
	vector2D_sscalar(&eva, agent->wEva);
	vector2D_sscalar(&wan, agent->wWan);
	vector2D_sscalar(&avo, agent->wAvo);
	vector2D_sscalar(&flo, agent->wFlo);
	vector2D_sscalar(&fol, agent->wFol);

	// Accumulate steering force
	vector2D_sadd(&agent->acc, &arr);
	vector2D_sadd(&agent->acc, &dep);
	vector2D_sadd(&agent->acc, &pur);
	vector2D_sadd(&agent->acc, &eva);
	vector2D_sadd(&agent->acc, &wan);
	vector2D_sadd(&agent->acc, &avo);
	vector2D_sadd(&agent->acc, &flo);
	vector2D_sadd(&agent->acc, &fol);

	// Limit to maximum steering force
	vector2D_slimit(&agent->acc, maxforce);
}

void
agent_bounding (Agent *agent)
{
	if (bounded)
	{
		if (agent->pos.x <= BRadius)
			agent->vel.x = BRadius - agent->pos.x;

		else if (agent->pos.x >= SCREEN_W - BRadius)
			agent->vel.x = (SCREEN_W - BRadius) - agent->pos.x;

		if (agent->pos.y <= BRadius)
			agent->vel.y = BRadius - agent->pos.y;

		else if (agent->pos.y >= SCREEN_H - BRadius)
			agent->vel.y = (SCREEN_H - BRadius) - agent->pos.y;
	}

	else
	{
		if (agent->pos.x < -agent->mass)
			agent->pos.x = SCREEN_W + agent->mass;

		if (agent->pos.y < -agent->mass)
			agent->pos.y = SCREEN_H + agent->mass;

		if (agent->pos.x > SCREEN_W + agent->mass)
			agent->pos.x = -agent->mass;

		if (agent->pos.y > SCREEN_H + agent->mass)
			agent->pos.y = -agent->mass;
	}
}

void
agent_run (Agent *agent, BbQueue *boids, BbQueue *predators, BbQueue *objs)
{
	vector2D_set_pos(&agent->acc, 0.0, 0.0); // Reset accelertion to 0 each cycle
	agent_steer(agent, boids, predators, objs); // Update steering with approprate behavior

	vector2D_sadd(&agent->vel, &agent->acc); // Update velocity

	switch (agent->action)
	{
		case AGENT_ACTION_FLEE:
			vector2D_slimit(&agent->vel, maxpursue);
		break; // Limit pursue speed

		case AGENT_ACTION_PURSUIT:
			vector2D_slimit(&agent->vel, maxevade);
		break; // Limit evade speed

		case AGENT_ACTION_FOLLOW:
			vector2D_slimit(&agent->vel, maxfollow);
		break; // Limit follow speed

		default:
			vector2D_slimit(&agent->vel, maxspeed);
		break; // Limit speed
	}

	vector2D_sadd(&agent->pos, &agent->vel); // Move agent

	agent_bounding(agent); // Wrap around the screen or else...
	agent_update_weight(agent); // Updates weights
}

void
agent_update_weight (Agent *agent)
{
	agent->wArr = KArrive;
	agent->wDep = KDepart;
	agent->wPur = KPursue;
	agent->wEva = KEvade;
	agent->wWan = KWander;
	agent->wAvo = KAvoid;
	agent->wFlo = KFlock;
	agent->wCoh = KCohesion;
	agent->wSep = KSeparate;
	agent->wAli = KAlignment;
	agent->wFol = KFollow;
}

/* Behaviors */

void
agent_predator (Agent *agent, BbQueue *boids)
{
	Agent *boid = NULL;
	float distance;
	Vector2D v;

	if (agent->energy > 0)
		agent->energy -= random(0, 1);

	if (agent->energy < 0)
		agent->energy = 10.0 * (float)ceil(random(5, 10));

	if (agent->energy < 20
	&&  agent->action == AGENT_ACTION_WANDERING)
	{
		agent->target = NULL;

		foreach_bbqueue_item (boids, boid)
		{
			v = vector2D_sub (&boid->pos, &agent->pos);
			distance = vector2D_length(&v);

			if (distance < 150)
			{
				agent->action = AGENT_ACTION_PURSUIT;
				agent->target = boid;
				break_foreach;
			}
		}
	}

	if (agent->energy > 20
	&&  agent->action == AGENT_ACTION_PURSUIT)
	{
		agent->action = AGENT_ACTION_WANDERING;
		agent->target = NULL;
	}
}

Vector2D
agent_follow (Agent *agent)
{

}

Vector2D
agent_evade (Agent *agent, BbQueue *predators)
{
	Vector2D steer = vector2D_new();
	Agent *predator = NULL;

	foreach_bbqueue_item (predators, predator)
	{
		float distance = vector2D_distance(&agent->pos, &predator->pos);

		if (distance < pow(ERadius, 2))
		{
			agent->action = AGENT_ACTION_FLEE;

			steer = agent_flee(agent, &predator->pos);
			vector2D_sscalar(&steer, maxevade);

			return steer;
		}
	}

	agent->action = AGENT_ACTION_WANDERING;

	return steer;
}


Vector2D
agent_seek (Agent *agent, Vector2D *target)
{
	Vector2D steer; // The steering vector
	Vector2D desired = vector2D_sub(target, &agent->pos); // A vector pointing from current location to the target
	float distance = vector2D_length(&desired); // Distance from the target is the magnitude of the vector

	// If the distance is greater than 0, calc steering (otherwise return zero vector)
	if (distance > 0)
	{
		vector2D_snormalize(&desired); // Normalize desired
		vector2D_sscalar(&desired, maxforce);

		steer = vector2D_sub(&desired, &agent->vel); // Steering = Desired minus Velocity
	}

	else
		steer = vector2D_new();

	return steer;
}

Vector2D
agent_flee (Agent *agent, Vector2D *target)
{
	Vector2D steer; // The steering vector
	Vector2D desired = vector2D_sub(target, &agent->pos); // A vector pointing from current location to the target
	float distance = vector2D_length(&desired); // Distance from the target is the magnitude of the vector

	// If the distance is greater than 0, calc steering (otherwise return zero vector)
	if (distance > 0
	&&  distance < pow(ARadius * 100, 2)
	)
	{
		vector2D_snormalize(&desired); // Normalize desired
		vector2D_sscalar(&desired, maxforce);

		steer = vector2D_sub(&agent->vel, &desired); // Steering = Desired minus Velocity
	}

	else
		steer = vector2D_new();

	return steer;
}

Vector2D
agent_arrival (Agent *agent, Vector2D *target)
{
	Vector2D steer; // The steering vector
	Vector2D desired = vector2D_sub(target, &agent->pos); // A vector pointing from current location to the target
	float distance = vector2D_length(&desired); // Distance from the target is the magnitude of the vector

	// If the distance is greater than 0, calc steering (otherwise return zero vector)
	if (distance > 0)
	{
		vector2D_snormalize(&desired); // Normalize desired

		if (distance < pow(ARadius, 2))
		{
			vector2D_sscalar(&desired, maxspeed * (float)(distance / (float)ARadius)); // This damping is somewhat arbitrary
		}

		else
			vector2D_sscalar(&desired, maxforce);

		steer = vector2D_sub(&desired, &agent->vel); // Steering = Desired minus Velocity
	}

	else
		steer = vector2D_new();

	return steer;
}

Vector2D
agent_departure (Agent *agent, Vector2D *target)
{
	Vector2D steer; // The steering vector
	Vector2D desired = vector2D_sub(target, &agent->pos); // A vector pointing from current location to the target
	float distance = vector2D_length(&desired); // Distance from the target is the magnitude of the vector

	// If the distance is greater than 0, calc steering (otherwise return zero vector)
	if (distance > 0
	&&  distance < ARadius * 2
	)
	{
		vector2D_snormalize(&desired); // Normalize desired

		if (distance < pow(ARadius, 2))
		{
			vector2D_sscalar(&desired, maxspeed * (ARadius / distance)); // This damping is somewhat arbitrary
		}

		else
			vector2D_sscalar(&desired, maxforce);

		steer = vector2D_sub(&agent->vel, &desired); // Steering = Desired minus Velocity
	}

	else
		steer = vector2D_new();

	return steer;
}

Vector2D
agent_pursue (Agent *agent)
{
	Vector2D steer;
	Vector2D desired = vector2D_sub(&agent->pos, &agent->pos);
	float distance = vector2D_length(&desired);

	if (distance > 0)
	{
		vector2D_snormalize(&desired);

		if (distance < pow(ARadius, 2))
		{
			distance = distance / 2;
			vector2D_sscalar(&desired, maxspeed);
		}

		else
			vector2D_sscalar(&desired, maxforce);

		steer = vector2D_sub(&desired, &agent->vel);
	}

	else
		steer = vector2D_new();

	return steer;
}

Vector2D
agent_wander(Agent *agent)
{
	agent->wdelta += random(-NRadius, NRadius); // Determine noise ratio

	// Calculate the new location to steer towards on the wander circle
	Vector2D center = agent->vel; // Get center of wander circle
	//vector2D_sscalar(&center, 60.0f); // Multiply by distance
	vector2D_sadd(&center, &agent->pos); // Make it relative to boid's location

	// Apply offset to get new target
	Vector2D offset = vector2D_pos_new(WRadius * cos(agent->wdelta),
									   WRadius * sin(agent->wdelta));

	Vector2D target = vector2D_add(&center, &offset); // Determine new target

	// Steer toward new target
	Vector2D steer = agent_seek(agent, &target); // Steer towards it

	return steer;
}

Vector2D
agent_avoid (Agent *agent, BbQueue *objs)
{
	Vector2D steer  = vector2D_new();
	Vector2D tmp;
	Object *obj = NULL;

	foreach_bbqueue_item (objs, obj)
	{
		// Distance between object and avoidance sphere
		float distance = vector2D_distance(&obj->pos, &agent->pos);
		// If distance is less than the sum of the two radius, there is collision
		float bound = (obj->mass * 0.5) + BRadius + ORadius;

		if (distance < pow(bound, 2))
		{
			agent->wAvo = 10.0f;
			agent->wWan = 0.1f;

			float collision = (obj->mass + agent->mass) * 0.5f;

			if (distance < pow(collision, 2))
			{
				steer = vector2D_sub(&agent->pos, &obj->pos);
				vector2D_sscalar(&steer, maxforce * 0.1f);

				return steer;
			}

			else
			{
				tmp = vector2D_add(&agent->pos, &agent->vel);
				float direction = vector2D_distance(&obj->pos, &tmp);

				// If is heading toward obstacle
				if (direction < distance)
				{
					// If steering in the verticle direction
					if (abs(agent->vel.x) <= abs(agent->vel.y))
						steer = vector2D_pos_new((agent->pos.x - obj->pos.x), agent->vel.y);

					else // If steering in the horizontal direction
						steer = vector2D_pos_new(agent->vel.x, (agent->pos.y - obj->pos.y));

					vector2D_sscalar(&steer, (maxforce * (float)pow(bound, 2) / (float)distance) * 0.001f);
				}
			}
		}
	}

	return steer;
}

Vector2D
agent_flocking (Agent *agent, BbQueue *boids)
{
	// Get steering forces
	Vector2D steer = vector2D_new();
	Vector2D coh = vector2D_new(); // Perceived center
	Vector2D sep = vector2D_new(); // Displacement
	Vector2D ali = vector2D_new(); // Perceived velocity

	Agent *boid = NULL;

	int count = 0;
	// Agents try to fly towards the centre of mass of neighbouring agents
	// Agents try to keep a small distance away from other objects (including other agents)
	// Agents try to match velocity with near agents

	foreach_bbqueue_item(boids, boid)
	{
		float distance = vector2D_distance(&agent->pos, &boid->pos);

		// Go through each agents
		if (agent != boid && distance < pow(Rn, 2))
		{
			vector2D_sadd(&coh, &boid->pos); // Cohesion
			vector2D_sadd(&ali, &boid->vel); // Alignment
			count++;
		}

		// Separation
		if (agent != boid && distance < pow(SDistance, 2))
		{
			Vector2D diff = vector2D_sub(&boid->pos, &agent->pos); // (agent.position - bJ.position)
			vector2D_snormalize(&diff);
			distance = distance / Rn;
			vector2D_sscalar(&diff, 1.0 / distance); // Weighed by distance
			vector2D_ssub(&sep, &diff); // c = c - (agent.position - bJ.position)
		}
	}

	if (count > 0)
	{
		// Cohesion - Step towards the center of mass
		vector2D_sscalar(&coh, 1.0 / (float)count); // cJ = pc / (N-1)
		vector2D_ssub(&coh, &agent->pos); // (pcJ - bJ.position)
		vector2D_sscalar(&coh, CStep); // (pcJ - bJ.position) / 100

		// Alignment - Find average velocity
		vector2D_sscalar(&ali, 1.0 / (float)count); // pvJ = pvJ / N-1
		vector2D_ssub(&ali, &agent->vel); // (pvJ - bJ.velocity)
		vector2D_sscalar(&ali, AVelocity); // (pvJ - bJ.velocity) / 8
	}

	// Apply weights
	vector2D_sscalar(&coh, agent->wCoh);
	vector2D_sscalar(&sep, agent->wSep);
	vector2D_sscalar(&ali, agent->wAli);

	// Accumulate forces
	vector2D_sadd(&steer, &coh);
	vector2D_sadd(&steer, &sep);
	vector2D_sadd(&steer, &ali);

	// Add speed
	vector2D_sscalar(&steer, maxspeed);

	return steer;
}

void
agent_debug (Agent *agent)
{
	printf("Agent debug : %p\n"
			"pos = %f / %f\n\n",
			agent,
			agent->pos.x, agent->pos.y
	);
}

void
agent_free (Agent *agent)
{
	if (agent != NULL)
	{
		free (agent);
	}
}
