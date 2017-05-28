// --- Author	: Moreau Cyril - Spl3en
// --- File		: Graphic.h
// --- Date		: 2011-11-21-11.25.06
// --- Version	: 1.0

#ifndef Graphic_INCLUDED
#define Graphic_H_INCLUDED


// ---------- Includes ------------
#include <stdlib.h>

// ---------- Defines -------------


// ------ Class declaration -------
typedef
struct _Graphic
{

	int bitmap;


}	Graphic;



// --------- Constructors ---------

Graphic *
graphic_new (void);


// ----------- Methods ------------




// --------- Destructors ----------

void
graphic_free (Graphic *graphic);






#endif // Graphic_INCLUDED
