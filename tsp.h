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

#ifndef TSP_H_
#define TSP_H_

#include <stdbool.h>
#include <math.h>

typedef double dist; // Unit of distance

#define MAX_DIST INFINITY

// City
typedef struct {
	char* name; // Human-readable name
	dist x, y;  // Visual positioning
} city;

// Full map data
typedef unsigned id; // Index of a city in map
typedef struct {
	size_t size; // Number of cities
	city* towns; // List of cities by id
	dist** data; // Distance matrix between cities, by id
} map;

// Full or partial run on a map
typedef struct {
	id* path;    // Visiting order (a list of m->size ids)
	size_t size; // Number of cities currently in run
	dist length; // Total run length
} run;

// Distance between two points on map
dist distance (dist x1, dist y1, dist x2, dist y2);

// Converts a list of city with coordinates into a full map with distances
map* make_map (city* towns, size_t n);

// Free a map
void destroy_map (map* m);

// Free a run
void destroy_run (run* r);

#endif // TSP_H_
