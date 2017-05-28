#include "Canvas.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(NULL));

	World *world = world_new();
	canvas_new(world, SCREEN_WIDTH, SCREEN_HEIGHT);

	canvas_main();

    return 0;
}
END_OF_MAIN();
