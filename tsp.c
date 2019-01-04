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

#include <stdlib.h>
#include <string.h>

#include "tsp.h"

dist distance (dist x1, dist y1, dist x2, dist y2)
{
	dist dx = x1-x2, dy = y1-y2;
	return sqrt(dx*dx + dy*dy);
}

map* make_map (city* towns, size_t n)
{
	map* m = calloc(1, sizeof(map));
	m->size = n;
	m->towns = towns;
	m->data = calloc(n, sizeof(dist*));

	// Euclidian distances.
	for (size_t i = 0 ; i < n ; ++i) {
		m->data[i] = calloc(n, sizeof(dist));
		for (size_t j = 0 ; j < n ; ++j)
			m->data[i][j] = distance(towns[i].x, towns[i].y, towns[j].x, towns[j].y);
	}
	return m;
}

void destroy_map (map* m)
{
	for (size_t i = 0 ; i < m->size ; ++i)
		free(m->data[i]);
	free(m->data);
	free(m);
}

void destroy_run (run* r)
{
	free(r->path);
	free(r);
}
