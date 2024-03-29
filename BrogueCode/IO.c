/*
 *  IO.c
 *  Brogue
 *
 *  Created by Brian Walker on 1/10/09.
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

#include <math.h>
#include <time.h>

#include "Rogue.h"
#include "IncludeGlobals.h"

// accuracy depends on how many clock cycles occur per second
#define MILLISECONDS	(clock() * 1000 / CLOCKS_PER_SEC)

#define MILLISECONDS_FOR_CAUTION	100

void considerCautiousMode() {
	/*
	signed long oldMilliseconds = rogue.milliseconds;
	rogue.milliseconds = MILLISECONDS;
	clock_t i = clock();
	printf("\n%li", i);
	if (rogue.milliseconds - oldMilliseconds < MILLISECONDS_FOR_CAUTION) {
		rogue.cautiousMode = true;
	}*/
}

// flags the entire window as needing to be redrawn at next flush.
// very low level -- does not interface with the guts of the game.
void refreshScreen() {
	short i, j;
	
	for( i=0; i<COLS; i++ ) {
		for( j=0; j<ROWS; j++ ) {
			displayBuffer[i][j].needsUpdate = true;
		}
	}
	commitDraws();
}

// higher-level redraw
void displayLevel() {
	
	short i, j;
	
	for( i=0; i<DCOLS; i++ ) {
		for( j=0; j<DROWS; j++ ) {
			refreshDungeonCell(i, j);
		}
	}
}

// converts colors into components
void storeColorComponents(char components[3], const color *theColor) {
	short rand = rand_range(0, theColor->rand);
	components[0] = max(0, min(100, theColor->red + rand_range(0, theColor->redRand) + rand));
	components[1] = max(0, min(100, theColor->green + rand_range(0, theColor->greenRand) + rand));
	components[2] = max(0, min(100, theColor->blue + rand_range(0, theColor->blueRand) + rand));
}

void bakeTerrainColors(color *foreColor, color *backColor, short x, short y) {
	const short *vals = &(terrainRandomValues[x][y][0]);
	const short foreRand = foreColor->rand * vals[6] / 1000;
	const short backRand = backColor->rand * vals[7] / 1000;
	
	foreColor->red += foreColor->redRand * vals[0] / 1000 + foreRand;
	foreColor->green += foreColor->greenRand * vals[1] / 1000 + foreRand;
	foreColor->blue += foreColor->blueRand * vals[2] / 1000 + foreRand;
	foreColor->redRand = foreColor->greenRand = foreColor->blueRand = foreColor->rand = 0;
	
	backColor->red += backColor->redRand * vals[3] / 1000 + backRand;
	backColor->green += backColor->greenRand * vals[4] / 1000 + backRand;
	backColor->blue += backColor->blueRand * vals[5] / 1000 + backRand;
	backColor->redRand = backColor->greenRand = backColor->blueRand = backColor->rand = 0;
	
	if (foreColor->colorDances || backColor->colorDances) {
		pmap[x][y].flags |= TERRAIN_COLORS_DANCING;
	} else {
		pmap[x][y].flags &= ~TERRAIN_COLORS_DANCING;
	}
}

void bakeColor(color *theColor) {
	short rand;
	
	rand = rand_range(0, theColor->rand);
	theColor->red += rand_range(0, theColor->redRand) + rand;
	theColor->green += rand_range(0, theColor->greenRand) + rand;
	theColor->blue += rand_range(0, theColor->blueRand) + rand;
	theColor->redRand = theColor->greenRand = theColor->blueRand = theColor->rand = 0;
}

void shuffleTerrainColors(short percentOfCells, boolean refreshCells) {
	short i, j, k;
	
	assureCosmeticRNG;
	
	for (i=0; i<DCOLS; i++) {
		for(j=0; j<DROWS; j++) {
			if (playerCanSeeOrSense(i, j)
				&& (!rogue.automationActive || !(rogue.turnNumber % 5))
				&& ((pmap[i][j].flags & TERRAIN_COLORS_DANCING)
					|| (player.status.hallucinating && pmap[i][j].flags & VISIBLE))
				&& (percentOfCells >= 100 || rand_range(1, 100) <= percentOfCells)) {
					
					for (k=0; k<8; k++) {
						terrainRandomValues[i][j][k] += rand_range(-600, 600);
						terrainRandomValues[i][j][k] = clamp(terrainRandomValues[i][j][k], 0, 1000);
					}
					
					if (refreshCells) {
						refreshDungeonCell(i, j);
					}
				}
		}
	}
	restoreRNG;
}

#define MIN_COLOR_DIFF			500

// weighted sum of the squares of the component differences. Weights are according to color perception.
#define COLOR_DIFF(f, b)		 (((f).red - (b).red) * ((f).red - (b).red) * 0.2126 \
								+ ((f).green - (b).green) * ((f).green - (b).green) * 0.7152 \
								+ ((f).blue - (b).blue) * ((f).blue - (b).blue) * 0.0722)

// if forecolor is too similar to back, darken or lighten it and return true.
// Assumes colors have already been baked (no random components).
boolean separateColors(color *fore, color *back) {
	color f, b, *modifier;
	short failsafe;
	boolean madeChange;
	
	f = *fore;
	b = *back;
	f.red		= max(0, min(100, f.red));
	f.green		= max(0, min(100, f.green));
	f.blue		= max(0, min(100, f.blue));
	b.red		= max(0, min(100, b.red));
	b.green		= max(0, min(100, b.green));
	b.blue		= max(0, min(100, b.blue));
	
	if (f.red + f.blue + f.green > 50 * 3) {
		modifier = &black;
	} else {
		modifier = &white;
	}
	
	madeChange = false;
	failsafe = 10;
	
	while(COLOR_DIFF(f, b) < MIN_COLOR_DIFF && --failsafe) {
		applyColorAverage(&f, modifier, 20);
		madeChange = true;
	}
	
	if (madeChange) {
		*fore = f;
		return true;
	} else {
		return false;
	}
}

// okay, this is kind of a beast...
void getCellAppearance(short x, short y, uchar *returnChar, color *returnForeColor, color *returnBackColor) {	
	short bestBCPriority, bestFCPriority, bestCharPriority;
	uchar cellChar = 0;
	color cellForeColor, cellBackColor, lightMultiplierColor = black, gasAugmentColor;
	boolean haveForeColor = false, haveBackColor = false;
	boolean monsterWithDetectedItem = false, needDistinctness = false;
	short gasAugmentWeight = 0;
	creature *monst = NULL;
	item *theItem = NULL;
	uchar itemChars[] = {POTION_CHAR, SCROLL_CHAR, FOOD_CHAR, WAND_CHAR,
						STAFF_CHAR, GOLD_CHAR, ARMOR_CHAR, WEAPON_CHAR, RING_CHAR};
	enum dungeonLayers layer, maxLayer;
	
	assureCosmeticRNG;
	
#ifdef BROGUE_ASSERTS
	assert(coordinatesAreInMap(x, y));
#endif
	
	if (pmap[x][y].flags & HAS_MONSTER) {
		monst = monsterAtLoc(x, y);
		monsterWithDetectedItem = (monst->carriedItem && (monst->carriedItem->flags & ITEM_MAGIC_DETECTED)
								   && itemMagicChar(monst->carriedItem) && !canSeeMonster(monst));
	}
	
	if (monsterWithDetectedItem) {
		theItem = monst->carriedItem;
	} else {
		theItem = itemAtLoc(x, y);
	}
	
	if (!playerCanSeeOrSense(x, y)
		&& !(pmap[x][y].flags & (ITEM_DETECTED | HAS_PLAYER))
		&& (!player.status.telepathic || !monst || (monst->info.flags & MONST_INANIMATE))
		&& !monsterWithDetectedItem
		&& (pmap[x][y].flags & (DISCOVERED | MAGIC_MAPPED))
		&& (pmap[x][y].flags & STABLE_MEMORY)) {
		
		// restore memory
		cellChar = pmap[x][y].rememberedAppearance.character;
		cellForeColor = colorFromComponents(pmap[x][y].rememberedAppearance.foreColorComponents);
		cellBackColor = colorFromComponents(pmap[x][y].rememberedAppearance.backColorComponents);
	} else {
		
		bestFCPriority = bestBCPriority = bestCharPriority = 10000;
		
		if ((pmap[x][y].flags & MAGIC_MAPPED) && !(pmap[x][y].flags & DISCOVERED) && !rogue.playbackOmniscience) {
			maxLayer = LIQUID + 1;
		} else {
			maxLayer = NUMBER_TERRAIN_LAYERS;
		}
		
		for (layer = 0; layer < maxLayer; layer++) {
			// gas shows up as a color augment, not directly
			if (pmap[x][y].layers[layer] && layer != GAS) {
				
				if ((!haveForeColor || tileCatalog[pmap[x][y].layers[layer]].drawPriority < bestFCPriority) && (tileCatalog[pmap[x][y].layers[layer]].foreColor)) {
					cellForeColor = *(tileCatalog[pmap[x][y].layers[layer]].foreColor);
					haveForeColor = true;
					bestFCPriority = tileCatalog[pmap[x][y].layers[layer]].drawPriority;
				}
				if ((!haveBackColor || tileCatalog[pmap[x][y].layers[layer]].drawPriority < bestBCPriority) && (tileCatalog[pmap[x][y].layers[layer]].backColor)) {
					cellBackColor = *(tileCatalog[pmap[x][y].layers[layer]].backColor);
					haveBackColor = true;
					bestBCPriority = tileCatalog[pmap[x][y].layers[layer]].drawPriority;
				}
				if ((!cellChar || tileCatalog[pmap[x][y].layers[layer]].drawPriority < bestCharPriority) && (tileCatalog[pmap[x][y].layers[layer]].displayChar)) {
					cellChar = tileCatalog[pmap[x][y].layers[layer]].displayChar;
					bestCharPriority = tileCatalog[pmap[x][y].layers[layer]].drawPriority;
				}
			}
		}
		
		colorMultiplierFromDungeonLight(x, y, &lightMultiplierColor);
		
		if (pmap[x][y].layers[GAS] && tileCatalog[pmap[x][y].layers[GAS]].backColor
			&& playerCanSeeOrSense(x, y)) {
			gasAugmentColor = *(tileCatalog[pmap[x][y].layers[GAS]].backColor);
			gasAugmentWeight = min(85, 35 + pmap[x][y].volume);
		}
		
		if (pmap[x][y].flags & HAS_PLAYER) {
			cellChar = player.info.displayChar;
			cellForeColor = *(player.info.foreColor);
			needDistinctness = true;
		} else if (((pmap[x][y].flags & HAS_ITEM) && (pmap[x][y].flags & ITEM_DETECTED)
					&& itemMagicChar(theItem)
					&& (!playerCanSeeOrSense(x, y) ||
						cellHasTerrainFlag(x, y, T_OBSTRUCTS_ITEMS)))
				   || monsterWithDetectedItem){
			cellChar = itemMagicChar(theItem);
			cellForeColor = white;
			if (cellChar == GOOD_MAGIC_CHAR) {
				cellForeColor = goodCombatMessageColor;
			} else if (cellChar == BAD_MAGIC_CHAR) {
				cellForeColor = badCombatMessageColor;
			}
			cellBackColor = black;
		} else if ((pmap[x][y].flags & HAS_MONSTER)
				   && (playerCanSeeOrSense(x, y) || ((monst->info.flags & MONST_IMMOBILE) && (pmap[x][y].flags & DISCOVERED)))
				   && (!(monst->info.flags & MONST_INVISIBLE) || monst->creatureState == MONSTER_ALLY)
				   && (!(monst->bookkeepingFlags & MONST_SUBMERGED) || rogue.inWater)) {
			needDistinctness = true;
			if (player.status.hallucinating > 0 && !(monst->info.flags & MONST_INANIMATE) && !rogue.playbackOmniscience) {
				cellChar = rand_range('a', 'z') + (rand_range(0, 1) ? 'A' - 'a' : 0);
				cellForeColor = *(monsterCatalog[rand_range(1, NUMBER_MONSTER_KINDS - 1)].foreColor);
			} else {
				cellChar = monst->info.displayChar;
				if (monst->info.flags & MONST_INVISIBLE) { // Invisible allies show up on the screen with a transparency effect.
					cellForeColor = cellBackColor;
				} else {
					cellForeColor = *(monst->info.foreColor);
					if (monst->creatureState == MONSTER_ALLY) {
						applyColorAverage(&cellForeColor, &pink, 50);
					}
				}
				//DEBUG if (monst->bookkeepingFlags & MONST_LEADER) applyColorAverage(&cellBackColor, &purple, 50);
			}
		} else if (player.status.telepathic > 0
				   && pmap[x][y].flags & HAS_MONSTER
				   && !(monst->info.flags & MONST_INANIMATE)
				   && (!playerCanSee(x, y) || (monst->info.flags & MONST_INVISIBLE || monst->bookkeepingFlags & MONST_SUBMERGED))) {
			if (player.status.hallucinating && !rogue.playbackOmniscience) {
				cellChar = (rand_range(0, 1) ? 'X' : 'x');
			} else {
				cellChar = (monst->info.displayChar >= 'a' && monst->info.displayChar <= 'z' ? 'x' : 'X');
			}
			cellForeColor = white;
			lightMultiplierColor = white;
			if (!(pmap[x][y].flags & DISCOVERED)) {
				cellBackColor = black;
				gasAugmentColor = black;
			}
		} else if ((pmap[x][y].flags & HAS_ITEM) && !cellHasTerrainFlag(x, y, T_OBSTRUCTS_ITEMS)
				   && (playerCanSeeOrSense(x, y) || (pmap[x][y].flags & (DISCOVERED) && !cellHasTerrainFlag(x, y, T_MOVES_ITEMS))) ) {
			needDistinctness = true;
			if (player.status.hallucinating && !rogue.playbackOmniscience) {
				cellChar = itemChars[rand_range(0, 8)];
				cellForeColor = yellow;
			} else {
				theItem = itemAtLoc(x, y);
				cellChar = theItem->displayChar;
				cellForeColor = *(theItem->foreColor);
			}
		} else if (playerCanSeeOrSense(x, y) || (pmap[x][y].flags & (DISCOVERED | MAGIC_MAPPED))) {
			// just don't want these to be plotted as black
		} else {
			*returnChar = ' ';
			*returnForeColor = black;
			*returnBackColor = undiscoveredColor;
			restoreRNG;
			return;
		}
		
		if (gasAugmentWeight) {
			applyColorAverage(&cellForeColor, &gasAugmentColor, gasAugmentWeight);
			// phantoms create sillhouettes in gas clouds
			if ((pmap[x][y].flags & HAS_MONSTER)
				&& (monst->info.flags & MONST_INVISIBLE)
				&& (!player.status.telepathic || (monst->info.flags & MONST_INANIMATE))) {
				
				if (player.status.hallucinating && !rogue.playbackOmniscience) {
					cellChar = monsterCatalog[rand_range(1, NUMBER_MONSTER_KINDS - 1)].displayChar;
				} else {
					cellChar = monst->info.displayChar;
				}
				cellForeColor = cellBackColor;
			}
			applyColorAverage(&cellBackColor, &gasAugmentColor, gasAugmentWeight);
		}
		
		if (!(pmap[x][y].flags & (VISIBLE | CLAIRVOYANT_VISIBLE | ITEM_DETECTED | HAS_PLAYER))
			&& (!player.status.telepathic || !monst || (monst->info.flags & MONST_INANIMATE)) && !monsterWithDetectedItem) {
			
			// store memory
			storeColorComponents(pmap[x][y].rememberedAppearance.foreColorComponents, &cellForeColor);
			storeColorComponents(pmap[x][y].rememberedAppearance.backColorComponents, &cellBackColor);
			
			
			lightMultiplierColor.red += minersLightStartColor.red;
			lightMultiplierColor.green += minersLightStartColor.green;
			lightMultiplierColor.blue += minersLightStartColor.blue;
			
			applyColorMultiplier(&cellForeColor, &lightMultiplierColor);
			applyColorMultiplier(&cellBackColor, &lightMultiplierColor);
			
			pmap[x][y].rememberedAppearance.character = cellChar;
			pmap[x][y].flags |= STABLE_MEMORY;
			pmap[x][y].rememberedTerrain = pmap[x][y].layers[highestPriorityLayer(x, y, false)];
			if (pmap[x][y].flags & HAS_ITEM) {
				for (theItem = floorItems->nextItem; (theItem->xLoc != x || theItem->yLoc != y); theItem = theItem->nextItem);
				pmap[x][y].rememberedItemCategory = theItem->category;
			} else {
				pmap[x][y].rememberedItemCategory = 0;
			}
			
			// then restore to eliminate visual artifacts
			cellForeColor = colorFromComponents(pmap[x][y].rememberedAppearance.foreColorComponents);
			cellBackColor = colorFromComponents(pmap[x][y].rememberedAppearance.backColorComponents);
		}
		
		if (rogue.playbackOmniscience
			&& !rogue.playbackFastForward
			&& !playerCanSee(x, y)
			&& playerCanSeeOrSense(x, y)) { // playback omniscience disregards undiscovered granite
			
			applyColorMultiplier(&cellBackColor, &omniscienceColor);
			applyColorMultiplier(&cellForeColor, &omniscienceColor);
		}
	}
	
	if ((pmap[x][y].flags & (ITEM_DETECTED) || monsterWithDetectedItem
		 || (player.status.telepathic > 0 && (pmap[x][y].flags & (HAS_MONSTER))
			 && monst && !(monst->info.flags & MONST_INANIMATE)))
		&& !playerCanSeeOrSense(x, y)) {
		// do nothing
	} else if (!(pmap[x][y].flags & VISIBLE) && (pmap[x][y].flags & CLAIRVOYANT_VISIBLE)) {
		// can clairvoyantly see it
		
		lightMultiplierColor.red += minersLightStartColor.red;
		lightMultiplierColor.green += minersLightStartColor.green;
		lightMultiplierColor.blue += minersLightStartColor.blue;
		
		applyColorMultiplier(&cellForeColor, &lightMultiplierColor);
		applyColorMultiplier(&cellBackColor, &lightMultiplierColor);
		
		applyColorMultiplier(&cellForeColor, &clairvoyanceColor);
		applyColorMultiplier(&cellBackColor, &clairvoyanceColor);
	} else if (!(pmap[x][y].flags & DISCOVERED) && (pmap[x][y].flags & MAGIC_MAPPED)) {
		// magic mapped only
		if (!rogue.playbackOmniscience) {
			needDistinctness = false;
			applyColorMultiplier(&cellForeColor, &magicMapColor);
			applyColorMultiplier(&cellBackColor, &magicMapColor);
		}
	} else if (!(pmap[x][y].flags & VISIBLE) && !rogue.playbackOmniscience) {
		// if it's not visible
		
		needDistinctness = false;
		if (rogue.inWater) {
			applyColorAverage(&cellForeColor, &black, 80);
			applyColorAverage(&cellBackColor, &black, 80);
		} else {
			applyColorMultiplier(&cellForeColor, &memoryColor);
			applyColorMultiplier(&cellBackColor, &memoryColor);
			applyColorAverage(&cellForeColor, &memoryOverlay, 25);
			applyColorAverage(&cellBackColor, &memoryOverlay, 25);
		}
	} else if (playerCanSeeOrSense(x, y) && rogue.playbackOmniscience && !(pmap[x][y].flags & (VISIBLE | CLAIRVOYANT_VISIBLE))) {

		// omniscience
		
		lightMultiplierColor.red += minersLightStartColor.red;
		lightMultiplierColor.green += minersLightStartColor.green;
		lightMultiplierColor.blue += minersLightStartColor.blue;
		
		applyColorMultiplier(&cellForeColor, &lightMultiplierColor);
		applyColorMultiplier(&cellBackColor, &lightMultiplierColor);
		
	} else {
		applyColorMultiplier(&cellForeColor, &lightMultiplierColor);
		applyColorMultiplier(&cellBackColor, &lightMultiplierColor);
		
		if (player.status.hallucinating) {
			randomizeColor(&cellForeColor, 40 * player.status.hallucinating / 300 + 20);
			randomizeColor(&cellBackColor, 40 * player.status.hallucinating / 300 + 20);
		}
		if (rogue.inWater) {
			applyColorMultiplier(&cellForeColor, &deepWaterLightColor);
			applyColorMultiplier(&cellBackColor, &deepWaterLightColor);
		}
	}
	// DEBUG cellBackColor.red = max(0,((scentMap[x][y] - rogue.scentTurnNumber) * 2) + 100);
	// DEBUG if (pmap[x][y].flags & PLAYER_STEPPED_HERE) cellBackColor.red += 20;
	
	bakeTerrainColors(&cellForeColor, &cellBackColor, x, y);
	
	if (needDistinctness) {
		separateColors(&cellForeColor, &cellBackColor);
	}
	
	*returnChar = cellChar;
	*returnForeColor = cellForeColor;
	*returnBackColor = cellBackColor;
	restoreRNG;
}

void refreshDungeonCell(short x, short y) {
	uchar cellChar;
#ifdef BROGUE_ASSERTS
	assert(coordinatesAreInMap(x, y));
#endif
	color foreColor, backColor;
	getCellAppearance(x, y, &cellChar, &foreColor, &backColor);
	plotCharWithColor(cellChar, mapToWindowX(x), mapToWindowY(y), foreColor, backColor);
}

void applyColorMultiplier(color *baseColor, color *multiplierColor) {
	baseColor->red = baseColor->red * multiplierColor->red / 100;
	baseColor->redRand = baseColor->redRand * multiplierColor->redRand / 100;
	baseColor->green = baseColor->green * multiplierColor->green / 100;
	baseColor->greenRand = baseColor->greenRand * multiplierColor->greenRand / 100;
	baseColor->blue = baseColor->blue * multiplierColor->blue / 100;
	baseColor->blueRand = baseColor->blueRand * multiplierColor->blueRand / 100;
	baseColor->rand = baseColor->rand * multiplierColor->rand / 100;
	//baseColor->colorDances *= multiplierColor->colorDances;
	return;
}

void applyColorAverage(color *baseColor, color *newColor, short averageWeight) {
	short weightComplement = 100 - averageWeight;
	baseColor->red = (baseColor->red * weightComplement + newColor->red * averageWeight) / 100;
	baseColor->redRand = (baseColor->redRand * weightComplement + newColor->redRand * averageWeight) / 100;
	baseColor->green = (baseColor->green * weightComplement + newColor->green * averageWeight) / 100;
	baseColor->greenRand = (baseColor->greenRand * weightComplement + newColor->greenRand * averageWeight) / 100;
	baseColor->blue = (baseColor->blue * weightComplement + newColor->blue * averageWeight) / 100;
	baseColor->blueRand = (baseColor->blueRand * weightComplement + newColor->blueRand * averageWeight) / 100;
	baseColor->rand = (baseColor->rand * weightComplement + newColor->rand * averageWeight) / 100;
	baseColor->colorDances = (baseColor->colorDances || newColor->colorDances);
	return;
}

void applyColorAugment(color *baseColor, const color *augmentingColor, short augmentWeight) {
	baseColor->red += (augmentingColor->red * augmentWeight) / 100;
	baseColor->redRand += (augmentingColor->redRand * augmentWeight) / 100;
	baseColor->green += (augmentingColor->green * augmentWeight) / 100;
	baseColor->greenRand += (augmentingColor->greenRand * augmentWeight) / 100;
	baseColor->blue += (augmentingColor->blue * augmentWeight) / 100;
	baseColor->blueRand += (augmentingColor->blueRand * augmentWeight) / 100;
	baseColor->rand += (augmentingColor->rand * augmentWeight) / 100;
	return;
}

void desaturate(color *baseColor, short weight) {
	short avg;
	avg = (baseColor->red + baseColor->green + baseColor->blue) / 3 + 1;
	baseColor->red = baseColor->red * (100 - weight) / 100 + (avg * weight / 100);
	baseColor->green = baseColor->green * (100 - weight) / 100 + (avg * weight / 100);
	baseColor->blue = baseColor->blue * (100 - weight) / 100 + (avg * weight / 100);
	
	avg = (baseColor->redRand + baseColor->greenRand + baseColor->blueRand) / 3 + 1;
	baseColor->redRand = baseColor->redRand * (100 - weight) / 100;
	baseColor->greenRand = baseColor->greenRand * (100 - weight) / 100;
	baseColor->blueRand = baseColor->blueRand * (100 - weight) / 100;
	
	baseColor->rand += 3 * avg * weight / 100;
}

short randomizeByPercent(short input, short percent) {
	return (rand_range(input * (100 - percent) / 100, input * (100 + percent) / 100));
}

void randomizeColor(color *baseColor, short randomizePercent) {
	baseColor->red = randomizeByPercent(baseColor->red, randomizePercent);
	baseColor->green = randomizeByPercent(baseColor->green, randomizePercent);
	baseColor->blue = randomizeByPercent(baseColor->blue, randomizePercent);
}

// takes dungeon coordinates
void colorBlendCell(short x, short y, color *hiliteColor, short hiliteStrength) {
	uchar displayChar;
	color foreColor, backColor;
	
	getCellAppearance(x, y, &displayChar, &foreColor, &backColor);
	applyColorAverage(&foreColor, hiliteColor, hiliteStrength);
	applyColorAverage(&backColor, hiliteColor, hiliteStrength);
	plotCharWithColor(displayChar, mapToWindowX(x), mapToWindowY(y), foreColor, backColor);
}

// takes dungeon coordinates
void hiliteCell(short x, short y, const color *hiliteColor, short hiliteStrength, boolean distinctColors) {
	uchar displayChar;
	color foreColor, backColor;
	
	assureCosmeticRNG;
	
	getCellAppearance(x, y, &displayChar, &foreColor, &backColor);
	applyColorAugment(&foreColor, hiliteColor, hiliteStrength);
	applyColorAugment(&backColor, hiliteColor, hiliteStrength);
	if (distinctColors) {
		separateColors(&foreColor, &backColor);
	}
	plotCharWithColor(displayChar, mapToWindowX(x), mapToWindowY(y), foreColor, backColor);
	
	restoreRNG;
}

void colorMultiplierFromDungeonLight(short x, short y, color *editColor) {
	editColor->red = max(0, editColor->redRand = tmap[x][y].light[0]);
	editColor->green = max(0, editColor->greenRand = tmap[x][y].light[1]);
	editColor->blue = max(0, editColor->blueRand = tmap[x][y].light[2]);
	editColor->rand = max(0, tmap[x][y].light[0] + tmap[x][y].light[1] + tmap[x][y].light[2]) / 3;
	editColor->colorDances = false;
}

void plotCharWithColor(uchar inputChar, short xLoc, short yLoc, color cellForeColor, color cellBackColor) {
	
	short foreRed = cellForeColor.red,
	foreGreen = cellForeColor.green,
	foreBlue = cellForeColor.blue,
	
	backRed = cellBackColor.red,
	backGreen = cellBackColor.green,
	backBlue = cellBackColor.blue,
	
	foreRand, backRand;
	
#ifdef BROGUE_ASSERTS
	assert(coordinatesAreInWindow(xLoc, yLoc));
#endif
	
	if (rogue.gameHasEnded || rogue.playbackFastForward) {
		return;
	}
	
	assureCosmeticRNG;
	
	foreRand = rand_range(0, cellForeColor.rand);
	backRand = rand_range(0, cellBackColor.rand);
	foreRed += rand_range(0, cellForeColor.redRand) + foreRand;
	foreGreen += rand_range(0, cellForeColor.greenRand) + foreRand;
	foreBlue += rand_range(0, cellForeColor.blueRand) + foreRand;
	backRed += rand_range(0, cellBackColor.redRand) + backRand;
	backGreen += rand_range(0, cellBackColor.greenRand) + backRand;
	backBlue += rand_range(0, cellBackColor.blueRand) + backRand;
	
	foreRed =		min(100, max(0, foreRed));
	foreGreen =		min(100, max(0, foreGreen));
	foreBlue =		min(100, max(0, foreBlue));
	backRed =		min(100, max(0, backRed));
	backGreen =		min(100, max(0, backGreen));
	backBlue =		min(100, max(0, backBlue));
	
	if (inputChar		!= displayBuffer[xLoc][yLoc].character
		|| foreRed		!= displayBuffer[xLoc][yLoc].foreColorComponents[0]
		|| foreGreen	!= displayBuffer[xLoc][yLoc].foreColorComponents[1]
		|| foreBlue		!= displayBuffer[xLoc][yLoc].foreColorComponents[2]
		|| backRed		!= displayBuffer[xLoc][yLoc].backColorComponents[0]
		|| backGreen	!= displayBuffer[xLoc][yLoc].backColorComponents[1]
		|| backBlue		!= displayBuffer[xLoc][yLoc].backColorComponents[2]) {
		
		displayBuffer[xLoc][yLoc].needsUpdate = true;
		
		displayBuffer[xLoc][yLoc].character = inputChar;
		displayBuffer[xLoc][yLoc].foreColorComponents[0] = foreRed;
		displayBuffer[xLoc][yLoc].foreColorComponents[1] = foreGreen;
		displayBuffer[xLoc][yLoc].foreColorComponents[2] = foreBlue;
		displayBuffer[xLoc][yLoc].backColorComponents[0] = backRed;
		displayBuffer[xLoc][yLoc].backColorComponents[1] = backGreen;
		displayBuffer[xLoc][yLoc].backColorComponents[2] = backBlue;
	}
	
	restoreRNG;
}

void plotCharToBuffer(uchar inputChar, short x, short y, color *foreColor, color *backColor, cellDisplayBuffer dbuf[COLS][ROWS]) {
	if (!dbuf) {
		plotCharWithColor(inputChar, x, y, *foreColor, *backColor);
		return;
	}
	
	assureCosmeticRNG;
	dbuf[x][y].foreColorComponents[0] = foreColor->red + rand_range(0, foreColor->redRand) + rand_range(0, foreColor->rand);
	dbuf[x][y].foreColorComponents[1] = foreColor->green + rand_range(0, foreColor->greenRand) + rand_range(0, foreColor->rand);
	dbuf[x][y].foreColorComponents[2] = foreColor->blue + rand_range(0, foreColor->blueRand) + rand_range(0, foreColor->rand);
	dbuf[x][y].backColorComponents[0] = backColor->red + rand_range(0, backColor->redRand) + rand_range(0, backColor->rand);
	dbuf[x][y].backColorComponents[1] = backColor->green + rand_range(0, backColor->greenRand) + rand_range(0, backColor->rand);
	dbuf[x][y].backColorComponents[2] = backColor->blue + rand_range(0, backColor->blueRand) + rand_range(0, backColor->rand);
	dbuf[x][y].character = inputChar;
	dbuf[x][y].opacity = 100;
	restoreRNG;
}

// Set to false and draws don't take effect, they simply queue up. Set to true and all of the
// queued up draws take effect.
void commitDraws() {
	short i, j;
	
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			if (displayBuffer[i][j].needsUpdate) {
				plotChar(displayBuffer[i][j].character, i, j,
						 displayBuffer[i][j].foreColorComponents[0],
						 displayBuffer[i][j].foreColorComponents[1],
						 displayBuffer[i][j].foreColorComponents[2],
						 displayBuffer[i][j].backColorComponents[0],
						 displayBuffer[i][j].backColorComponents[1],
						 displayBuffer[i][j].backColorComponents[2]);
				displayBuffer[i][j].needsUpdate = false;
			}
		}
	}
}

// Debug feature: display the level to the screen without regard to lighting, field of view, etc.
// Highlight the "grid" portion with the color at the strength -- all arguments are optional.
void dumpLevelToScreen() {
	short i, j;
	pcell backup;
	
	assureCosmeticRNG;
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (pmap[i][j].layers[DUNGEON] != GRANITE) {
				backup = pmap[i][j];
				pmap[i][j].flags |= VISIBLE;
				tmap[i][j].light[0] = 100;
				tmap[i][j].light[1] = 100;
				tmap[i][j].light[2] = 100;
				refreshDungeonCell(i, j);
				pmap[i][j] = backup;
			} else {
				plotCharWithColor(' ', mapToWindowX(i), mapToWindowY(j), white, black);
			}

		}
	}
	restoreRNG;
}

void hiliteGrid(char hiliteGrid[DCOLS][DROWS], color *hiliteColor, short hiliteStrength) {
	short i, j, x, y;
	color hCol;
	
	assureCosmeticRNG;
	
	if (hiliteColor) {
		hCol = *hiliteColor;
	} else {
		hCol = yellow;
	}
	
	bakeColor(&hCol);
	
	if (!hiliteStrength) {
		hiliteStrength = 75;
	}
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (hiliteGrid[i][j]) {
				x = mapToWindowX(i);
				y = mapToWindowY(j);
				
				displayBuffer[x][y].needsUpdate = true;
				displayBuffer[x][y].backColorComponents[0] = clamp(displayBuffer[x][y].backColorComponents[0] + hCol.red * hiliteStrength / 100, 0, 100);
				displayBuffer[x][y].backColorComponents[1] = clamp(displayBuffer[x][y].backColorComponents[1] + hCol.green * hiliteStrength / 100, 0, 100);
				displayBuffer[x][y].backColorComponents[2] = clamp(displayBuffer[x][y].backColorComponents[2] + hCol.blue * hiliteStrength / 100, 0, 100);
				displayBuffer[x][y].foreColorComponents[0] = clamp(displayBuffer[x][y].foreColorComponents[0] + hCol.red * hiliteStrength / 100, 0, 100);
				displayBuffer[x][y].foreColorComponents[1] = clamp(displayBuffer[x][y].foreColorComponents[1] + hCol.green * hiliteStrength / 100, 0, 100);
				displayBuffer[x][y].foreColorComponents[2] = clamp(displayBuffer[x][y].foreColorComponents[2] + hCol.blue * hiliteStrength / 100, 0, 100);
			}
		}
	}
	restoreRNG;
}

void blackOutScreen() {
	short i, j;
	
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			plotCharWithColor(' ', i, j, black, black);
		}
	}
}

void copyDisplayBuffer(cellDisplayBuffer toBuf[COLS][ROWS], cellDisplayBuffer fromBuf[COLS][ROWS]) {
	short i, j;
	
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			toBuf[i][j] = fromBuf[i][j];
		}
	}
}

void clearDisplayBuffer(cellDisplayBuffer dbuf[COLS][ROWS]) {
	short i, j, k;
	
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			dbuf[i][j].character = ' ';
			for (k=0; k<3; k++) {
				dbuf[i][j].foreColorComponents[k] = 0;
				dbuf[i][j].backColorComponents[k] = 0;
			}
			dbuf[i][j].opacity = 0;
		}
	}
}

color colorFromComponents(char rgb[3]) {
	color theColor = black;
	theColor.red	= rgb[0];
	theColor.green	= rgb[1];
	theColor.blue	= rgb[2];
	return theColor;
}

// draws overBuf over the current display with per-cell pseudotransparency as specified in overBuf.
// If previousBuf is not null, it gets filled with the preexisting display for reversion purposes.
void overlayDisplayBuffer(cellDisplayBuffer overBuf[COLS][ROWS], cellDisplayBuffer previousBuf[COLS][ROWS]) {
	short i, j;
	color foreColor, backColor, tempColor;
	uchar character;
	
	if (previousBuf) {
		copyDisplayBuffer(previousBuf, displayBuffer);
	}
	
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			
			backColor = colorFromComponents(overBuf[i][j].backColorComponents);
			
			// character and fore color:
			if (overBuf[i][j].character == ' ') { // Blank cells in the overbuf take the character from the screen.
				character = displayBuffer[i][j].character;
				foreColor = colorFromComponents(displayBuffer[i][j].foreColorComponents);
				applyColorAverage(&foreColor, &backColor, overBuf[i][j].opacity);
			} else {
				character = overBuf[i][j].character;
				foreColor = colorFromComponents(overBuf[i][j].foreColorComponents);
			}
			
			// back color:
			tempColor = colorFromComponents(displayBuffer[i][j].backColorComponents);
			applyColorAverage(&backColor, &tempColor, 100 - overBuf[i][j].opacity);
			
			plotCharWithColor(character, i, j, foreColor, backColor);
		}
	}
}

// takes a list of locations, a color and a list of strengths and flashes the foregrounds of those locations.
// Strengths are percentages measuring how hard the color flashes at its peak.
void flashForeground(short *x, short *y, color **flashColor, short *flashStrength, short count, short frames) {
	short i, j, percent;
	uchar *displayChar;
	color *bColor, *fColor, newColor;
	
	if (count <= 0) {
		return;
	}
	
	assureCosmeticRNG;
	
	displayChar = (uchar *) malloc(count * sizeof(uchar));
	fColor = (color *) malloc(count * sizeof(color));
	bColor = (color *) malloc(count * sizeof(color));
	
	for (i=0; i<count; i++) {
		getCellAppearance(x[i], y[i], &displayChar[i], &fColor[i], &bColor[i]);
		bakeColor(&fColor[i]);
		bakeColor(&bColor[i]);
	}
	
	for (j=frames; j>= 0; j--) {
		for (i=0; i<count; i++) {
			percent = flashStrength[i] * j / frames;
			newColor = fColor[i];
			applyColorAverage(&newColor, flashColor[i], percent);
			plotCharWithColor(displayChar[i], mapToWindowX(x[i]), mapToWindowY(y[i]), newColor, bColor[i]);
		}
		if (j) {
			if (pauseBrogue(1)) {
				j = 1;
			}
		}
	}
	
	free(displayChar);
	free(fColor);
	free(bColor);
	
	restoreRNG;
}

void flash(color *theColor, short frames, short x, short y) {
	short i;
	boolean interrupted = false;
	
	for (i=0; i<frames && !interrupted; i++) {
		colorBlendCell(x, y, theColor, 100 - 100 * i / frames);
		interrupted = pauseBrogue(50);
	}
	
	refreshDungeonCell(x, y);
}

// special effect expanding flash of light at dungeon coordinates (x, y) restricted to tiles with matching flags
void lightFlash(const color *theColor, unsigned long reqTerrainFlags,
				unsigned long reqTileFlags, short frames, short maxRadius, short x, short y) {
	short i, j, k, intensity, currentRadius, fadeOut;
	short localRadius[DCOLS][DROWS];
	boolean tileQualifies[DCOLS][DROWS], aTileQualified, fastForward;
	
	aTileQualified = false;
	fastForward = false;
	
	for (i = max(x - maxRadius, 0); i < min(x + maxRadius, DCOLS); i++) {
		for (j = max(y - maxRadius, 0); j < min(y + maxRadius, DROWS); j++) {
			if ((!reqTerrainFlags || cellHasTerrainFlag(reqTerrainFlags, i, j))
				&& (!reqTileFlags || (pmap[i][j].flags & reqTileFlags))
				&& (i-x) * (i-x) + (j-y) * (j-y) <= maxRadius * maxRadius) {
				
				tileQualifies[i][j] = true;
				localRadius[i][j] = sqrt((i-x) * (i-x) + (j-y) * (j-y));
				aTileQualified = true;
			} else {
				tileQualifies[i][j] = false;
			}
		}
	}
	
	if (!aTileQualified) {
		return;
	}
	
	for (k = 1; k <= frames; k++) {
		currentRadius = max(1, maxRadius * k / frames);
		fadeOut = min(100, (frames - k) * 100 * 5 / frames);
		for (i = max(x - maxRadius, 0); i < min(x + maxRadius, DCOLS); i++) {
			for (j = max(y - maxRadius, 0); j < min(y + maxRadius, DROWS); j++) {
				if (tileQualifies[i][j] && (localRadius[i][j] <= currentRadius)) {
					
					intensity = 100 - 100 * (currentRadius - localRadius[i][j] - 2) / currentRadius;
					intensity = fadeOut * intensity / 100;
					
					hiliteCell(i, j, theColor, intensity, false);
				}
			}
		}
		if (!fastForward && (rogue.playbackFastForward || pauseBrogue(50))) {
			k = frames - 1;
			fastForward = true;
		}
	}
}

#define bCurve(x)	(((x) * (x) + 11) / (10 * ((x) * (x) + 1)) - 0.1)

// x and y are global coordinates, not within the playing square
void funkyFade(cellDisplayBuffer displayBuf[COLS][ROWS], color *colorStart,
			   color *colorEnd, short stepCount, short x, short y, boolean invert) {
	short i, j, n, weight;
	double x2, y2, weightGrid[COLS][ROWS][3], percentComplete;
	color tempColor, colorMid, foreColor, backColor;
	uchar tempChar;
	short **distanceMap;
	boolean fastForward;
	
#ifdef BROGUE_LIBTCOD
	stepCount *= 15; // libtcod displays much faster
#endif
	
	fastForward = false;
	distanceMap = allocDynamicGrid();
	fillDynamicGrid(distanceMap, 0);
	calculateDistances(distanceMap, player.xLoc, player.yLoc, T_OBSTRUCTS_PASSABILITY, 0, true);
	
	for (i=0; i<COLS; i++) {
		x2 = (double) ((i - x) * 5.0 / COLS);
		for (j=0; j<ROWS; j++) {
			y2 = (double) ((j - y) * 2.5 / ROWS);
			
			weightGrid[i][j][0] = bCurve(x2*x2+y2*y2) * (.7 + .3 * cos(5*x2*x2) * cos(5*y2*y2));
			weightGrid[i][j][1] = bCurve(x2*x2+y2*y2) * (.7 + .3 * sin(5*x2*x2) * cos(5*y2*y2));
			weightGrid[i][j][2] = bCurve(x2*x2+y2*y2);
			
		}	
	}
	
	for (n=(invert ? stepCount - 1 : 0); (invert ? n >= 0 : n <= stepCount); n += (invert ? -1 : 1)) {
		for (i=0; i<COLS; i++) {
			for (j=0; j<ROWS; j++) {
				
				percentComplete = (double) (n) * 100 / stepCount;
				
				colorMid = *colorStart;
				if (colorEnd) {
					applyColorAverage(&colorMid, colorEnd, n * 100 / stepCount);
				}
				
				// the fade color floods the reachable dungeon tiles faster
				if (!invert && coordinatesAreInMap(windowToMapX(i), windowToMapY(j))
					&& distanceMap[windowToMapX(i)][windowToMapY(j)] >= 0 && distanceMap[windowToMapX(i)][windowToMapY(j)] < 30000) {
					percentComplete *= 1.0 + (100.0 - min(100, distanceMap[windowToMapX(i)][windowToMapY(j)])) / 100.;
				}
				
				weight = (short) percentComplete + weightGrid[i][j][2] * percentComplete * 10;
				weight = min(100, weight);
				tempColor = black;
				
				tempColor.red = ((short) percentComplete + weightGrid[i][j][0] * percentComplete * 10) * colorMid.red / 100;
				tempColor.red = min(colorMid.red, tempColor.red);
				
				tempColor.green = ((short) percentComplete + weightGrid[i][j][1] * percentComplete * 10) * colorMid.green / 100;
				tempColor.green = min(colorMid.green, tempColor.green);
				
				tempColor.blue = ((short) percentComplete + weightGrid[i][j][2] * percentComplete * 10) * colorMid.blue / 100;
				tempColor.blue = min(colorMid.blue, tempColor.blue);
				
				backColor = black;
				
				backColor.red = displayBuf[i][j].backColorComponents[0];
				backColor.green = displayBuf[i][j].backColorComponents[1];
				backColor.blue = displayBuf[i][j].backColorComponents[2];
				
				foreColor = (invert ? white : black);
				
				if (j < MESSAGE_LINES
					&& i >= mapToWindowX(0)
					&& i < mapToWindowX(strLenWithoutEscapes(displayedMessage[MESSAGE_LINES - j - 1]))) {
					tempChar = displayedMessage[MESSAGE_LINES - j - 1][windowToMapX(i)];
				} else {
					tempChar = displayBuf[i][j].character;
					
					foreColor.red = displayBuf[i][j].foreColorComponents[0];
					foreColor.green = displayBuf[i][j].foreColorComponents[1];
					foreColor.blue = displayBuf[i][j].foreColorComponents[2];
					
					applyColorAverage(&foreColor, &tempColor, weight);
				}
				applyColorAverage(&backColor, &tempColor, weight);
				plotCharWithColor(tempChar, i, j, foreColor, backColor);
			}
		}
		if (!fastForward && pauseBrogue(1)) {
			fastForward = true;
			n = (invert ? 1 : stepCount - 2);
		}
	}
	
	freeDynamicGrid(distanceMap);
}

void displayMachines() {
	short i, j;
	color foreColor, backColor, machineColors[50];
	uchar dchar;
	
	assureCosmeticRNG;
	
	for (i=0; i<50; i++) {
		machineColors[i] = black;
		machineColors[i].red = rand_range(0, 100);
		machineColors[i].green = rand_range(0, 100);
		machineColors[i].blue = rand_range(0, 100);
	}
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (pmap[i][j].machineNumber) {
				getCellAppearance(i, j, &dchar, &foreColor, &backColor);
				applyColorAugment(&backColor, &(machineColors[pmap[i][j].machineNumber]), 50);
				plotCharWithColor(dchar, mapToWindowX(i), mapToWindowY(j), foreColor, backColor);
			}
		}
	}
	displayMoreSign();
	displayLevel();
	
	restoreRNG;
}

#define CHOKEMAP_DISPLAY_CUTOFF	160
void displayChokeMap() {
	short i, j;
	color foreColor, backColor;
	uchar dchar;
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (chokeMap[i][j] < CHOKEMAP_DISPLAY_CUTOFF) {
				if (pmap[i][j].flags & IS_GATE_SITE) {
					getCellAppearance(i, j, &dchar, &foreColor, &backColor);
					applyColorAugment(&backColor, &teal, 50);
					plotCharWithColor(dchar, mapToWindowX(i), mapToWindowY(j), foreColor, backColor);
				} else
					if (chokeMap[i][j] < CHOKEMAP_DISPLAY_CUTOFF) {
					getCellAppearance(i, j, &dchar, &foreColor, &backColor);
					applyColorAugment(&backColor, &red, 100 - chokeMap[i][j] * 100 / CHOKEMAP_DISPLAY_CUTOFF);
					plotCharWithColor(dchar, mapToWindowX(i), mapToWindowY(j), foreColor, backColor);
				}
			}
		}
	}
	displayMoreSign();
	displayLevel();
}

void displayLoops() {
	short i, j;
	color foreColor, backColor;
	uchar dchar;
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (pmap[i][j].flags & IN_LOOP) {
				getCellAppearance(i, j, &dchar, &foreColor, &backColor);
				applyColorAugment(&backColor, &yellow, 50);
				plotCharWithColor(dchar, mapToWindowX(i), mapToWindowY(j), foreColor, backColor);
				//colorBlendCell(i, j, &tempColor, 100);//hiliteCell(i, j, &tempColor, 100, true);
			}
			if (pmap[i][j].flags & IS_CHOKEPOINT) {
				getCellAppearance(i, j, &dchar, &foreColor, &backColor);
				applyColorAugment(&backColor, &teal, 50);
				plotCharWithColor(dchar, mapToWindowX(i), mapToWindowY(j), foreColor, backColor);
			}
		}
	}
	waitForAcknowledgment();
}

boolean pauseBrogue(short milliseconds) {
	boolean interrupted;
	
	commitDraws();
	if (rogue.playbackMode && rogue.playbackFastForward) {
		interrupted = pauseForMilliseconds(1);
	} else {
		interrupted = pauseForMilliseconds(milliseconds);
	}
	pausingTimerStartsNow();
	return interrupted;
}

void nextBrogueEvent(rogueEvent *returnEvent, boolean colorsDance, boolean realInputEvenInPlayback) {
	rogueEvent recordingInput;
	boolean repeatAgain;
	short pauseDuration;
	
	returnEvent->eventType = EVENT_ERROR;
	
	if (rogue.playbackMode && (!realInputEvenInPlayback)) {
		do {
			repeatAgain = false;
			if (!rogue.playbackFastForward && rogue.playbackBetweenTurns || rogue.playbackOOS) {
				pauseDuration = (rogue.playbackPaused ? DEFAULT_PLAYBACK_DELAY : rogue.playbackDelayThisTurn);
				if (pauseDuration && pauseBrogue(pauseDuration)) {
					// if the player did something during playback
					nextBrogueEvent(&recordingInput, false, true);
					executePlaybackInput(&recordingInput);
					repeatAgain = true;
				}
			}
		} while ((rogue.playbackPaused || repeatAgain || rogue.playbackOOS) && !rogue.gameHasEnded);
		rogue.playbackDelayThisTurn = rogue.playbackDelayPerTurn;
		recallEvent(returnEvent);
	} else {
		commitDraws();
		if (rogue.creaturesWillFlashThisTurn) {
			displayMonsterFlashes(true);
		}
		nextKeyOrMouseEvent(returnEvent, colorsDance);
		// recording done elsewhere
	}
	
	pausingTimerStartsNow();
	
	if (returnEvent->eventType == EVENT_ERROR) {
		rogue.playbackPaused = rogue.playbackMode; // pause if replaying
		message("Event error!", true, true);
	}
}

void executeMouseClick(rogueEvent *theEvent) {
	short x, y;
	boolean autoConfirm;
	x = theEvent->param1;
	y = theEvent->param2;
	autoConfirm = theEvent->controlKey;
	if (coordinatesAreInMap(windowToMapX(x), windowToMapY(y))) {
		travel(windowToMapX(x), windowToMapY(y), autoConfirm);
	}
}

void crystalize() {
	extern color forceFieldColor;
	short i, j;
	for (i=0; i<DCOLS; i++) {
		for (j=0; j < DROWS; j++) {
			if ((player.xLoc - i) * (player.xLoc - i) + (player.yLoc - j) * (player.yLoc - j) <= 20 * 20) {
				if (i == 0 || i == DCOLS - 1 || j == 0 || j == DROWS - 1) {
					pmap[i][j].layers[DUNGEON] = CRYSTAL_WALL;
				} else if (!(pmap[i][j].flags & IMPREGNABLE)
						   && (tileCatalog[pmap[i][j].layers[DUNGEON]].flags & (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_VISION))) {
					pmap[i][j].layers[DUNGEON] = FORCEFIELD;
				}
				//pmap[i][j].flags |= IN_FIELD_OF_VIEW | VISIBLE | DISCOVERED;
			}
		}
	}
	updateVision(false);
	lightFlash(&forceFieldColor, 0, 0, 10, 20, player.xLoc, player.yLoc);
	displayLevel();
	refreshSideBar(NULL);
}

void executeKeystroke(unsigned short keystroke, boolean controlKey, boolean shiftKey) {
	short direction = -1;
	
	confirmMessages();
	stripShiftFromMovementKeystroke(&keystroke);
	
	switch (keystroke) {
		case UP_KEY:
		case UP_ARROW:
		case NUMPAD_8:
			direction = UP;
			break;
		case DOWN_KEY:
		case DOWN_ARROW:
		case NUMPAD_2:
			direction = DOWN;
			break;
		case LEFT_KEY:
		case LEFT_ARROW:
		case NUMPAD_4:
			direction = LEFT;
			break;
		case RIGHT_KEY:
		case RIGHT_ARROW:
		case NUMPAD_6:
			direction = RIGHT;
			break;
		case NUMPAD_7:
			if (shiftKey) {
				recordKeystroke(EASY_MODE_KEY, false, false);
				enableEasyMode();
				break;
			}
			// otherwise continue to movement
		case UPLEFT_KEY:
			direction = UPLEFT;
			break;
		case UPRIGHT_KEY:
		case NUMPAD_9:
			direction = UPRIGHT;
			break;
		case DOWNLEFT_KEY:
		case NUMPAD_1:
			direction = DOWNLEFT;
			break;
		case DOWNRIGHT_KEY:
		case NUMPAD_3:
			direction = DOWNRIGHT;
			break;
		case DESCEND_KEY:
			considerCautiousMode();
			useStairs(1);
			refreshSideBar(NULL);
			break;
		case ASCEND_KEY:
			considerCautiousMode();
			useStairs(-1);
			refreshSideBar(NULL);
			break;
		case REST_KEY:
		case PERIOD_KEY:
		case NUMPAD_5:
			considerCautiousMode();
			rogue.justRested = true;
			recordKeystroke(REST_KEY, false, false);
			playerTurnEnded();
			refreshSideBar(NULL);
			break;
		case AUTO_REST_KEY:
			rogue.justRested = true;
			autoRest();
			break;
		case SEARCH_KEY:
			recordKeystroke(SEARCH_KEY, false, false);
			considerCautiousMode();
			search(rogue.awarenessBonus < 0 ? 20 : 50);
			playerTurnEnded();
			refreshSideBar(NULL);
			break;
		case INVENTORY_KEY:
			displayInventory(ALL_ITEMS, 0, 0, true);
			break;
		case EQUIP_KEY:
			equip();
			refreshSideBar(NULL);
			break;
		case UNEQUIP_KEY:
			unequip();
			refreshSideBar(NULL);
			break;
		case DROP_KEY:
			drop();
			refreshSideBar(NULL);
			break;
		case APPLY_KEY:
			apply(NULL);
			refreshSideBar(NULL);
			break;
		case THROW_KEY:
			throwCommand();
			refreshSideBar(NULL);
			break;
		case FIGHT_KEY:
			autoFight(false);
			refreshSideBar(NULL);
			break;
		case FIGHT_TO_DEATH_KEY:
			autoFight(true);
			refreshSideBar(NULL);
			break;
		case CALL_KEY:
			call();
			break;
		case REPEAT_TRAVEL_KEY:
			considerCautiousMode();
			if (rogue.lastTravelLoc[0] != 0 && rogue.lastTravelLoc[1] != 0) {
				travel(rogue.lastTravelLoc[0], rogue.lastTravelLoc[1], (controlKey || shiftKey));
			} else {
				examineMode();
			}
			break;
		case EXAMINE_KEY:
			examineMode();
			break;
		case EXPLORE_KEY:
			considerCautiousMode();
			explore(controlKey ? 1 : 30);
			break;
		case AUTOPLAY_KEY:
			autoPlayLevel(controlKey);
			break;
		case HELP_KEY:
			printHelpScreen();
			break;
		case DISCOVERIES_KEY:
			printDiscoveriesScreen();
			break;
		case VIEW_RECORDING_KEY:
			if (rogue.playbackMode) {
				return;
			}
			confirmMessages();
			if ((rogue.turnNumber < 50 || confirm("End this game and load a saved recording? (y/n)", false))
				&& openFile("View recording: ", "Recording", RECORDING_SUFFIX)) {
				freeEverything();
				randomNumbersGenerated = 0;
				rogue.playbackMode = true;
				initializeRogue();
				startLevel(rogue.depthLevel, 1);
				displayAnnotation(); // in case there's an annotation for turn 0
			}
			break;
		case LOAD_SAVED_GAME_KEY:
			if (rogue.playbackMode) {
				return;
			}
			confirmMessages();
			if ((rogue.turnNumber < 50 || confirm("End this game and load a saved game? (y/n)", false))
				&& openFile("Open saved game: ", "Saved game", GAME_SUFFIX)) {
				loadSavedGame();
			}
			break;
		case SAVE_GAME_KEY:
			if (rogue.playbackMode) {
				return;
			}
			if (confirm("Suspend this game? (This feature is still in beta.) (y/n)", false)) {
				saveGame();
			}
			break;
		case SEED_KEY:
			/*DEBUG {
				cellDisplayBuffer dbuf[COLS][ROWS];
				copyDisplayBuffer(dbuf, displayBuffer);
				funkyFade(dbuf, &white, 0, 100, mapToWindowX(player.xLoc), mapToWindowY(player.yLoc), false);
			}*/
			// DEBUG showWaypoints();
			// DEBUG displayLoops();
			// DEBUG displayChokeMap();
			//DEBUG displayMachines();
			// parseFile();
			// DEBUG spawnDungeonFeature(player.xLoc, player.yLoc, &dungeonFeatureCatalog[DF_METHANE_GAS_ARMAGEDDON], true, false);
			printSeed();
			// DEBUG victory();
			DEBUG crystalize();
			break;
		case EASY_MODE_KEY:
			if (shiftKey) {
				enableEasyMode();
			}
			break;
		default:
			break;
	}
	if (direction >= 0) { // if it was a movement command
		considerCautiousMode();
		if (controlKey || shiftKey) {
			playerRuns(direction);
		} else {
			playerMoves(direction);
		}
		refreshSideBar(NULL);
		if (D_SAFETY_VISION) {
			displaySafetyMap();
		}
	}
	
	rogue.cautiousMode = false;
}

boolean getInputTextString(char *inputText, char *prompt, short maxLength,
						   char *defaultEntry, char *promptSuffix, short textEntryType) {
	short charNum, charStartNum, i;
	char keystroke, suffix[100];
	const short textEntryBounds[TEXT_INPUT_TYPES][2] = {{' ', '~'}, {'0', '9'}};
	
	charStartNum = mapToWindowX(strLenWithoutEscapes(prompt));
	maxLength = min(maxLength, COLS - charStartNum);
	
	confirmMessages();
	message(prompt, true, false);
	printString(defaultEntry, charStartNum, MESSAGE_LINES - 1, &white, &black, 0);
	
	strcpy(inputText, defaultEntry);
	charNum = strLenWithoutEscapes(inputText);
	for (i = charNum; i < maxLength; i++) {
		inputText[i] = ' ';
	}
	
	if (promptSuffix[0] == '\0') { // empty suffix
		strcpy(suffix, " "); // so that deleting doesn't leave a white trail
	} else {
		strcpy(suffix, promptSuffix);
	}
	
	do {
		printString(suffix, charNum + charStartNum, MESSAGE_LINES - 1, &gray, &black, 0);
		plotCharWithColor((suffix[0] ? suffix[0] : ' '), charStartNum + charNum, MESSAGE_LINES - 1, black, white);
		keystroke = nextKeyPress();
		if (keystroke == DELETE_KEY && charNum > 0) {
			printString(suffix, charNum + charStartNum - 1, MESSAGE_LINES - 1, &gray, &black, 0);
			plotCharWithColor(' ', charStartNum + charNum + strlen(suffix) - 1, MESSAGE_LINES - 1, black, black);
			charNum--;
			inputText[charNum] = ' ';
		} else if (keystroke >= textEntryBounds[textEntryType][0]
				   && keystroke <= textEntryBounds[textEntryType][1]) { // allow only ASCII input
			inputText[charNum] = keystroke;
			plotCharWithColor(keystroke, charStartNum + charNum, MESSAGE_LINES - 1, white, black);
			printString(suffix, charNum + charStartNum + 1, MESSAGE_LINES - 1, &gray, &black, 0);
			if (charNum < maxLength) {
				charNum++;
			}
		}
	} while (keystroke != RETURN_KEY && keystroke != ESCAPE_KEY && keystroke != ENTER_KEY);
	
	inputText[charNum] = '\0';
	
	if (keystroke == ESCAPE_KEY) {
		return false;
	}
	strcat(displayedMessage[0], inputText);
	strcat(displayedMessage[0], suffix);
	return true;
}

void displayCenteredAlert(char *message) {
	printString(message, (COLS - strLenWithoutEscapes(message)) / 2, ROWS / 2, &teal, &black, 0);
}

void flashTemporaryAlert(char *message, int time) {
	boolean fastForward;
	int		i, j, x, messageLength, percentComplete, previousPercentComplete;
	color backColors[COLS], backColor, foreColor;
	cellDisplayBuffer dbufs[COLS];
	uchar dchar;
	
	if (rogue.playbackFastForward) {
		return;
	}
	
	assureCosmeticRNG;
	
	messageLength = strLenWithoutEscapes(message);
	fastForward = false;
	
	x = (COLS - messageLength) / 2;
	for (j=0; j<messageLength; j++) {
		backColors[j] = colorFromComponents(displayBuffer[j + x][ROWS / 2].backColorComponents);
		dbufs[j] = displayBuffer[j + x][ROWS / 2];
	}
	
	previousPercentComplete = -1;
	for (i=0; i < time && fastForward == false; i++) {
		percentComplete = 100 * i / time;
		percentComplete = percentComplete * percentComplete / 100; // transition is front-loaded
		if (previousPercentComplete != percentComplete) {
			for (j=0; j<messageLength; j++) {
				if (i==0) {
					backColors[j] = colorFromComponents(displayBuffer[j + x][ROWS / 2].backColorComponents);
					dbufs[j] = displayBuffer[j + x][ROWS / 2];
				}
				backColor = backColors[j];
				applyColorAverage(&backColor, &black, 100 - percentComplete);
				if (percentComplete < 50) {
					dchar = message[j];
					foreColor = teal;
					applyColorAverage(&foreColor, &backColor, percentComplete * 2);
				} else {
					dchar = dbufs[j].character;
					foreColor = colorFromComponents(dbufs[j].foreColorComponents);
					applyColorAverage(&foreColor, &backColor, (100 - percentComplete) * 2);
				}
				plotCharWithColor(dchar, j+x, ROWS/2, foreColor, backColor);
			}
		}
		previousPercentComplete = percentComplete;
		fastForward = pauseBrogue(1);
	}
	for (j=0; j<messageLength; j++) {
		plotCharWithColor(dbufs[j].character, j+x, ROWS/2, colorFromComponents(dbufs[j].foreColorComponents), backColors[j]);
	}
	
	restoreRNG;
}

void waitForAcknowledgment() {
	unsigned short int key;
	
	if (rogue.autoPlayingLevel || (rogue.playbackMode && !rogue.playbackOOS)) {
		return;
	}
	
	do {
		key = nextKeyPress();
		if (key != ACKNOWLEDGE_KEY && key != ESCAPE_KEY) {
			flashTemporaryAlert(" -- Press space to continue -- ", 500);
		}
	} while (key != ACKNOWLEDGE_KEY && key != ESCAPE_KEY);
}

void waitForKeystrokeOrMouseClick() {
	rogueEvent theEvent;
	do {
		nextBrogueEvent(&theEvent, false, false);
	} while (theEvent.eventType != KEYSTROKE && theEvent.eventType != MOUSE_UP);
}

boolean confirm(char *prompt, boolean alsoDuringPlayback) {
	char keystroke;
	
	if (rogue.autoPlayingLevel || (!alsoDuringPlayback && rogue.playbackMode)) {
		return true; // oh yes he did
	}
	
	message(prompt, true, false);
	keystroke = nextKeyPress();
	confirmMessages();
	if (keystroke == 'Y' || keystroke == 'y') {
		return true;
	}
	return false;
}

void displayMonsterFlashes(boolean flashingEnabled) {
	creature *monst;
	short x[100], y[100], strength[100], count = 0;
	color *flashColor[100];
	
	rogue.creaturesWillFlashThisTurn = false;
	
	if (rogue.autoPlayingLevel || rogue.blockCombatText) {
		return;
	}
	
	assureCosmeticRNG;
	
	CYCLE_MONSTERS_AND_PLAYERS(monst) {
		if (monst->bookkeepingFlags & MONST_WILL_FLASH) {
			monst->bookkeepingFlags &= ~MONST_WILL_FLASH;
			if (flashingEnabled && canSeeMonster(monst)) {
				x[count] = monst->xLoc;
				y[count] = monst->yLoc;
				strength[count] = monst->flashStrength;
				flashColor[count] = &(monst->flashColor);
				count++;
			}
		}
	}
	flashForeground(x, y, flashColor, strength, count, 250);
	restoreRNG;
}

void temporaryMessage(char *msg, boolean requireAcknowledgment) {
	char message[COLS];
	short i, j;
	
	assureCosmeticRNG;
	strcpy(message, msg);
	
	for (i=0; message[i] == COLOR_ESCAPE; i += 4) {
		upperCase(&(message[i]));
	}
	
	refreshSideBar(NULL);
	
	for (i=0; i<MESSAGE_LINES; i++) {
		for (j=0; j<DCOLS; j++) {
			plotCharWithColor(' ', mapToWindowX(j), i, black, black);
		}
	}
	printString(message, mapToWindowX(0), mapToWindowY(-1), &white, &black, 0);
	if (requireAcknowledgment) {
		waitForAcknowledgment();
		updateMessageDisplay();
	}
	restoreRNG;
}

void messageWithColor(char *msg, color *theColor, boolean requireAcknowledgment) {
	char buf[COLS*2];
	short i;
	
	i=0;
	i = encodeMessageColor(buf, i, theColor);
	strcpy(&(buf[i]), msg);
	message(buf, true, requireAcknowledgment);
}

void message(char *msg, boolean primaryMessage, boolean requireAcknowledgment) {
	char message[COLS*2];
	short i;
	
	assureCosmeticRNG;
	
	for (i=0; i < COLS*2 && msg[i] != '\0'; i++) {
		message[i] = msg[i];
	}
	message[i] = '\0';
	
	if (message[0] == COLOR_ESCAPE) {
		upperCase(&(message[4]));
	} else {
		upperCase(message);
	}
	
	if (primaryMessage) {
		rogue.disturbed = true;
		if (requireAcknowledgment) {
			refreshSideBar(NULL);
		}
		displayCombatText();
		
		// need to confirm the oldest message?
		if (!messageConfirmed[MESSAGE_LINES - 1]) {
			displayMoreSign();
			for (i=0; i<MESSAGE_LINES; i++) {
				messageConfirmed[i] = true;
			}
		}
		
		for (i = MESSAGE_LINES - 1; i >= 1; i--) {
			messageConfirmed[i] = messageConfirmed[i-1];
			strcpy(displayedMessage[i], displayedMessage[i-1]);
		}
		messageConfirmed[0] = false;
		strcpy(displayedMessage[0], message);
		
		// display the message:
		updateMessageDisplay();
		
		if (requireAcknowledgment || rogue.cautiousMode) {
			displayMoreSign();
			confirmMessages();
			rogue.cautiousMode = false;
		}
		
		if (rogue.playbackMode) {
			rogue.playbackDelayThisTurn += rogue.playbackDelayPerTurn * 5;
		}
	} else {
		// flavor text
		printString(message, mapToWindowX(0), ROWS - 1, &flavorTextColor, &black, 0);
		for (i = strLenWithoutEscapes(message); i < DCOLS; i++) {
			plotCharWithColor(' ', mapToWindowX(i), ROWS - 1, black, black);
		}
	}
	restoreRNG;
}

void displayMoreSign() {
	short i;
	
	if (rogue.autoPlayingLevel) {
		return;
	}
	
	if (strLenWithoutEscapes(displayedMessage[0]) < DCOLS - 8 || messageConfirmed[0]) {
		printString("--MORE--", COLS - 8, MESSAGE_LINES-1, &black, &white, 0);
		waitForAcknowledgment();
		printString("        ", COLS - 8, MESSAGE_LINES-1, &black, &black, 0);
	} else {
		printString("--MORE--", COLS - 8, MESSAGE_LINES, &black, &white, 0);
		waitForAcknowledgment();
		for (i=1; i<=8; i++) {
			refreshDungeonCell(DCOLS - i, 0);
		}
	}
}

// Inserts a four-character color escape sequence into a string at the insertion point.
// Does NOT check string lengths, so it could theoretically write over the null terminator.
// Returns the new insertion point.
short encodeMessageColor(char *msg, short i, const color *theColor) {
	boolean needTerminator = false;
	color col = *theColor;
	
	assureCosmeticRNG;
	
	bakeColor(&col);
	
	col.red		= clamp(col.red, 0, 100);
	col.green	= clamp(col.green, 0, 100);
	col.blue	= clamp(col.blue, 0, 100);
	
	needTerminator = !msg[i] || !msg[i + 1] || !msg[i + 2] || !msg[i + 3];
	
	msg[i++] = COLOR_ESCAPE;
	msg[i++] = (char) (COLOR_VALUE_INTERCEPT + col.red);
	msg[i++] = (char) (COLOR_VALUE_INTERCEPT + col.green);
	msg[i++] = (char) (COLOR_VALUE_INTERCEPT + col.blue);
	
	if (needTerminator) {
		msg[i] = '\0';
	}
	
	restoreRNG;
	
	return i;
}

// Call this when the i'th character of msg is COLOR_ESCAPE.
// It will return the encoded color, and will advance i past the color escape sequence.
short decodeMessageColor(const char *msg, short i, color *returnColor) {
	
	if (msg[i] != COLOR_ESCAPE) {
		printf("\nAsked to decode a color escape that didn't exist!");
		*returnColor = white;
	} else {
		i++;
		*returnColor = black;
		returnColor->red	= (short) (msg[i++] - COLOR_VALUE_INTERCEPT);
		returnColor->green	= (short) (msg[i++] - COLOR_VALUE_INTERCEPT);
		returnColor->blue	= (short) (msg[i++] - COLOR_VALUE_INTERCEPT);
		
		returnColor->red	= clamp(returnColor->red, 0, 100);
		returnColor->green	= clamp(returnColor->green, 0, 100);
		returnColor->blue	= clamp(returnColor->blue, 0, 100);
	}
	return i;
}

// Returns a color for combat text based on the identity of the victim.
color *messageColorFromVictim(creature *monst) {
	if (monstersAreEnemies(&player, monst)) {
		return &goodCombatMessageColor;
	} else if (monst == &player || monst->creatureState == MONSTER_ALLY) {
		return &badCombatMessageColor;
	} else {
		return &white;
	}
}

void updateMessageDisplay() {
	short i, j, m;
	color messageColor;
	
	for (i=0; i<MESSAGE_LINES; i++) {
		messageColor = white;
		
		if (messageConfirmed[i]) {
			applyColorAverage(&messageColor, &black, 50);
			applyColorAverage(&messageColor, &black, 75 * i / MESSAGE_LINES);
		}
		
		for (j = m = 0; displayedMessage[i][m] && j < DCOLS; j++, m++) {
			
			while (displayedMessage[i][m] == COLOR_ESCAPE) {
				m = decodeMessageColor(displayedMessage[i], m, &messageColor); // pulls the message color out and advances m
				if (messageConfirmed[i]) {
					applyColorAverage(&messageColor, &black, 50);
					applyColorAverage(&messageColor, &black, 75 * i / MESSAGE_LINES);
				}
			}
			
			plotCharWithColor(displayedMessage[i][m], mapToWindowX(j), MESSAGE_LINES - i - 1,
							  messageColor,
							  black);
		}
		for (; j < DCOLS; j++) {
			plotCharWithColor(' ', mapToWindowX(j), MESSAGE_LINES - i - 1, black, black);
		}
	}
}

void deleteMessages() {
	short i;
	for (i=0; i<MESSAGE_LINES; i++) {
		displayedMessage[i][0] = '\0';
	}
	confirmMessages();
}

void confirmMessages() {
	short i;
	for (i=0; i<MESSAGE_LINES; i++) {
		messageConfirmed[i] = true;
	}
	updateMessageDisplay();
}

void stripShiftFromMovementKeystroke(unsigned short *keystroke) {
	unsigned short newKey = *keystroke - ('A' - 'a');
	if (newKey == LEFT_KEY
		|| newKey == RIGHT_KEY
		|| newKey == DOWN_KEY
		|| newKey == UP_KEY
		|| newKey == UPLEFT_KEY
		|| newKey == UPRIGHT_KEY
		|| newKey == DOWNLEFT_KEY
		|| newKey == DOWNRIGHT_KEY) {
		*keystroke -= 'A' - 'a';
	}
}

void upperCase(char *theChar) {
	if (*theChar >= 'a' && *theChar <= 'z') {
		(*theChar) += ('A' - 'a');
	}
}

void refreshSideBar(creature *focusMonst) {
	short printY, shortestDistance, i, x, y;
	creature *monst, *closestMonst;
	//item *theItem, *closestItem;
	char buf[COLS];
	
	if (rogue.gameHasEnded) {
		return;
	}
	
	assureCosmeticRNG;
	
	printY = 0;
	
	x = player.xLoc;
	y = player.yLoc;
	
	if (rogue.playbackMode) {
		printString("   -- PLAYBACK --   ", 0, printY++, &white, &black, 0);
		if (rogue.howManyTurns > 0) {
			sprintf(buf, "Turn %li/%li", rogue.turnNumber, rogue.howManyTurns);
			printProgressBar(0, printY++, buf, rogue.turnNumber, rogue.howManyTurns, &darkPurple, false);
		}
		printString("                    ", 0, printY++, &white, &black, 0);
	}
	
	printY = printMonsterInfo(&player, printY, (focusMonst ? true : false));
	
	for (monst = monsters->nextCreature; monst != NULL; monst = monst->nextCreature) {
		monst->bookkeepingFlags &= ~MONST_ADDED_TO_STATS_BAR;
	}
	
	if (focusMonst) {
		printY = printMonsterInfo(focusMonst, printY, false);
		focusMonst->bookkeepingFlags |= MONST_ADDED_TO_STATS_BAR;
	}
	
	do {
		shortestDistance = 10000;
		for (monst = monsters->nextCreature; monst != NULL; monst = monst->nextCreature) {
			if (canSeeMonster(monst) && !(monst->bookkeepingFlags & MONST_ADDED_TO_STATS_BAR)
				&& (x - monst->xLoc) * (x - monst->xLoc) + (y - monst->yLoc) * (y - monst->yLoc) < shortestDistance) {
				shortestDistance = (x - monst->xLoc) * (x - monst->xLoc) + (y - monst->yLoc) * (y - monst->yLoc);
				closestMonst = monst;
			}
		}
		if (shortestDistance < 10000) {
			printY = printMonsterInfo(closestMonst, printY, (focusMonst ? true : false));
			closestMonst->bookkeepingFlags |= MONST_ADDED_TO_STATS_BAR;
		}
	} while (shortestDistance < 10000 && printY < ROWS);
	
	for (i=printY; i< ROWS - 1; i++) {
		printString("                    ", 0, i, &white, &black, 0);
	}
	sprintf(buf, "  -- Depth: %i --%s   ", rogue.depthLevel, (rogue.depthLevel < 10 ? " " : ""));
	printString(buf, 0, ROWS - 1, &white, &black, 0);
	
	restoreRNG;
}

void printString(const char *theString, short x, short y, color *foreColor, color *backColor, cellDisplayBuffer dbuf[COLS][ROWS]) {
	color fColor;
	short i;
	
	fColor = *foreColor;
	
	for (i=0; theString[i] != '\0' && x < COLS; i++, x++) {
		while (theString[i] == COLOR_ESCAPE) {
			i = decodeMessageColor(theString, i, &fColor);
			if (!theString[i]) {
				break;
			}
		}
		
		if (dbuf) {
			plotCharToBuffer(theString[i], x, y, &fColor, backColor, dbuf);
		} else {
			plotCharWithColor(theString[i], x, y, fColor, *backColor);
		}
	}
}

// Returns the number of lines, including the newlines already in the text.
// Puts the output in "to" only if we receive a "to" -- can make it null and just get a line count.
short wrapText(char *to, const char *sourceText, short width) {
	boolean increaseWidth;
	short i, scanChar, lineWidth, textLength, lineCount, lineStartIndex;
	char printString[COLS * ROWS * 2];
	
	strcpy(printString, sourceText); // a copy we can write on
	textLength = strlen(printString); // do NOT discount escape sequences
	lineCount = 1;
	lineStartIndex = 0;
	
	// first go through and replace spaces with newlines as needed
	for (scanChar = 0, lineWidth = 0;
		 scanChar < textLength;
		 scanChar++) {
		increaseWidth = true;
		
		if (lineWidth >= width
			&& printString[scanChar] != ' '
			&& printString[scanChar] != COLOR_ESCAPE
			&& printString[scanChar] != '\n') { // reached the end of this line
			for (i=scanChar; i >= lineStartIndex; i--) { // go back to find the last space
				if (printString[i] == ' '
					&& (i < 1 || printString[i-1] != COLOR_ESCAPE)
					&& (i < 2 || printString[i-2] != COLOR_ESCAPE)
					&& (i < 3 || printString[i-3] != COLOR_ESCAPE)) { // newlines embedded in color escape sequences don't count
					printString[i] = '\n'; // replace it with a newline
					//lineCount++;
					lineWidth = 0;
					lineStartIndex = scanChar = i + 1; // resume scanning from one character after the new newline
					break;
				}
			}
		}
		
		if (scanChar > 0 && printString[scanChar - 1] == '\n') { // previous character is a newline
			lineCount++;
			lineStartIndex = lineWidth = 0;
			increaseWidth = false;
		}
		
		if (printString[scanChar] == COLOR_ESCAPE) {
			scanChar += 3;
			increaseWidth = false;
		}
		
		if (increaseWidth) {
			lineWidth++;
		}
	}
	if (to) {
		strcpy(to, printString);
	}
	
	// count the newlines directly
	lineCount = 0;
	for (i=0; i<textLength; i++) {
		while (printString[i] == COLOR_ESCAPE) {
			i += 4;
		}
		if (printString[i] == '\n') {
			lineCount++;
		}
	}
	return lineCount;
}

// returns the y-coordinate of the last line
short printStringWithWrapping(char *theString, short x, short y, short width, color *foreColor,
							 color*backColor, cellDisplayBuffer dbuf[COLS][ROWS]) {
	color fColor;
	char printString[COLS * ROWS * 2];
	short i, px, py;
	
	wrapText(printString, theString, width);
	
	// display the string
	px = x; //px and py are the print insertion coordinates; x and y remain the top-left of the text box
	py = y;
	fColor = *foreColor;
	
	for (i=0; printString[i] != '\0'; i++) {
		if (printString[i] == '\n') {
			px = x; // back to the leftmost column
			if (py < ROWS - 1) { // don't advance below the bottom of the screen
				py++; // next line
			} else {
				break; // If we've run out of room, stop.
			}
			continue;
		} else if (printString[i] == COLOR_ESCAPE) {
			i = decodeMessageColor(printString, i, &fColor) - 1;
			continue;
		}
		
		if (dbuf) {
			if (coordinatesAreInWindow(px, py)) {
				plotCharToBuffer(printString[i], px, py, &fColor, backColor, dbuf);
			}
		} else {
			if (coordinatesAreInWindow(px, py)) {
				plotCharWithColor(printString[i], px, py, fColor, *backColor);
			}
		}
		
		px++;
	}
	return py;
}

char nextKeyPress() {
	rogueEvent theEvent;
	do {
		nextBrogueEvent(&theEvent, false, false);
	} while (theEvent.eventType != KEYSTROKE);
	return theEvent.param1;
}

#define BROGUE_HELP_LINE_COUNT	32

void printHelpScreen() {
	short i, j;
	cellDisplayBuffer dbuf[COLS][ROWS], rbuf[COLS][ROWS];
	char helpText[BROGUE_HELP_LINE_COUNT][DCOLS*3] = {
		"",
		"          ****-- Commands --",
		"",
		"            x: ****examine your surroundings",
		"            i: ****view inventory (and then (A-Z) to examine an item)",
		"",
		"hjklyubn, arrow keys, or numpad: ****move or attack",
		"<control> + hjklyubn, arrow keys, or numpad: ****run",
		"            a: ****apply or activate an item (eat, read, zap)",
		"            e: ****equip an item (armor, weapon or ring)",
		"            r: ****remove an item (armor, weapon or ring)",
		"            d: ****drop an item",
		"            t: ****throw an item",
		"            c: ****call an item something (i.e. name it)",
		"            z: ****rest (do nothing for one turn)",
		"            Z: ****sleep (rest until better or interrupted)",
		"            s: ****search for secret doors and traps",
		"            >: ****descend a flight of stairs",
		"            <: ****ascend a flight of stairs",
		"            f: ****fight monster (shift-f: fight to the death)",
		"            D: ****display discovered items",
		"            X: ****auto-explore the level (control-X: fast forward)",
		"            A: ****autopilot (control-A: fast forward)",
		"            S: ****save game and quit",
		"            O: ****open saved game and resume play",
		"            V: ****view saved recording",
		"     <return>: ****travel to previous travel destination",
		"      <space>: ****clear message",
		"        <esc>: ****cancel a command",
		"<mouse click>: ****travel to location (control-click: auto-confirm)",
		" ",
		"        -- press space to continue --"
	};
	
	// Replace the "****"s with color escapes.
	for (i=0; i<BROGUE_HELP_LINE_COUNT; i++) {
		for (j=0; helpText[i][j]; j++) {
			if (helpText[i][j] == '*') {
				j = encodeMessageColor(helpText[i], j, &white);
			}
		}
	}
	
	clearDisplayBuffer(dbuf);
	
	// Print the text to the dbuf.
	for (i=0; i<BROGUE_HELP_LINE_COUNT && i < ROWS; i++) {
		printString(helpText[i], mapToWindowX(1), i, &itemMessageColor, &black, dbuf);
	}
	
	// Set the dbuf opacity.
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<ROWS; j++) {
			//plotCharWithColor(' ', mapToWindowX(i), j, black, black);
			dbuf[mapToWindowX(i)][j].opacity = INTERFACE_OPACITY;
		}
	}
	
	// Display.
	overlayDisplayBuffer(dbuf, rbuf);
	displayMoreSign();
	overlayDisplayBuffer(rbuf, 0);
	updateFlavorText();
	updateMessageDisplay();
}

void printDiscoveries(short category, short count, unsigned short itemCharacter, short x, short y, cellDisplayBuffer dbuf[COLS][ROWS]) {
	color *theColor, goodColor, badColor;
	char buf[COLS];
	short i, x2, magic;
	itemTable *theTable = tableForItemCategory(category);
	
	goodColor = goodCombatMessageColor;
	applyColorAverage(&goodColor, &black, 50);
	badColor = badCombatMessageColor;
	applyColorAverage(&badColor, &black, 50);
	
	for (i = 0; i < count; i++) {
		if (theTable[i].identified) {
			theColor = &white;
			plotCharToBuffer(itemCharacter, x, y + i, &yellow, &black, dbuf);
		} else {
			theColor = &darkGray;
		}
		strcpy(buf, theTable[i].name);
		upperCase(buf);
		strcat(buf, " ");
		printString(buf, x + 2, y + i, theColor, &black, dbuf);
		
		x2 = x + 2 + strLenWithoutEscapes(buf);
		magic = magicCharDiscoverySuffix(category, i);
		plotCharToBuffer('(', x2++, y + i, &darkGray, &black, dbuf);
		if (magic != -1) {
			plotCharToBuffer(GOOD_MAGIC_CHAR, x2++, y + i, &goodColor, &black, dbuf);
		}
		if (magic != 1) {
			plotCharToBuffer(BAD_MAGIC_CHAR, x2++, y + i, &badColor, &black, dbuf);
		}
		plotCharToBuffer(')', x2++, y + i, &darkGray, &black, dbuf);
	}
}

void printDiscoveriesScreen() {
	short i, j, y;
	cellDisplayBuffer dbuf[COLS][ROWS], rbuf[COLS][ROWS];
	
	clearDisplayBuffer(dbuf);
	
	printString("-- SCROLLS --", mapToWindowX(3), y = mapToWindowY(1), &flavorTextColor, &black, dbuf);
	printDiscoveries(SCROLL, NUMBER_SCROLL_KINDS, SCROLL_CHAR, mapToWindowX(3), ++y, dbuf);

	printString("-- WANDS --", mapToWindowX(3), y += NUMBER_SCROLL_KINDS + 1, &flavorTextColor, &black, dbuf);
	printDiscoveries(WAND, NUMBER_WAND_KINDS, WAND_CHAR, mapToWindowX(3), ++y, dbuf);
	
	printString("-- POTIONS --", mapToWindowX(29), y = mapToWindowY(1), &flavorTextColor, &black, dbuf);
	printDiscoveries(POTION, NUMBER_POTION_KINDS, POTION_CHAR, mapToWindowX(29), ++y, dbuf);
	
	printString("-- STAFFS --", mapToWindowX(54), y = mapToWindowY(1), &flavorTextColor, &black, dbuf);
	printDiscoveries(STAFF, NUMBER_STAFF_KINDS, STAFF_CHAR, mapToWindowX(54), ++y, dbuf);
	
	printString("-- RINGS --", mapToWindowX(54), y += NUMBER_STAFF_KINDS + 1, &flavorTextColor, &black, dbuf);
	printDiscoveries(RING, NUMBER_RING_KINDS, RING_CHAR, mapToWindowX(54), ++y, dbuf);
	
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			dbuf[i][j].opacity = (i < STAT_BAR_WIDTH ? 0 : INTERFACE_OPACITY);
		}
	}
	overlayDisplayBuffer(dbuf, rbuf);
	
	displayMoreSign();
	
	overlayDisplayBuffer(rbuf, NULL);
//	confirmMessages();
//	displayLevel();
}

#define scorePercentAt(y, k)	(k + (100-k) * (COLS - y) / COLS / 100)

void printHighScores(boolean hiliteMostRecent) {
	short i, k, hiliteLineNum, maxLength = 0, leftOffset;
	rogueHighScoresEntry list[25];
	char buf[DCOLS*3];
	color scoreColor;
	boolean fastForward;
	
	hiliteLineNum = getHighScoresList(list);
	
	if (!hiliteMostRecent) {
		hiliteLineNum = -1;
	}
	
	blackOutScreen();
	
	for (i = 0; i < 25 && list[i].score > 0; i++) {
		if (strLenWithoutEscapes(list[i].description) > maxLength) {
			maxLength = strLenWithoutEscapes(list[i].description);
		}
	}
	
	leftOffset = min(COLS - maxLength - 21 - 1, COLS/5);
	
	fastForward = false;
	
	for (k=1; k<=100; k++) {
		
		scoreColor = black;
		applyColorAverage(&scoreColor, &yellow, scorePercentAt(1, k));
		printString("-- HIGH SCORES --", (COLS - 17 + 1) / 2, 1, &scoreColor, &black, 0);
		
		for (i = 0; i < 25 && list[i].score > 0; i++) {
			scoreColor = black;
			if (i == hiliteLineNum) {
				applyColorAverage(&scoreColor, &teal, scorePercentAt(i+3, k));
			} else {
				applyColorAverage(&scoreColor, &white, scorePercentAt(i+3, k));
				applyColorAverage(&scoreColor, &black, (i * 50 / 24));
			}
			
			// number
			sprintf(buf, "%s%i)", (i + 1 < 10 ? " " : ""), i + 1);
			printString(buf, leftOffset, i + 3, &scoreColor, &black, 0);
			
			// score
			sprintf(buf, "%li", list[i].score);
			printString(buf, leftOffset + 5, i + 3, &scoreColor, &black, 0);
			
			// date
			printString(list[i].date, leftOffset + 12, i + 3, &scoreColor, &black, 0);
			
			// description
			printString(list[i].description, leftOffset + 21, i + 3, &scoreColor, &black, 0);
		}
		
		scoreColor = black;
		applyColorAverage(&scoreColor, &gray, scorePercentAt(ROWS - 1, k));
		printString(PLAY_AGAIN_STRING, (COLS - strLenWithoutEscapes(PLAY_AGAIN_STRING)) / 2, ROWS - 1, &scoreColor, &black, 0);
		
		if (!fastForward) {
			fastForward = pauseBrogue(10);
			if (fastForward) {
				k = max(99, k);
			}
		}
	}
}

void showWaypoints() {
	short i, j, k, n, start[2], end[2];
	short coords[DCOLS][2];
	
	for (i = 0; i < numberOfWaypoints; i++) {
		start[0] = waypoints[i].x;
		start[1] = waypoints[i].y;
		for (j = 0; j < waypoints[i].connectionCount; j++) {
			end[0] = waypoints[i].connection[j][0];
			end[1] = waypoints[i].connection[j][1];
			n = getLineCoordinates(coords, start, end);
			for (k = 0; k < n && (coords[k][0] != end[0] || coords[k][1] != end[1]); k++) {
				hiliteCell(coords[k][0], coords[k][1], &yellow, 50, true);
			}
		}
	}
	
	for (i = 0; i < numberOfWaypoints; i++) {
		hiliteCell(waypoints[i].x, waypoints[i].y, &white, 50, true);
	}
}

void displaySafetyMap() {
	short i, j, score, topRange, bottomRange;
	color tempColor, foreColor, backColor;
	uchar dchar;
	
	topRange = -30000;
	bottomRange = 30000;
	tempColor = black;
	
	if (!rogue.updatedSafetyMapThisTurn) {
		updateSafetyMap();
	}
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (cellHasTerrainFlag(i, j, T_WAYPOINT_BLOCKER) || (safetyMap[i][j] == safetyMap[0][0]) || (i == player.xLoc && j == player.yLoc)) {
				continue;
			}
			if (safetyMap[i][j] > topRange) {
				topRange = safetyMap[i][j];
				//if (topRange == 0) {
					//printf("\ntop is zero at %i,%i", i, j);
				//}
			}
			if (safetyMap[i][j] < bottomRange) {
				bottomRange = safetyMap[i][j];
			}
		}
	}
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			if (cellHasTerrainFlag(i, j, T_OBSTRUCTS_PASSABILITY | T_LAVA_INSTA_DEATH)
				|| (safetyMap[i][j] == safetyMap[0][0])
				|| (i == player.xLoc && j == player.yLoc)) {
				continue;
			}
			score = 300 - (safetyMap[i][j] - bottomRange) * 300 / max(1, (topRange - bottomRange));
			tempColor.blue = max(min(score, 100), 0);
			score -= 100;
			tempColor.red = max(min(score, 100), 0);
			score -= 100;
			tempColor.green = max(min(score, 100), 0);
			getCellAppearance(i, j, &dchar, &foreColor, &backColor);
			plotCharWithColor(dchar, mapToWindowX(i), mapToWindowY(j), foreColor, tempColor);
			//colorBlendCell(i, j, &tempColor, 100);//hiliteCell(i, j, &tempColor, 100, false);
		}
	}
	//printf("\ntop: %i; bottom: %i", topRange, bottomRange);
}

void printSeed() {
	char buf[COLS];
	sprintf(buf, "Level sequence ID #%lu", rogue.seed);
	message(buf, true, false);	
}

void printProgressBar(short x, short y, char barLabel[COLS], long amtFilled, long amtMax, color *fillColor, boolean dim) {
	char barText[] = "                    "; // string length is 20
	short i, labelOffset;
	color currentFillColor, textColor, progressBarColor, darkenedBarColor;
	
	if (y >= ROWS - 1) { // don't write over the depth number
		return;
	}
	
	if (amtMax <= 0) {
		amtMax = 1;
	}
	
	progressBarColor = *fillColor;
	if (!(y % 2)) {
		applyColorAverage(&progressBarColor, &black, 25);
	}
	
	if (dim) {
		applyColorAverage(&progressBarColor, &black, 50);
		applyColorAverage(&darkenedBarColor, &black, 50);
	}
	
	darkenedBarColor = progressBarColor;
	
	applyColorAverage(&darkenedBarColor, &black, 75);
	
	labelOffset = (20 - strlen(barLabel)) / 2;
	for (i=0; i<strlen(barLabel); i++) {
		barText[i + labelOffset] = barLabel[i];
	}
	
	amtFilled = max(0, min(amtFilled, amtMax));
	
	if (amtMax < 10000000) {
		amtFilled *= 100;
		amtMax *= 100;
	}
	
	for (i=0; i<20; i++) {
		currentFillColor = (i <= (20 * amtFilled / amtMax) ? progressBarColor : darkenedBarColor);
		if (i == 20 * amtFilled / amtMax) {
			applyColorAverage(&currentFillColor, &black, 75 - 75 * (amtFilled % (amtMax / 20)) / (amtMax / 20));
		}
		textColor = (dim ? gray : white);
		applyColorAverage(&textColor, &currentFillColor, (dim ? 50 : 33));
		plotCharWithColor(barText[i], x + i, y, textColor, currentFillColor);
	}
}

// returns the y-coordinate after the last line printed
short printMonsterInfo(creature *monst, short y, boolean dim) {
	char buf[COLS], monstName[COLS];
	uchar monstChar;
	color monstForeColor, monstBackColor;
	long amtFilled, amtMax;
	
	char hallucinationStrings[10][COLS] = {
		"     (Dancing)      ",
		"     (Singing)      ",
		"  (Pontificating)   ",
		"     (Skipping)     ",
		"     (Spinning)     ",
		"      (Crying)      ",
		"     (Laughing)     ",
		"     (Humming)      ",
		"    (Whistling)     ",
		"    (Quivering)     ",
	};
	
	if (y >= ROWS - 1) {
		return ROWS - 1;
	}
	
	assureCosmeticRNG;
	
	if (y < ROWS - 1) {
		printString("                    ", 0, y, &white, &black, 0);
		getCellAppearance(monst->xLoc, monst->yLoc, &monstChar, &monstForeColor, &monstBackColor);
		if (dim) {
			applyColorAverage(&monstForeColor, &black, 50);
			applyColorAverage(&monstBackColor, &black, 50);
		}
		plotCharWithColor(monstChar, 0, y, monstForeColor, monstBackColor);
		monsterName(monstName, monst, false);
		upperCase(monstName);
		
		if (monst == &player) {
			sprintf(buf, ": %s, Level %i", monstName, rogue.experienceLevel);
		} else {
			sprintf(buf, ": %s", monstName);
		}
		printString(buf, 1, y++, (dim ? &gray : &white), &black, 0);
	}
	
	// hit points
	printProgressBar(0, y++, "Health", monst->currentHP, monst->info.maxHP, &blueBar, dim);
	
	if (monst == &player) {
		
		// nutrition
		if (player.status.nutrition > HUNGER_THRESHOLD) {
			printProgressBar(0, y++, "Nutrition", player.status.nutrition, STOMACH_SIZE, &blueBar, dim);
		} else if (player.status.nutrition > WEAK_THRESHOLD) {
			printProgressBar(0, y++, "Nutrition (Hungry)", player.status.nutrition, STOMACH_SIZE, &blueBar, dim);
		} else if (player.status.nutrition > FAINT_THRESHOLD) {
			printProgressBar(0, y++, "Nutrition (Weak)", player.status.nutrition, STOMACH_SIZE, &blueBar, dim);
		} else {
			printProgressBar(0, y++, "Nutrition (Faint)", player.status.nutrition, STOMACH_SIZE, &blueBar, dim);
		}
		
		// experience
		sprintf(buf, "%i%s   Experience   %s%i", rogue.experienceLevel, (rogue.experienceLevel < 10 ? " " : ""),
				(rogue.experienceLevel < 9 ? " " : ""), rogue.experienceLevel + 1);
		amtFilled = rogue.experience - (rogue.experienceLevel == 1 ? 0 : levelPoints[rogue.experienceLevel - 2]);
		amtMax = (rogue.experienceLevel == 1 ? levelPoints[0]
				  : levelPoints[rogue.experienceLevel - 1] - levelPoints[rogue.experienceLevel - 2]);
		printProgressBar(0, y++, buf, amtFilled, amtMax, &blueBar, dim);
	}
	
	if (!player.status.hallucinating || rogue.playbackOmniscience || monst == &player) {
		if (monst->status.burning) {
			printProgressBar(0, y++, "Burning", monst->status.burning, monst->maxStatus.burning, &redBar, dim);
		}
		if (monst->status.paralyzed) {
			printProgressBar(0, y++, "Paralyzed", monst->status.paralyzed, monst->maxStatus.paralyzed, &redBar, dim);
		}
		if (monst->status.stuck) {
			printProgressBar(0, y++, "Entangled", monst->status.stuck, monst->maxStatus.stuck, &redBar, dim);
		}
		if (monst->status.confused) {
			printProgressBar(0, y++, "Confused", monst->status.confused, monst->maxStatus.confused, &redBar, dim);
		}
		if (monst->status.hasted) {
			printProgressBar(0, y++, "Hasted", monst->status.hasted, monst->maxStatus.hasted, &redBar, dim);
		}
		if (monst->status.slowed) {
			printProgressBar(0, y++, "Slowed", monst->status.slowed, monst->maxStatus.slowed, &redBar, dim);
		}
		if (monst->status.immuneToFire) {
			printProgressBar(0, y++, "Immune to Fire", monst->status.immuneToFire, monst->maxStatus.immuneToFire, &redBar, dim);
		}
		if (monst->status.levitating) {
			printProgressBar(0, y++, (monst == &player ? "Levitating" : "Flying"), monst->status.levitating, monst->maxStatus.levitating, &redBar, dim);
		}
		if (monst->status.hallucinating) {
			printProgressBar(0, y++, "Hallucinating", monst->status.hallucinating, monst->maxStatus.hallucinating, &redBar, dim);
		}
		if (monst->status.darkness) {
			printProgressBar(0, y++, "Darkened", monst->status.darkness, monst->maxStatus.darkness, &redBar, dim);
		}
		if (monst->status.telepathic) {
			printProgressBar(0, y++, "Telepathic", monst->status.telepathic, monst->maxStatus.telepathic, &redBar, dim);
		}
		if (monst->status.magicalFear) {
			printProgressBar(0, y++, "Frightened", monst->status.magicalFear, monst->maxStatus.magicalFear, &redBar, dim);
		}
		if (monst->status.nauseous) {
			printProgressBar(0, y++, "Nauseous", monst->status.nauseous, monst->maxStatus.nauseous, &redBar, dim);
		}
		if (monst->status.poisoned) {
			printProgressBar(0, y++, "Poisoned", monst->status.poisoned, monst->maxStatus.poisoned, &redBar, dim);
		}
		if (monst->status.entranced) {
			printProgressBar(0, y++, "Entranced", monst->status.entranced, monst->maxStatus.entranced, &redBar, dim);
		}
		if (monst->status.discordant) {
			printProgressBar(0, y++, "Discordant", monst->status.discordant, monst->maxStatus.discordant, &redBar, dim);
		}
		if (monst != &player && monst->info.flags & MONST_REFLECT_4) {
			printProgressBar(0, y++, "Reflects Magic", 1, 1, &redBar, dim);
		}
		if (monst->status.lifespanRemaining) {
			printProgressBar(0, y++, "Lifespan", monst->status.lifespanRemaining, monst->maxStatus.lifespanRemaining, &redBar, dim);
		}
		if (monst->targetCorpseLoc[0] == monst->xLoc && monst->targetCorpseLoc[1] == monst->yLoc) {
			printProgressBar(0, y++,  monsterText[monst->info.monsterID].absorbStatus, monst->corpseAbsorptionCounter, 20, &redBar, dim);
		}
	}
	
	if (monst != &player
		&& (!(monst->info.flags & MONST_INANIMATE)
			|| monst->creatureState == MONSTER_ALLY)) {
		if (player.status.hallucinating && !rogue.playbackOmniscience && y < ROWS - 1) {
			printString(hallucinationStrings[rand_range(0, 9)], 0, y++, (dim ? &darkGray : &gray), &black, 0);
		} else if (monst->bookkeepingFlags & MONST_CAPTIVE && y < ROWS - 1) {
			printString("     (Captive)      ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
		} else if (monst->creatureState == MONSTER_SLEEPING && y < ROWS - 1) {
			printString("     (Sleeping)     ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
		} else if (monst->creatureState == MONSTER_FLEEING && y < ROWS - 1) {
			printString("     (Fleeing)      ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
		} else if ((monst->creatureState == MONSTER_TRACKING_SCENT) && y < ROWS - 1) {
			printString("     (Hunting)      ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
		} else if ((monst->creatureState == MONSTER_WANDERING) && y < ROWS - 1) {
			if ((monst->bookkeepingFlags & MONST_FOLLOWER) && monst->leader && (monst->leader->info.flags & MONST_IMMOBILE)) {
				// follower of an immobile leader -- i.e. a totem
				printString("    (Worshiping)    ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
			} else if ((monst->bookkeepingFlags & MONST_FOLLOWER) && monst->leader && (monst->leader->bookkeepingFlags & MONST_CAPTIVE)) {
				// actually a captor/torturer
				printString("     (Guarding)     ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
			} else {
				printString("    (Wandering)     ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
			}
		} else if ((monst->creatureState == MONSTER_ALLY) && y < ROWS - 1) {
			printString("       (Ally)       ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
		}
	} else if (monst == &player) {
		if (y < ROWS - 1) {
			if (!rogue.armor || rogue.armor->flags & ITEM_IDENTIFIED) {
				sprintf(buf, "Str: %i  Armor: %i", rogue.strength, player.info.defense / 10);
			} else {
				sprintf(buf, "Str: %i  Armor: %i?",
						rogue.strength,
						(armorTable[rogue.armor->kind].range.upperBound + armorTable[rogue.armor->kind].range.lowerBound) / 20);
			}
			buf[20] = '\0';
			printString("                    ", 0, y, &white, &black, 0);
			printString(buf, (20 - strLenWithoutEscapes(buf)) / 2, y++, (dim ? &darkGray : &gray), &black, 0);
		}
		if (y < ROWS - 1 && rogue.gold) {
			sprintf(buf, "Gold: %li", rogue.gold);
			buf[20] = '\0';
			printString("                    ", 0, y, &white, &black, 0);
			printString(buf, (20 - strLenWithoutEscapes(buf)) / 2, y++, (dim ? &darkGray : &gray), &black, 0);
		}
	}
	
	if (y < ROWS - 1) {
		printString("                    ", 0, y++, (dim ? &darkGray : &gray), &black, 0);
	}
	restoreRNG;
	return y;
}

void rectangularShading(short minX, short minY, short maxX, short maxY, const color *backColor, cellDisplayBuffer dbuf[COLS][ROWS]) {
	short i, j, dist;
	
	assureCosmeticRNG;
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			storeColorComponents(dbuf[i][j].backColorComponents, backColor);
			
			if (i >= minX && i <= maxX && j >= minY && j <= maxY) {
				dbuf[i][j].opacity = min(100, INTERFACE_OPACITY);
			} else {
				dist = 0;
				dist += max(0, max(minX - i, i - maxX));
				dist += max(0, max(minY - j, j - maxY));
				dbuf[i][j].opacity = (int) ((INTERFACE_OPACITY - 10) / max(1, dist));
				if (dbuf[i][j].opacity < 5) {
					dbuf[i][j].opacity = 0;
				}
			}
		}
	}
	restoreRNG;
}

#define MIN_DEFAULT_INFO_PANEL_WIDTH	38

// y and width are optional and will be automatically calculated if width <= 0.
// Width will automatically be widened if the text would otherwise fall off the bottom of the
// screen, and x will be adjusted to keep the widening box from falling off the right of the
// screen.
void printTextBox(char *textBuf, short x, short y, short width,
				  color *foreColor, color *backColor, cellDisplayBuffer rbuf[COLS][ROWS]) {
	cellDisplayBuffer dbuf[COLS][ROWS];
	short x2, y2, lineCount;
	
	if (width <= 0) {
		// autocalculate y and width
		if (x < DCOLS / 2 - 1) {
			x2 = mapToWindowX(x + 10);
			width = (DCOLS - x) - 20;
		} else {
			x2 = mapToWindowX(10);
			width = x - 20;
		}
		y2 = mapToWindowY(2);
		
		if (width < MIN_DEFAULT_INFO_PANEL_WIDTH) {
			x2 -= (MIN_DEFAULT_INFO_PANEL_WIDTH - width) / 2;
			width = MIN_DEFAULT_INFO_PANEL_WIDTH;
		}
	} else {
		y2 = y;
		x2 = x;
	}
	
	while (((lineCount = wrapText(NULL, textBuf, width)) + y2) >= ROWS && width < COLS-5) {
		// While the text doesn't fit and the width doesn't fill the screen, increase the width.
		width++;
		if (x2 + (width / 2) > COLS / 2) {
			// If the horizontal midpoint of the text box is on the right half of the screen,
			// move the box one space to the left.
			x2--;
		}
	}
	
	clearDisplayBuffer(dbuf);
	printStringWithWrapping(textBuf, x2, y2, width, foreColor, backColor, dbuf);
	rectangularShading(x2, y2, x2 + width, y2 + lineCount, backColor, dbuf);
	overlayDisplayBuffer(dbuf, rbuf);
}

void printMonsterDetails(creature *monst, cellDisplayBuffer rbuf[COLS][ROWS]) {
	char textBuf[COLS * 100];
	
	monsterDetails(textBuf, monst);
	printTextBox(textBuf, monst->xLoc, 0, 0, &white, &black, rbuf);
}

void printItemDetails(item *theItem, cellDisplayBuffer rbuf[COLS][ROWS]) {
	char textBuf[COLS * 100];
	
	itemDetails(textBuf, theItem);
	printTextBox(textBuf, theItem->xLoc, 0, 0, &white, &black, rbuf);
}
