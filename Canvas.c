#include "Canvas.h"

static Canvas *this = NULL;

extern float Rn; // R neighborhood
extern float BRadius; // Avoid: radius of agent bounding sphere

BbQueue *pass_refresh = NULL;
int pass_array[SCREEN_WIDTH][SCREEN_HEIGHT];

int act_pass = 0;
int connection = 0;

void
canvas_toggle_pass ()
{
	act_pass = !act_pass;
}

void
canvas_new (World *world, int width, int height)
{
	if ((this = malloc(sizeof(Canvas))) == NULL)
		return;

	world->w = width;
	world->h = height;

	this->world = world;
	this->mouse_left = 0;
	this->mouse_right = 0;
	this->mouse_left_loop = 0;
	this->mouse_right_loop = 0;

    /* Allegro */
    allegro_init();
    set_color_depth(32);
    set_gfx_mode((FULLSCREEN) ? GFX_AUTODETECT_FULLSCREEN : GFX_AUTODETECT_WINDOWED, width, height, 0, 0);
    install_keyboard();
    install_mouse();
    show_mouse(screen);



    pass_refresh = bb_queue_new();

	this->db = create_bitmap(width, height);
	this->tracks = create_bitmap(width, height);


	canvas_clear_pass();
}

void
canvas_draw_agent (Agent *agent)
{
	int x = agent->pos.x;
	int y = agent->pos.y;

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= SCREEN_WIDTH) x = SCREEN_WIDTH - 1;
	if (y >= SCREEN_HEIGHT) y = SCREEN_HEIGHT - 1;

	POINT *point_to_refresh = malloc(sizeof(POINT));
	point_to_refresh->x = x;
	point_to_refresh->y = y;
	pass_array[x][y] = pass_array[x][y] + 1;

	bb_queue_add(pass_refresh, point_to_refresh);

    int col = (agent->type == 1) ? makecol(70, 70, 150) : makecol(255, 0, 0);

	// Neighborhood radius
	circle (this->db,
		agent->pos.x, agent->pos.y,
		agent->mass / 2,
		col
	);
	circlefill (this->db,
		agent->pos.x, agent->pos.y,
		agent->mass / 4,
		col
	);


	// Velocity
	line(this->db,
		 agent->pos.x, agent->pos.y,
		 agent->pos.x + (agent->vel.x * agent->mass / 10),
		 agent->pos.y + (agent->vel.y * agent->mass / 10),
		 makecol(255, 255, 255)
	);

	// Steering
	// line(this->db, agent->pos.x, agent->pos.y, agent->pos.x + agent->acc.x * 2, agent->pos.y + agent->acc.y * 2, makecol(0, 255, 0));
}

void
canvas_draw_object (Object *obj)
{
	int tmp = obj->mass / 2.0 - 5;
	while (tmp >= 1)
	{
		circle (this->db, obj->pos.x, obj->pos.y, tmp, obj->col);
		tmp -= 5;
	}
}

void
canvas_clear_pass ()
{
	int i, j;
	clear_bitmap(this->tracks);

	for (i = 0; i < this->world->h; i++)
	{
		for (j = 0; j < this->world->w; j++)
		{
			pass_array[j][i] = 0;
		}
	}
}

void
canvas_draw_world (World *world)
{
	Object *obj = NULL;
	Agent *agent = NULL;

	foreach_bbqueue_item (world->objs, obj)
		canvas_draw_object(obj);

	foreach_bbqueue_item (world->boids, agent)
		canvas_draw_agent(agent);

	foreach_bbqueue_item (world->predators, agent)
		canvas_draw_agent(agent);
}

void
canvas_draw_near_agent (BbQueue *agent_list)
{
	Agent *agent = NULL;
	Agent *other_agent = NULL;

	Vector2D pos;
	float dist;
	int col, r, g ,b;

	foreach_bbqueue_item (agent_list, agent)
	{
		foreach_bbqueue_item (agent_list, other_agent)
		{
			if (agent == other_agent)
				continue;

			pos = vector2D_sub (&other_agent->pos, &agent->pos);
			dist = vector2D_length(&pos);

			if (dist != 0)
			{
				canvas_get_intensity(10000 / dist, &r, &g, &b);
				col = makecol(r, g, b);

				if (dist < 100)
				{
					line (this->db,
						  other_agent->pos.x, other_agent->pos.y,
						  agent->pos.x, agent->pos.y,
						  col
					);
				}
			}
		}
	}
}

void
canvas_toggle_connection ()
{
	connection = !connection;
}

void
canvas_draw_mouse_left ()
{
	int loop = this->mouse_left_loop;
	int r = this->mouse_right_loop;
	int b = this->mouse_left_loop;

	while (loop > 0)
	{
		circle (this->db, mouse_x, mouse_y, loop, makecol(r * 2.5, 0, b * 2.5));
		b -= (b > 0) ? 10 : 0;
		r -= (r > 0) ? 10 : 0;
		loop-=10;
	}
}

void
canvas_take_screenshot ()
{
	struct timeb time;
	ftime(&time);

	char *filename = str_dup_printf("screenshot-%d%d.bmp", time.time, time.millitm);

	save_bitmap(filename, this->tracks, NULL);

	free(filename);
}

void
canvas_handle_keyboard ()
{
	// --- F1
	if (key[KEY_F1] && this->f1 != 1)
	{
		this->f1 = 1;
		canvas_clear_pass();
	}
	else if (!key[KEY_F1])
		this->f1 = 0;


	// --- F2
	if (key[KEY_F2] && this->f2 != 1)
	{
		this->f2 = 1;
		canvas_toggle_pass();
	}
	else if (!key[KEY_F2])
		this->f2 = 0;


	// --- F3
	if (key[KEY_F3] && this->f3 != 1)
	{
		this->f3 = 1;
		canvas_toggle_connection();
	}
	else if (!key[KEY_F3])
		this->f3 = 0;

	// --- F4
	if (key[KEY_F4] && this->f4 != 1)
	{
		this->f4 = 1;
		canvas_take_screenshot();
	}
	else if (!key[KEY_F4])
		this->f4 = 0;

	// --- F5
	if (key[KEY_F5] && this->f5 != 1)
	{
		this->f5 = 1;
		canvas_take_screenshot();
	}

	else if (!key[KEY_F5])
		this->f5 = 0;

	// --- F6
	if (key[KEY_F6] && this->f6 != 1)
	{
		canvas_soft_tracks();
		this->f6 = 1;
	}

	else if (!key[KEY_F6])
		this->f6 = 0;
}

void
canvas_soft_tracks ()
{
	int i, j;

	for (i = 0; i < this->world->h; i++)
	{
		for (j = 0; j < this->world->w; j++)
		{
			if (pass_array[j][i] > 0)
			{
				POINT *point_to_refresh = malloc(sizeof(POINT));
				point_to_refresh->x = j;
				point_to_refresh->y = i;

				bb_queue_add(pass_refresh, point_to_refresh);

				pass_array[j][i] = pass_array[j][i] - 1;
			}
		}
	}

}

void
canvas_main ()
{
	struct timeb t_start, t_current;
	struct timeb t_start_logic, t_current_logic;
	int t_diff;
	int t_diff_logic;

	// World initialisation
	world_init_agents(this->world);
	world_init_objs(this->world);

	ftime(&t_start);
	ftime(&t_start_logic);

	while (!key[KEY_ESC])
	{
		ftime(&t_current);
		ftime(&t_current_logic);
		t_diff = (int) (1000.0 * (t_current.time - t_start.time) + (t_current.millitm - t_start.millitm));
		t_diff_logic = (int) (1000.0 * (t_current_logic.time - t_start_logic.time) + (t_current_logic.millitm - t_start_logic.millitm));

		canvas_handle_keyboard();

		if (t_diff_logic > 1000.0 / 50)
		{
			ftime(&t_start_logic);
			world_update(this->world);
		}

		if (t_diff > 1000.0 / 100.0)
		{
			ftime(&t_start);

			(act_pass) ? canvas_draw_tracks() : clear_bitmap(this->db);

			if (mouse_b & 1 && this->mouse_left != 1)
			{
				this->mouse_left = 1;
				this->mouse_left_loop = 100;
			}
			else if (!(mouse_b & 1))
				this->mouse_left = 0;


			if (this->mouse_left_loop > 0)
				this->mouse_left_loop--;

			if (this->mouse_left_loop == 0 && this->mouse_left)
				this->mouse_left_loop = 100;

			canvas_draw_mouse_left();

			canvas_draw_world(this->world);

			if (connection)
				canvas_draw_near_agent(this->world->boids);

			blit(this->db, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		}

		sleep(1);
	}
}

void
canvas_get_intensity (int val, int *pR, int *pG, int *pB)
{
	int r = val, g = 0, b = 0;

	if (r > 255)
	{
		g = r - 255;
		r = 255;
		if (g > 255)
		{
			b = g - 255;
			g = 255;

			if (b > 255)
				b = 255;
		}
	}

	*pR = r;
	*pG = g;
	*pB = b;
}

void
canvas_draw_tracks ()
{
	int i, f = 0, t = 0, h = 0;
	const int coef = 50;
	POINT *p = NULL;

	int len = bb_queue_get_length(pass_refresh);

	for (i = 0; i < len; i++)
	{
		p = bb_queue_pop(pass_refresh);
		f = pass_array[p->x][p->y];

		f *= coef;

		canvas_get_intensity(f, &f, &t, &h);

		_putpixel32(this->tracks, p->x, p->y, makecol(f, t, h));

		free(p);

		t = 0;
		h = 0;
	}

	blit(this->tracks, this->db, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void
canvas_free ()
{
	if (this != NULL)
	{
		free (this);
	}
}
