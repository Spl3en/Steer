#include "Graphic.h"


Graphic *
graphic_new (void)
{
	Graphic *graphic;

	if ((graphic = malloc(sizeof(Graphic))) == NULL)
		return NULL;



	return graphic;
}




void
graphic_free (Graphic *graphic)
{
	if (graphic != NULL)
	{
		free (graphic);
	}
}
