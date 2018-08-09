/*  Quick and dirty implementation of a Traveling Salesman Problem solver.
 *  Copyright (C) 2018 - Jérôme Kirman
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "tsp.h"
#include "solver.h"

unsigned echo_fps (unsigned interval, void* param)
{
	int* pframes = param;
	printf("%i FPS\n", *pframes);
	*pframes = 0;
	return interval;
}

dist pic_w, pic_h;
unsigned win_w, win_h;

double x2w (dist x) { return win_w * x/pic_w; }
double y2h (dist y) { return win_h * y/pic_h; }
dist w2x (double w) { return pic_w * w/win_w ; }
dist h2y (double h) { return pic_h * h/win_h; }

id near_city (Sint32 w, Sint32 h, map* m)
{
	id nid = -1;
	double nd = 24 * pic_w / win_w;

	for (id i = 0 ; i < m->size ; ++i)
	{
		dist d = distance(w2x(w), h2y(h), m->towns[i].x, m->towns[i].y);
		if (d < nd)
		{
			nd = d;
			nid = i;
		}
	}

	return nid;
}

SDL_Texture* render_length (SDL_Renderer* rd, TTF_Font* f, SDL_Texture* t, SDL_Rect* r, dist l)
{
	SDL_Color c = {255,255,0,0};
	char txt[16];
	if (t)
		SDL_DestroyTexture(t);
	snprintf(txt, 15, "%.2f", l);
	SDL_Surface* s = TTF_RenderText_Blended(f, txt, c);
	t = SDL_CreateTextureFromSurface(rd, s);
	SDL_FreeSurface(s);
	SDL_QueryTexture(t, NULL, NULL, &r->w, &r->h);
	return t;
}

struct instance_s
{
	char* name;
	char* picfile;
	unsigned w, h;
	unsigned nbt;
	city* towns;
};

#define BUFSIZE (1 << 16)
#define MAXTOWNS 256

struct instance_s parse_instance (char* filename)
{
	char* buf = calloc(BUFSIZE, sizeof(char));
	int fd = open(filename, O_RDONLY);
	size_t len = read(fd, buf, BUFSIZE-1);
	close(fd);

	size_t marks[5 + 3*MAXTOWNS];
	unsigned m = 0;
	marks[m++] = 0;
	for (size_t i = 0 ; i < len ; ++i)
		if (buf[i] == '\n' || buf[i] == '\t')
		{
			buf[i] = '\0';
			marks[m++] = i+1;
		}

	assert(m >= 5);

	struct instance_s inst;
	inst.name = calloc(strlen(buf + marks[0]), sizeof(char));
	strcpy(inst.name, buf + marks[0]);
	inst.picfile = calloc(strlen(buf + marks[1]), sizeof(char));
	strcpy(inst.picfile, buf + marks[1]);
	inst.w = atoi(buf + marks[2]);
	inst.h = atoi(buf + marks[3]);
	inst.nbt = atoi(buf + marks[4]);

	assert(m >= 5 + 3*inst.nbt);

	inst.towns = calloc(inst.nbt, sizeof(city));
	for (unsigned t = 0 ; t < inst.nbt ; ++t)
	{
		inst.towns[t].name = calloc(strlen(buf + marks[5+3*t]), sizeof(char));
		strcpy(inst.towns[t].name, buf + marks[5+3*t]);
		inst.towns[t].x = atof(buf + marks[5+3*t+1]);
		inst.towns[t].y = atof(buf + marks[5+3*t+2]);
	}

	free(buf);
	return inst;
}

int main (int argc, char* argv[])
{
	chdir("./assets/");

	// Load instance data
	struct instance_s inst;
	if (argc == 1)
		inst = parse_instance("france.txt");
	else
		inst = parse_instance(argv[1]);

	win_h = 1000;
	win_w = (dist) win_h * inst.w/inst.h;

	// Create and solve TSP instance
	map* map = make_map(inst.towns, inst.nbt);
	run* best_run = bruteforce(map);
	run* nn_run = nearest_neighbor(map);

	for (id i = 0 ; i < map->size ; i++)
		printf("%s ", map->towns[best_run->path[i]].name);
	printf("%g \n", best_run->length);

	for (id i = 0 ; i < map->size ; i++)
		printf("%s ", map->towns[nn_run->path[i]].name);
	printf("%g \n", nn_run->length);


	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	TTF_Font* font = TTF_OpenFont("./free_mono.ttf", 16);
	printf("%s\n", TTF_GetError());

	SDL_Event e;
	int frames = 0;
	SDL_AddTimer(1000, echo_fps, &frames);

	SDL_Window* mw = SDL_CreateWindow(inst.name, 0, 0, win_w, win_h, 0);
	SDL_Renderer* canvas = SDL_CreateRenderer(mw, -1, 0);
	SDL_Rect scr = {0, 0, win_w, win_h};

	// Background image
	pic_w = inst.w;
	pic_h = inst.h;
	SDL_Surface* bgs = IMG_Load(inst.picfile);
	SDL_Texture* bg = SDL_CreateTextureFromSurface(canvas, bgs);
	SDL_FreeSurface(bgs);

	// Text descriptions
	char txt[256];
	SDL_Rect hud_bl = {0, 0, 0, 0}, hud_bl2 = {0, 0, 0, 0}, hud_tl = {0, 0, 0, 0};
	SDL_Color hudc = {255,255,0,0};
	SDL_Texture** t_towns = calloc(map->size, sizeof(SDL_Surface*));
	for (id i = 0 ; i < map->size ; ++i)
	{
		snprintf(txt, 255, "%s (%.2f,%.2f)", map->towns[i].name, map->towns[i].x, map->towns[i].y);
		SDL_Surface* sms = TTF_RenderText_Blended(font, txt, hudc);
		t_towns[i] = SDL_CreateTextureFromSurface(canvas, sms);
		SDL_FreeSurface(sms);
	}
	
	SDL_Surface* stt = TTF_RenderText_Blended(font, "Longueur totale : ", hudc);
	SDL_Texture* t_total = SDL_CreateTextureFromSurface(canvas, stt);
	SDL_FreeSurface(stt);
	SDL_QueryTexture(t_total, NULL, NULL, &hud_bl.w, &hud_bl.h);
	hud_bl.y = hud_bl2.y = win_h-hud_bl.h;
	hud_bl2.x = hud_bl.w;
	SDL_Texture* t_len = render_length(canvas, font, NULL, &hud_bl2, 0.0);

	// Current run
	run* r = calloc(1, sizeof(run));
	r->size = 1;
	r->path = calloc(map->size, sizeof(id));
	r->length = 0;

	bool* done = calloc(map->size, sizeof(bool));
	for (id i = 0 ; i < map->size ; ++i)
		done[i] = false;
	done[0] = true;

	// Drawing loop
	bool end = false;
	while (!end)
	{
		// Handle events
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
			{ // Select city (left click)
				id cid = near_city(e.button.x, e.button.y, map);
				if (cid != (unsigned) -1 && !done[cid])
				{
					done[cid] = true;
					r->path[r->size] = cid;
					r->length += map->data[r->path[r->size-1]][r->path[r->size]];
					r->size++;

					if (r->size == map->size)
						r->length += map->data[r->path[r->size-1]][r->path[0]];

					t_len = render_length(canvas, font, t_len, &hud_bl2, r->length);
				}
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT && r->size < map->size && r->size > 1)
			{ // Cancel (right click)
				r->size--;
				r->length -= map->data[r->path[r->size-1]][r->path[r->size]];
				done[r->path[r->size]] = false;
				r->path[r->size] = 0;

				t_len = render_length(canvas, font, t_len, &hud_bl2, r->length);
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_R)
			{ // Reset (r)
				r->size = 1;
				r->length = 0;
				for (id i = 1 ; i < map->size ; ++i)
					done[i] = false;

				t_len = render_length(canvas, font, t_len, &hud_bl2, r->length);
			}
			else if (e.type == SDL_QUIT)
				end = true; // Quit
		}

		// Draw cities
		for (id i = 0 ; i < map->size ; ++i)
		{
			if (done[i])
				SDL_SetRenderDrawColor(canvas, 0, 255, 255, 0);
			else
				SDL_SetRenderDrawColor(canvas, 0, 0, 255, 0);
			SDL_Rect town = {x2w(map->towns[i].x)-8, y2h(map->towns[i].y)-8, 16, 16};
			SDL_RenderFillRect(canvas, &town);
		}

		// Draw current run
		SDL_SetRenderDrawColor(canvas, 255, 0, 255, 0);
		for (unsigned i = 1 ; i < r->size ; ++i)
		{
			city src = map->towns[r->path[i]], dst = map->towns[r->path[i-1]];
			SDL_RenderDrawLine(canvas, x2w(src.x), y2h(src.y), x2w(dst.x), y2h(dst.y));
		}
		if (r->size == map->size)
		{
			city src = map->towns[r->path[0]], dst = map->towns[r->path[r->size-1]];
			SDL_RenderDrawLine(canvas, x2w(src.x), y2h(src.y), x2w(dst.x), y2h(dst.y));
		}

		// Draw best run (if h pressed)
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_H])
		{
			SDL_SetRenderDrawColor(canvas, 0, 255, 0, 0);
			for (unsigned i = 0 ; i < best_run->size ; ++i)
			{
				city src = map->towns[best_run->path[i]], dst = map->towns[best_run->path[(i+1)%best_run->size]];
				SDL_RenderDrawLine(canvas, x2w(src.x), y2h(src.y), x2w(dst.x), y2h(dst.y));
			}
		}
	
		// Draw nearest neighbor run (if n pressed)
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_N])
		{
			SDL_SetRenderDrawColor(canvas, 255, 128, 0, 0);
			for (unsigned i = 0 ; i < nn_run->size ; ++i)
			{
				city src = map->towns[nn_run->path[i]], dst = map->towns[nn_run->path[(i+1)%nn_run->size]];
				SDL_RenderDrawLine(canvas, x2w(src.x), y2h(src.y), x2w(dst.x), y2h(dst.y));
			}
		}

		// Heads-Up Display
		SDL_SetRenderDrawColor(canvas, 0, 0, 0, 0);
		int x, y;
		SDL_GetMouseState(&x, &y);
		id cid = near_city(x, y, map);
		if (cid != (unsigned) -1)
		{
			SDL_RenderFillRect(canvas, &hud_tl);
			SDL_QueryTexture(t_towns[cid], NULL, NULL, &hud_tl.w, &hud_tl.h);
			SDL_RenderCopy(canvas, t_towns[cid], NULL, &hud_tl);
		}
		SDL_RenderFillRect(canvas, &hud_bl);
		SDL_RenderFillRect(canvas, &hud_bl2);
		SDL_RenderCopy(canvas, t_total, NULL, &hud_bl);
		SDL_RenderCopy(canvas, t_len, NULL, &hud_bl2);

		// Refresh screen
		SDL_RenderPresent(canvas);
		//SDL_SetRenderDrawColor(canvas, 0, 0, 0, 0);
		//SDL_RenderClear(canvas);
		SDL_RenderCopy(canvas, bg, NULL, &scr);

		frames++;
	}

	for (id i = 0 ; i < map->size ; ++i)
		SDL_DestroyTexture(t_towns[i]);
	free(t_towns);

	SDL_DestroyTexture(t_total);
	SDL_DestroyTexture(t_len);
	SDL_DestroyTexture(bg);

	TTF_CloseFont(font);

	SDL_DestroyRenderer(canvas);
	SDL_DestroyWindow(mw);
	SDL_Quit();
	TTF_Quit();

	free(inst.name);
	free(inst.picfile);
	for (id i = 0 ; i < map->size ; ++i)
		free(inst.towns[i].name);
	free(inst.towns);

	destroy_map(map);

	destroy_run(best_run);
	destroy_run(nn_run);
	destroy_run(r);

	free(done);

	return EXIT_SUCCESS;
}
