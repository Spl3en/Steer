#include "Object.h"


Object *
object_new (float px, float py, float m, int t)
{
	Object *object;

	if ((object = malloc(sizeof(Object))) == NULL)
		return NULL;

    object->pos = vector2D_pos_new(px, py);
    object->mass = m;
    object->type = t;

	object->col = makecol(rand()%255, rand()%255, rand()%255);

	return object;
}

void
object_debug (Object *obj)
{
	printf("Object Debug : (%p)\n"
		   "pos = %f / %f\n"
		   "mass = %f\n"
		   "type = %d\n\n",
		   (void *)obj,
			obj->pos.x, obj->pos.y,
			obj->mass,
			obj->type
	);
}

void
object_free (Object *object)
{
	if (object != NULL)
	{
		free (object);
	}
}
