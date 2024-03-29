/*
 *  Dijkstra.c
 *  Brogue
 *
 *  Copyright 2010. All rights reserved.
 *  
 *  This file is part of Brogue.
 *
 *  Brogue is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Brogue is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Brogue.  If not, see <http://www.gnu.org/licenses/>.
 */

// This code was created by Joshua Day, to replace my clunkier and slower Dijkstra scanning algorithm in Movement.c.
// Thanks very much, Joshua.

#include "Rogue.h"
#include "IncludeGlobals.h"

struct pdsLink {
	short distance;
	short cost;
	pdsLink *left, *right;
};

struct pdsMap {
	boolean eightWays;

	pdsLink front;
	pdsLink links[DCOLS * DROWS];
};

void pdsUpdate(pdsMap *map) {
	short dir, dirs;
	pdsLink *left = NULL, *right = NULL, *link = NULL;
	
	dirs = map->eightWays ? 8 : 4;

	pdsLink *head = map->front.right;
	map->front.right = NULL;

	while (head != NULL) {
		for (dir = 0; dir < dirs; dir++) {
			link = head + (nbDirs[dir][0] + DCOLS * nbDirs[dir][1]);

			// verify passability
			if (link->cost < 0) continue;
			if (dir >= 4) {
				pdsLink *way1, *way2;
				way1 = head + nbDirs[dir][0];
				way2 = head + DCOLS * nbDirs[dir][1];
				if (way1->cost == PDS_OBSTRUCTION || way2->cost == PDS_OBSTRUCTION) continue;
			}

			if (head->distance + link->cost < link->distance) {
				link->distance = head->distance + link->cost;

				// reinsert the touched cell; it'll be close to the beginning of the list now, so
				// this will be very fast.  start by removing it.

				if (link->right != NULL) link->right->left = link->left;
				if (link->left != NULL) link->left->right = link->right;
			
				left = head;
				right = head->right;
				while (right != NULL && right->distance < link->distance) {
					left = right;
					right = right->right;
				}
				if (left != NULL) left->right = link;
				link->right = right;
				link->left = left;
				if (right != NULL) right->left = link;
			}
		}

		right = head->right;

		head->left = NULL;
		head->right = NULL;

		head = right;
	}
}

void pdsClear(pdsMap *map, short maxDistance, boolean eightWays) {
	short i;
	
	map->eightWays = eightWays;

	map->front.right = NULL;

	for (i=0; i < DCOLS*DROWS; i++) {
		map->links[i].distance = maxDistance;
		map->links[i].left = map->links[i].right = NULL;
	}
}

short pdsGetDistance(pdsMap *map, short x, short y) {
	pdsUpdate(map);
	return PDS_CELL(map, x, y)->distance;
}

void pdsSetDistance(pdsMap *map, short x, short y, short distance) {
	pdsLink *left, *right, *link;

	if (x > 0 && y > 0 && x < DCOLS - 1 && y < DROWS - 1) {
		link = PDS_CELL(map, x, y);
		if (link->distance > distance) {
			link->distance = distance;

			if (link->right != NULL) link->right->left = link->left;
			if (link->left != NULL) link->left->right = link->right;

			left = &map->front;
			right = map->front.right;
			
			while (right != NULL && right->distance < link->distance) {
				left = right;
				right = right->right;
			}

			link->right = right;
			link->left = left;
			left->right = link;
			if (right != NULL) right->left = link;
		}
	}
}

void pdsSetCosts(pdsMap *map, short **costMap) {
	short i, j;

	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (i != 0 && j != 0 && i < DCOLS - 1 && j < DROWS - 1) {
				PDS_CELL(map, i, j)->cost = costMap[i][j];
			} else {
				PDS_CELL(map, i, j)->cost = PDS_FORBIDDEN;
			}
		}
	}
}

void pdsBatchInput(pdsMap *map, short **distanceMap, char passMap[DCOLS][DROWS], short maxDistance, boolean eightWays) {
	short i, j;
	pdsLink *left, *right;

	map->eightWays = eightWays;

	left = NULL;
	right = NULL;

	map->front.right = NULL;
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			pdsLink *link = PDS_CELL(map, i, j);

			if (distanceMap != NULL) {
				link->distance = distanceMap[i][j];
			} else {
				if (passMap != NULL) {
					// totally hackish; refactor
					link->distance = maxDistance;
				}
			}

			if ((passMap == NULL || passMap[i][j]) && i > 0 && j > 0 && i < DCOLS - 1 && j < DROWS - 1) {
				link->cost = 1;

				if (link->distance < maxDistance) {
					if (right == NULL || right->distance > link->distance) {
						// left and right are used to traverse the list; if many cells have similar values,
						// some time can be saved by not clearing them with each insertion.  this time,
						// sadly, we have to start from the front.

						left = &map->front;
						right = map->front.right;
					}

					while (right != NULL && right->distance < link->distance) {
						left = right;
						right = right->right;
					}

					link->right = right;
					link->left = left;
					left->right = link;
					if (right != NULL) right->left = link;

					left = link;
				} else {
					link->right = NULL;
					link->left = NULL;
				}
			} else {
				if (cellHasTerrainFlag(i, j, T_OBSTRUCTS_PASSABILITY)) link->cost = PDS_OBSTRUCTION;
				else link->cost = PDS_FORBIDDEN;
				link->right = NULL;
				link->left = NULL;
			}
		}
	}
}

void pdsBatchOutput(pdsMap *map, short **distanceMap) {
	short i, j;

	pdsUpdate(map);
	// transfer results to the distanceMap
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			distanceMap[i][j] = PDS_CELL(map, i, j)->distance;
		}
	}
}

void pdsInvalidate(pdsMap *map, short maxDistance) {
	pdsBatchInput(map, NULL, NULL, maxDistance, map->eightWays);
}

void dijkstraScan(short **distanceMap, short **costMap, char passMap[DCOLS][DROWS], boolean useDiagonals) {
	static pdsMap map;

	pdsBatchInput(&map, distanceMap, passMap, 30000, useDiagonals);
	if (costMap) {
		pdsSetCosts(&map, costMap);
	}
	pdsBatchOutput(&map, distanceMap);
}

void calculateDistances(short **distanceMap,
						short destinationX, short destinationY,
						unsigned long blockingTerrainFlags,
						creature *traveler,
						boolean canUseSecretDoors) {
	static pdsMap map;

	short i, j;
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			char cost;
			if (canUseSecretDoors && pmap[i][j].layers[DUNGEON] == SECRET_DOOR) {
				cost = 1;
			} else if (cellHasTerrainFlag(i, j, T_OBSTRUCTS_PASSABILITY)) {
				cost = PDS_OBSTRUCTION;
			} else if ((traveler && traveler == &player && !(pmap[i][j].flags & (DISCOVERED | MAGIC_MAPPED)))
					   || ((traveler && monsterAvoids(traveler, i, j)) || cellHasTerrainFlag(i, j, blockingTerrainFlags))) {
				cost = PDS_FORBIDDEN;
			} else {
				cost = 1;
			}
			
			PDS_CELL(&map, i, j)->cost = cost;
		}
	}
	
	pdsClear(&map, 30000, true);
	pdsSetDistance(&map, destinationX, destinationY, 0);
	pdsBatchOutput(&map, distanceMap);
}

short pathingDistance(short x1, short y1, short x2, short y2, unsigned long blockingTerrainFlags) {
	short retval, **distanceMap = allocDynamicGrid();
	calculateDistances(distanceMap, x2, y2, blockingTerrainFlags, NULL, true);
	retval = distanceMap[x1][y1];
	freeDynamicGrid(distanceMap);
	return retval;
}

