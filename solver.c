/*  Quick and dirty implementation of a Traveling Salesman Problem solver.
 *  Copyright (C) 2018-2019 - Jérôme Kirman
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

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "solver.h"

void bruteforce_aux (const map* m, run* r, bool* done);

// Current best run
run* best_run = NULL;

// Recursive brute force approach
void bruteforce_aux (const map* m, run* r, bool* done)
{
	dist d = r->length;

	// Complete runs
	if (r->size == m->size)
	{
		// Close the loop
		d += m->data[r->path[r->size-1]][r->path[0]];

		// Here comes a new challenger
		if (d < best_run->length)
		{
			memcpy(best_run->path, r->path, r->size*sizeof(id));
			best_run->length = d;
		}

		return;
	}

	// Try unvisited cities
	for (id i = 0 ; i < m->size ; ++i)
	{
		if (done[i]) continue; // Visited

		// Push i
		r->path[r->size] = i;
		r->length = d + m->data[r->path[r->size-1]][r->path[r->size]];
		r->size++;
		done[i] = true;

		bruteforce_aux(m, r, done);

		// Pop i
		done[i] = false;
		r->size--;
		r->length = d;
	}
}

run* bruteforce (const map* m)
{
	// Init working run, best run and todo list
	run* r = calloc(1, sizeof(run));
	r->size = 1;
	r->path = calloc(m->size, sizeof(id));
	r->length = 0;

	best_run = calloc(1, sizeof(run));
	best_run->size = m->size;
	best_run->path = calloc(m->size, sizeof(id));
	best_run->length = MAX_DIST;

	bool* done = calloc(m->size, sizeof(bool));
	for (id i = 0 ; i < m->size ; ++i)
		done[i] = false;
	done[0] = true;

	// HCF instruction
	bruteforce_aux(m, r, done);

	free(done);
	free(r->path);
	free(r);

	return best_run;
}

run* nearest_neighbor (const map* m)
{
	run* r = calloc(1, sizeof(run));
	r->size = 1;
	r->path = calloc(m->size, sizeof(id));
	r->length = 0;
	
	bool* done = calloc(m->size, sizeof(bool));
	for (id i = 0 ; i < m->size ; ++i)
		done[i] = false;
	done[0] = true;

	do {
		id nn = (id) -1;
		dist nd = MAX_DIST;
		for (id i = 0 ; i < m->size ; ++i) {
			if (done[i])
				continue;

			dist d = m->data[r->path[r->size-1]][i];

			if (d < nd) {
				nd = d;
				nn = i;
			}
		}

		done[nn] = true;
		r->path[r->size] = nn;
		r->length += nd;
	} while (++r->size < m->size);

	// Close the loop
	r->length += m->data[r->path[r->size-1]][r->path[0]];

	free(done);

	return r;
}
