// --- Author	: Moreau Cyril - Spl3en
// --- File		: Canvas.h
// --- Date		: 2011-11-21-13.41.18
// --- Version	: 1.0

#ifndef Canvas_H_INCLUDED
#define Canvas_H_INCLUDED

// ---------- Includes ------------
#include <stdlib.h>
#include <sys\timeb.h>
#include <allegro.h>
#include <winalleg.h>

#include "World.h"
#include "../Ztring/Ztring.h"
#include "../Sleeper/Sleeper.h"

// ---------- Defines -------------
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define FULLSCREEN    0

// ------ Class declaration -------
typedef
struct _Canvas
{
	BITMAP *tracks;
	BITMAP *db;
	World *world;

	int mouse_left;
	int mouse_right;
	int mouse_left_loop;
	int mouse_right_loop;

	int f1, f2, f3, f4, f5, f6;

}	Canvas;



// --------- Constructors ---------

void
canvas_new 			(World *world, int width, int height);


// ----------- Methods ------------


void
canvas_draw_object 	(Object *obj);

void
canvas_draw_world 	(World *world);

void
canvas_draw_agent 	(Agent *agent);

void
canvas_draw_tracks ();

void
canvas_soft_tracks ();

void
canvas_clear_pass ();

void
canvas_toggle_pass ();

void
canvas_main 		(void);

void
canvas_draw_near_agent (BbQueue *agent_list);

void
canvas_get_intensity (int val, int *pR, int *pG, int *pB);


// --------- Destructors ----------

void
canvas_free ();






#endif // Canvas_INCLUDED
