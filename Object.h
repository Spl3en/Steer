// --- Author	: Moreau Cyril - Spl3en
// --- File		: Object.h
// --- Date		: 2011-11-21-12.48.50
// --- Version	: 1.0

#ifndef Object_H_INCLUDED
#define Object_H_INCLUDED

// ---------- Includes ------------
#include <stdlib.h>
#include <allegro.h>
#include "../Vector/Vector2D.h"

// ---------- Defines -------------
#define OBJECT_TYPE_1	1
#define OBJECT_TYPE_2	2

// ------ Class declaration -------
typedef
struct _Object
{
	Vector2D pos;
	float mass;
	int type;

	int col;

}	Object;



// --------- Constructors ---------

Object *
object_new (float px, float py, float m, int t);


// ----------- Methods ------------




void
object_debug (Object *obj);

// --------- Destructors ----------

void
object_free (Object *object);






#endif // Object_INCLUDED
