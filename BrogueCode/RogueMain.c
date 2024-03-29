/*
 *  RogueMain.c
 *  Brogue
 *
 *  Created by Brian Walker on 12/26/08.
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

#include "Rogue.h"
#include "IncludeGlobals.h"
#include <math.h>
#include <time.h>

void rogueMain() {
	rogueEvent theEvent;
	
	randomNumbersGenerated = 0;
	
	rogue.playbackMode = false;
	rogue.playbackFastForward = false;
	rogue.playbackBetweenTurns = false;
	
	strcpy(currentFilePath, LAST_GAME_PATH);
	
	initializeRogue();
	startLevel(rogue.depthLevel, 1); // descending into level 1
	
	while(!rogue.gameHasEnded) {
	
		rogue.RNG = RNG_COSMETIC; // dancing terrain colors can't influence recordings
		rogue.playbackBetweenTurns = true;
		nextBrogueEvent(&theEvent, true, false);
		rogue.RNG = RNG_SUBSTANTIVE;
		
		executeEvent(&theEvent);
	}
	
	freeEverything();
}

void executeEvent(rogueEvent *theEvent) {
	rogue.playbackBetweenTurns = false;
	if (theEvent->eventType == KEYSTROKE) {
		executeKeystroke(theEvent->param1, theEvent->controlKey, theEvent->shiftKey);
	} else if (theEvent->eventType == MOUSE_UP) {
		executeMouseClick(theEvent);
	}
}

boolean fileExists(char *pathname) {
	FILE *openedFile;
	openedFile = fopen(pathname, "rb");
	if (openedFile) {
		fclose(openedFile);
		return true;
	} else {
		return false;
	}
}

// Player specifies a file; if all goes well, that's the new currentFilePath and return true.
// Otherwise, return false and leave currentFilePath untouched.
boolean openFile(char *prompt, char *defaultName, char *suffix) {
	boolean retval;
	char newFilePath[FILENAME_MAX];
	char annotationFilePath[FILENAME_MAX];
	
	retval = false;
	
	if (getInputTextString(newFilePath, prompt, min(DCOLS-25, FILENAME_MAX - strlen(suffix)),
						   defaultName, suffix, TEXT_INPUT_NORMAL)
		&& newFilePath[0] != '\0') {
		
		annotationPathname[0] = '\0';
		strcpy(annotationFilePath, newFilePath);
		if (strlen(annotationFilePath) + strlen(ANNOTATION_SUFFIX) < FILENAME_MAX) {
			strcat(annotationFilePath, ANNOTATION_SUFFIX);
			if (fileExists(annotationFilePath)) {
				strcpy(annotationPathname, annotationFilePath);
			}
		}
		strcat(newFilePath, suffix);
		if (fileExists(newFilePath)) {
			strcpy(currentFilePath, newFilePath);
			retval = true;
		} else {
			confirmMessages();
			message("File not found.", true, false);
		}
	} else {
		confirmMessages();
	}
	return retval;
}

void welcome() {
	message("Hello and welcome, adventurer, to the Dungeons of Doom!", true, false);
	messageWithColor("Press <?> for help at any time.", &flavorTextColor, false);
	message("The doors to the dungeon slam shut behind you.", false, false);
}

void initializeRogue() {
	short i, j, ll[3];
	item *theItem;
	uchar k;
	boolean playingback, playbackFF;
	
	// generate libtcod font bitmap
	// add any new unicode characters here to include them
	/*
	uchar c[16] = {
		FLOOR_CHAR,
		CHASM_CHAR,
		TRAP_CHAR,
		FIRE_CHAR,
		FOLIAGE_CHAR,
		AMULET_CHAR,
		SCROLL_CHAR,
		RING_CHAR,
		WEAPON_CHAR,
		GEM_CHAR,
		TOTEM_CHAR,
		TURRET_CHAR,
		BAD_MAGIC_CHAR,
		GOOD_MAGIC_CHAR,
		' ',
		' ',
	};
	i = j = 0;
	for (k=0; k<256; k++) {
		i = k % 16;
		j = k / 16;
		if (j >= ROWS) {
			break;
		}
		if (j == 8) {
			plotCharWithColor(c[i], i+1, j+1, white, black);
		} else {
			plotCharWithColor(k, i+1, j+1, white, black);
		}
	}
	waitForAcknowledgment();
	*/
	
	playingback = rogue.playbackMode; // the only two animals that need to go on the ark
	playbackFF = rogue.playbackFastForward;
	memset((void *) &rogue, 0, sizeof( playerCharacter )); // the flood
	rogue.playbackMode = playingback;
	rogue.playbackFastForward = playbackFF;
	
	rogue.RNG = RNG_SUBSTANTIVE;
	if (!rogue.playbackMode) {
		rogue.seed = seedRandomGenerator(DUNGEON_SEED);
	}
	initRecording();
	
	//	levels[0].upStairsLoc[0] = rand_range(1, DCOLS - 2);
	//	levels[0].upStairsLoc[1] = rand_range(1, DROWS - 2);
	
	levels[0].upStairsLoc[0] = (DCOLS - 1) / 2;
	levels[0].upStairsLoc[1] = DROWS - 2;
	
	// reset enchant and gain strength frequencies
	rogue.strengthPotionFrequency = 40;
	rogue.enchantScrollFrequency = 60;
	
	// all DF messages are eligible for display
	resetDFMessageEligibility();
	
	// initialize the levels list
	for (i=0; i<101; i++) {
		levels[i].levelSeed = (unsigned long) rand_range(0, 9999) + 10000 * rand_range(0, 9999);
		levels[i].monsters = NULL;
		levels[i].dormantMonsters = NULL;
		levels[i].items = NULL;
		levels[i].roomStorage = NULL;
		levels[i].visited = false;
		levels[i].numberOfRooms = 0;
		levels[i].playerExitedVia[0] = 0;
		levels[i].playerExitedVia[1] = 0;
		do {
			levels[i].downStairsLoc[0] = rand_range(1, DCOLS - 2);
			levels[i].downStairsLoc[1] = rand_range(1, DROWS - 2);
		} while (distanceBetween(levels[i].upStairsLoc[0], levels[i].upStairsLoc[1],
								 levels[i].downStairsLoc[0], levels[i].downStairsLoc[1]) < DCOLS / 3);
		if (i < 100) {
			levels[i+1].upStairsLoc[0] = levels[i].downStairsLoc[0];
			levels[i+1].upStairsLoc[1] = levels[i].downStairsLoc[1];
		}
	}
	
	// decide which are the three lucky levels
	ll[0] = rand_range(5, 13);
	ll[1] = rand_range(5, 12);
	if (ll[1] >= ll[0]) {
		ll[1]++;
	}
	ll[2] = rand_range(5, 11);
	if (ll[2] >= ll[0]) {
		ll[2]++;
	}
	if (ll[2] >= ll[1]) {
		ll[2]++;
	}
	
	rogue.luckyLevels[2] = max(ll[0], max(ll[1], ll[2]));
	rogue.luckyLevels[0] = min(ll[0], min(ll[1], ll[2]));
	for (i=0; i<3; i++) {
		if (ll[i] != rogue.luckyLevels[0] && ll[i] != rogue.luckyLevels[2]) {
			rogue.luckyLevels[1] = ll[i];
			break;
		}
	}
	
	rogue.rewardRoomsGenerated = 0;
	
	rogue.gameHasEnded = false;
	rogue.highScoreSaved = false;
	rogue.cautiousMode = false;
	rogue.milliseconds = 0;
	brogueCursorX = brogueCursorY = 0;
	
	// clear screen and display buffer
	for (i=0; i<COLS; i++) {
		for (j=0; j<ROWS; j++) {
			displayBuffer[i][j].character = 0;
			displayBuffer[i][j].needsUpdate = false;
			displayBuffer[i][j].opacity = 100;
			for (k=0; k<3; k++) {
				displayBuffer[i][j].foreColorComponents[k] = 0;
				displayBuffer[i][j].backColorComponents[k] = 0;
			}
			plotCharWithColor(' ', i, j, black, black);
		}
	}
	
	// pre-shuffle the random terrain colors
	assureCosmeticRNG;
	for (i=0; i<DCOLS; i++) {
		for( j=0; j<DROWS; j++ ) {
			for (k=0; k<8; k++) {
				terrainRandomValues[i][j][k] = rand_range(0, 1000);
			}
		}
	}
	restoreRNG;
	
#ifndef LIBTCOD
#ifdef USE_UNICODE
	if (!rogue.playbackMode) {
		// preload funky characters so the game doesn't pause the first time they are displayed
		plotCharWithColor(FLOOR_CHAR, 0, 0, black, black);
		plotCharWithColor(CHASM_CHAR, 0, 1, black, black);
		plotCharWithColor(TRAP_CHAR, 0, 2, black, black);
		plotCharWithColor(FIRE_CHAR, 0, 3, black, black);
		plotCharWithColor(AMULET_CHAR, 0, 4, black, black);
		plotCharWithColor(RING_CHAR, 0, 5, black, black);
		plotCharWithColor(WEAPON_CHAR, 0, 6, black, black);
		plotCharWithColor(TURRET_CHAR, 0, 7, black, black);
		plotCharWithColor(TOTEM_CHAR, 0, 8, black, black);
		pauseBrogue(1);
	}
#endif
#endif
	
	for (i=0; i<NUMBER_MONSTER_KINDS; i++) {
		monsterCatalog[i].monsterID = i;
	}
	
	shuffleFlavors();
	
	deleteMessages();
	
	// seed the stacks
	floorItems = (item *) malloc(sizeof(item));
	memset(floorItems, '\0', sizeof(item));
	floorItems->nextItem = NULL;
	packItems = (item *) malloc(sizeof(item));
	memset(packItems, '\0', sizeof(item));
	packItems->nextItem = NULL;
	monsters = (creature *) malloc(sizeof(creature));
	memset(monsters, '\0', sizeof(creature));
	dormantMonsters = (creature *) malloc(sizeof(creature));
	memset(dormantMonsters, '\0', sizeof(creature));
	monsters->nextCreature = NULL;
	graveyard = (creature *) malloc(sizeof(creature));
	memset(graveyard, '\0', sizeof(creature));
	graveyard->nextCreature = NULL;
	rooms = (room *) malloc(sizeof(room));
	memset(rooms, '\0', sizeof(room));
	rooms->nextRoom = NULL;
	
	scentMap		= allocDynamicGrid();
	safetyMap		= allocDynamicGrid();
	allySafetyMap	= allocDynamicGrid();
	chokeMap		= allocDynamicGrid();
	
	rogue.mapToSafeTerrain = allocDynamicGrid();
	
	// Zero out the dynamic grids, as an essential safeguard against OOSes:
	fillDynamicGrid(scentMap, 0);
	fillDynamicGrid(safetyMap, 0);
	fillDynamicGrid(allySafetyMap, 0);
	fillDynamicGrid(chokeMap, 0);
	fillDynamicGrid(rogue.mapToSafeTerrain, 0);
	
	// initialize the player
	
	memset(&player, '\0', sizeof(creature));
	player.info = monsterCatalog[0];
	player.movementSpeed = player.info.movementSpeed;
	player.attackSpeed = player.info.attackSpeed;
	clearStatus(&player);
	player.carriedItem = NULL;
	player.status.nutrition = player.maxStatus.nutrition = STOMACH_SIZE;
	player.currentHP = player.info.maxHP;
	player.creatureState = MONSTER_ALLY;
	player.ticksUntilTurn = 0;
	
	rogue.depthLevel = 1;
	rogue.scentTurnNumber = 1000;
	rogue.turnNumber = 0;
	rogue.howManyDepthChanges = 0;
	rogue.foodSpawned = 0;
	rogue.gold = 0;
	rogue.disturbed = false;
	rogue.autoPlayingLevel = false;
	rogue.automationActive = false;
	rogue.justRested = false;
	rogue.easyMode = false;
	rogue.inWater = false;
	rogue.creaturesWillFlashThisTurn = false;
	rogue.updatedSafetyMapThisTurn = false;
	rogue.updatedAllySafetyMapThisTurn = false;
	rogue.updatedMapToSafeTerrainThisTurn = false;
	rogue.experienceLevel = 1;
	rogue.experience = 0;
	rogue.strength = 12;
	rogue.weapon = NULL;
	rogue.armor = NULL;
	rogue.ringLeft = NULL;
	rogue.ringRight = NULL;
	rogue.monsterSpawnFuse = rand_range(125, 175);
	rogue.ticksTillUpdateEnvironment = 100;
	rogue.mapToShore = NULL;
	rogue.goodItemsGenerated = 0;
	rogue.lastTravelLoc[0] = rogue.lastTravelLoc[1] = 0;
	rogue.xpxpThisTurn = 0;
	
	rogue.minersLight = lightCatalog[MINERS_LIGHT];
	
	rogue.clairvoyance = rogue.aggravating = rogue.regenerationBonus
	= rogue.stealthBonus = rogue.transference = rogue.wisdomBonus = 0;
	rogue.lightMultiplier = 1;
	
	theItem = generateItem(FOOD, RATION);
	theItem = addItemToPack(theItem);
	
	theItem = generateItem(WEAPON, DAGGER);
	theItem->enchant1 = theItem->enchant2 = 0;
	theItem->flags &= ~(ITEM_CURSED | ITEM_RUNIC);
	identify(theItem);
	theItem = addItemToPack(theItem);
	equipItem(theItem, false);
	
	theItem = generateItem(WEAPON, DART);
	theItem->enchant1 = theItem->enchant2 = 0;
	theItem->quantity = 15;
	theItem->flags &= ~(ITEM_CURSED | ITEM_RUNIC);
	identify(theItem);
	theItem = addItemToPack(theItem);
	
	theItem = generateItem(ARMOR, LEATHER_ARMOR);
	theItem->enchant1 = 0;
	theItem->flags &= ~(ITEM_CURSED | ITEM_RUNIC);
	identify(theItem);
	theItem = addItemToPack(theItem);
	equipItem(theItem, false);
	
	DEBUG {
		theItem = generateItem(RING, RING_CLAIRVOYANCE);
		theItem->enchant1 = max(DROWS, DCOLS);
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_FIRE);
		theItem->enchant1 = 10;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_LIGHTNING);
		theItem->enchant1 = 10;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_BLINKING);
		theItem->enchant1 = theItem->charges = 10;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_TUNNELING);
		theItem->enchant1 = 10;
		theItem->charges = 3000;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_OBSTRUCTION);
		theItem->enchant1 = 10;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(WAND, WAND_BECKONING);
		theItem->charges = 3000;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_DISCORD);
		theItem->enchant1 = theItem->charges = 10;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_ENTRANCEMENT);
		theItem->enchant1 = 10;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_POISON);
		theItem->enchant1 = 10;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_CONJURATION);
		theItem->enchant1 = 7;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(STAFF, STAFF_HEALING);
		theItem->enchant1 = 10;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(WAND, WAND_DOMINATION);
		theItem->enchant1 = 300;
		theItem->charges = 300;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(WEAPON, DAGGER);
		theItem->enchant1 = 100;
		theItem->enchant2 = W_SLAYING;
		theItem->vorpalEnemy = MK_REVENANT;
		theItem->charges = 10000;
		theItem->flags &= ~(ITEM_CURSED);
		theItem->flags |= (ITEM_PROTECTED | ITEM_RUNIC | ITEM_RUNIC_HINTED);
		theItem->damage.lowerBound = theItem->damage.upperBound = 3;
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(ARMOR, SCALE_MAIL);
		theItem->enchant1 = 30;
		theItem->enchant2 = A_ABSORPTION;
		theItem->flags &= ~(ITEM_CURSED | ITEM_RUNIC_HINTED);
		theItem->flags |= (ITEM_PROTECTED | ITEM_RUNIC);
		theItem->charges = 300;
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(RING, RING_AWARENESS);
		theItem->enchant1 = 30;
		theItem->flags &= ~ITEM_CURSED;
		identify(theItem);
		theItem = addItemToPack(theItem);
		
		theItem = generateItem(POTION, POTION_INCINERATION);
		theItem->quantity = 5;
		identify(theItem);
		theItem = addItemToPack(theItem);
	}
	
	welcome();
}

// call this once per level to set all the dynamic colors as a function of depth
void updateColors() {
	short i;
	
	for (i=0; i<NUMBER_DYNAMIC_COLORS; i++) {
		*(dynamicColors[i][0]) = *(dynamicColors[i][1]);
		applyColorAverage(dynamicColors[i][0], dynamicColors[i][2], min(100, max(0, rogue.depthLevel * 100 / 26)));
	}
}

void startLevel(short oldLevelNumber, short stairDirection) {
	unsigned long oldSeed;
	item *theItem;
	short loc[2], i, j, x, y, px, py, flying;
	boolean isAlreadyAmulet = false;
	creature *monst;
	enum dungeonLayers layer;
	unsigned long timeAway;
	short **mapToStairs;
	short **mapToPit;
	boolean connectingStairsDiscovered;
	
	rogue.lastTravelLoc[0] = rogue.lastTravelLoc[1] = 0;
	
	rogue.howManyDepthChanges++;
	
	if (stairDirection == 0) { // fallen
		connectingStairsDiscovered = (pmap[rogue.downLoc[0]][rogue.downLoc[1]].flags & (DISCOVERED | MAGIC_MAPPED) ? true : false);
		
		levels[oldLevelNumber-1].playerExitedVia[0] = player.xLoc;
		levels[oldLevelNumber-1].playerExitedVia[1] = player.yLoc;
	}
	
	if (oldLevelNumber != rogue.depthLevel) {
		px = player.xLoc;
		py = player.yLoc;
		if (cellHasTerrainFlag(player.xLoc, player.yLoc, T_AUTO_DESCENT)) {
			for (i=0; i<8; i++) {
				if (!cellHasTerrainFlag(player.xLoc+nbDirs[i][0], player.yLoc+nbDirs[i][1], (T_PATHING_BLOCKER))) {
					px = player.xLoc+nbDirs[i][0];
					py = player.yLoc+nbDirs[i][1];
					break;
				}
			}
		}
		mapToStairs = allocDynamicGrid();
		fillDynamicGrid(mapToStairs, 0);
		for (flying = 0; flying <= 1; flying++) {
			fillDynamicGrid(mapToStairs, 0);
			calculateDistances(mapToStairs, px, py, (flying ? T_OBSTRUCTS_PASSABILITY : T_PATHING_BLOCKER), NULL, true);
			for (monst = monsters->nextCreature; monst != NULL; monst = monst->nextCreature) {
				x = monst->xLoc;
				y = monst->yLoc;
				if (((monst->creatureState == MONSTER_TRACKING_SCENT && (stairDirection != 0 || monst->status.levitating))
					 || monst->creatureState == MONSTER_ALLY)
					&& (stairDirection != 0 || monst->currentHP > 10 || monst->status.levitating)
					&& ((flying != 0) == ((monst->status.levitating != 0)
										  || cellHasTerrainFlag(x, y, T_PATHING_BLOCKER)
										  || cellHasTerrainFlag(px, py, T_AUTO_DESCENT)))
					&& !(monst->bookkeepingFlags & MONST_CAPTIVE)
					&& !(monst->info.flags & MONST_WILL_NOT_USE_STAIRS)
					&& !(cellHasTerrainFlag(x, y, T_OBSTRUCTS_PASSABILITY))
					&& !monst->status.entranced
					&& !monst->status.paralyzed
					&& mapToStairs[monst->xLoc][monst->yLoc] < 30000) {
					monst->status.entersLevelIn = max(1, mapToStairs[monst->xLoc][monst->yLoc] * monst->movementSpeed / 100);
					switch (stairDirection) {
						case 1:
							monst->bookkeepingFlags |= MONST_APPROACHING_DOWNSTAIRS;
							break;
						case -1:
							monst->bookkeepingFlags |= MONST_APPROACHING_UPSTAIRS;
							break;
						case 0:
							monst->bookkeepingFlags |= MONST_APPROACHING_PIT;
							break;
						default:
							break;
					}
				}
			}
		}
		freeDynamicGrid(mapToStairs);
	}
	
	// save or clear old level rooms and lights
	for (monst = monsters->nextCreature; monst != NULL; monst = monst->nextCreature) {
		if (monst->mapToMe) {
			freeDynamicGrid(monst->mapToMe);
			monst->mapToMe = NULL;
		}
		if (monst->safetyMap) {
			freeDynamicGrid(monst->safetyMap);
			monst->safetyMap = NULL;
		}
	}
	levels[oldLevelNumber-1].monsters = monsters->nextCreature;
	levels[oldLevelNumber-1].dormantMonsters = dormantMonsters->nextCreature;
	levels[oldLevelNumber-1].items = floorItems->nextItem;
	
	levels[oldLevelNumber - 1].numberOfRooms = numberOfRooms;
	levels[oldLevelNumber - 1].roomStorage = rooms->nextRoom;
	rooms->nextRoom = NULL;
	
	for (i=0; i<DCOLS; i++) {
		for (j=0; j<DROWS; j++) {
			for (layer = 0; layer < NUMBER_TERRAIN_LAYERS; layer++) {
				levels[oldLevelNumber - 1].mapStorage[i][j].layers[layer] = pmap[i][j].layers[layer];
			}
			levels[oldLevelNumber - 1].mapStorage[i][j].volume = pmap[i][j].volume;
			levels[oldLevelNumber - 1].mapStorage[i][j].flags = (pmap[i][j].flags & PERMANENT_TILE_FLAGS);
			levels[oldLevelNumber - 1].mapStorage[i][j].rememberedAppearance = pmap[i][j].rememberedAppearance;
			levels[oldLevelNumber - 1].mapStorage[i][j].rememberedTerrain = pmap[i][j].rememberedTerrain;
			levels[oldLevelNumber - 1].mapStorage[i][j].rememberedItemCategory = pmap[i][j].rememberedItemCategory;
			levels[oldLevelNumber - 1].mapStorage[i][j].machineNumber = pmap[i][j].machineNumber;
		}
	}
	
	levels[oldLevelNumber - 1].awaySince = rogue.turnNumber;
	
	//	Prepare the new level
	
	rogue.minersLightRadius = 2.25 + (DCOLS - 1) * (float) pow(0.85, rogue.depthLevel);
	updateColors();
	updateRingBonuses(); // also updates miner's light
	
	if (!levels[rogue.depthLevel - 1].visited) { // level has not already been visited
		// generate new level
		oldSeed = (unsigned long) rand_range(0, 9999) + 10000 * rand_range(0, 9999);
		seedRandomGenerator(levels[rogue.depthLevel - 1].levelSeed);
		thisLevelProfile = &(levelProfileCatalog[rand_range(0, NUMBER_LEVEL_PROFILES - 1)]);
		
		monsters->nextCreature			= levels[rogue.depthLevel-1].monsters;
		dormantMonsters->nextCreature	= levels[rogue.depthLevel-1].dormantMonsters;
		floorItems->nextItem			= levels[rogue.depthLevel-1].items;
		
		digDungeon();
		setUpWaypoints();
		initializeLevel(stairDirection);
		
		shuffleTerrainColors(100, false);
		
		if (rogue.depthLevel >= AMULET_LEVEL && !numberOfMatchingPackItems(AMULET, 0, 0, false)
			&& levels[rogue.depthLevel-1].visited == false) {
			for (theItem = floorItems->nextItem; theItem != NULL; theItem = theItem->nextItem) {
				if (theItem->category & AMULET) {
					isAlreadyAmulet = true;
					break;
				}
			}
			if (!isAlreadyAmulet) {
				placeItem(generateItem(AMULET, 0), 0, 0);
			}
		}
		seedRandomGenerator(oldSeed);
		
		//logLevel();
		
		for (i = 0; i < 50; i++) {
			updateEnvironment();
		}
		
	} else { // level has already been visited
		
		// restore level
		timeAway = rogue.turnNumber - levels[rogue.depthLevel - 1].awaySince;
		numberOfRooms = levels[rogue.depthLevel - 1].numberOfRooms;
		rooms->nextRoom = levels[rogue.depthLevel - 1].roomStorage;
		
		for (i=0; i<DCOLS; i++) {
			for (j=0; j<DROWS; j++) {
				for (layer = 0; layer < NUMBER_TERRAIN_LAYERS; layer++) {
					pmap[i][j].layers[layer] = levels[rogue.depthLevel - 1].mapStorage[i][j].layers[layer];
				}
				pmap[i][j].volume = levels[rogue.depthLevel - 1].mapStorage[i][j].volume;
				pmap[i][j].flags = (levels[rogue.depthLevel - 1].mapStorage[i][j].flags & PERMANENT_TILE_FLAGS);
				pmap[i][j].rememberedAppearance = levels[rogue.depthLevel - 1].mapStorage[i][j].rememberedAppearance;
				pmap[i][j].rememberedTerrain = levels[rogue.depthLevel - 1].mapStorage[i][j].rememberedTerrain;
				pmap[i][j].rememberedItemCategory = levels[rogue.depthLevel - 1].mapStorage[i][j].rememberedItemCategory;
				pmap[i][j].machineNumber = levels[rogue.depthLevel - 1].mapStorage[i][j].machineNumber;
			}
		}
		
		setUpWaypoints();
		
		rogue.downLoc[0] = levels[rogue.depthLevel - 1].downStairsLoc[0];
		rogue.downLoc[1] = levels[rogue.depthLevel - 1].downStairsLoc[1];
		rogue.upLoc[0] = levels[rogue.depthLevel - 1].upStairsLoc[0];
		rogue.upLoc[1] = levels[rogue.depthLevel - 1].upStairsLoc[1];
		
		if (stairDirection == 1) { // heading downward
			player.xLoc = rogue.upLoc[0];
			player.yLoc = rogue.upLoc[1];
			pmap[player.xLoc][player.yLoc].flags |= HAS_PLAYER;
		} else if (stairDirection == -1) { // heading upward
			player.xLoc = rogue.downLoc[0];
			player.yLoc = rogue.downLoc[1];
			pmap[player.xLoc][player.yLoc].flags |= HAS_PLAYER;
		}
		
		monsters->nextCreature = levels[rogue.depthLevel - 1].monsters;
		dormantMonsters->nextCreature = levels[rogue.depthLevel - 1].dormantMonsters;
		floorItems->nextItem = levels[rogue.depthLevel - 1].items;
		
		if (numberOfMatchingPackItems(AMULET, 0, 0, false)) {
			isAlreadyAmulet = true;
		}
		
		for (theItem = floorItems->nextItem; theItem != NULL; theItem = theItem->nextItem) {
			if (theItem->category & AMULET) {
				if (isAlreadyAmulet) {
					
					pmap[theItem->xLoc][theItem->yLoc].flags &= ~(HAS_ITEM | ITEM_DETECTED);
					
					removeItemFromChain(theItem, floorItems);
					deleteItem(theItem);
					
					continue;
				}
				isAlreadyAmulet = true;
			}
			restoreItem(theItem);
		}
		
		mapToStairs = allocDynamicGrid();
		mapToPit = allocDynamicGrid();
		fillDynamicGrid(mapToStairs, 0);
		fillDynamicGrid(mapToPit, 0);
		calculateDistances(mapToStairs, player.xLoc, player.yLoc, T_PATHING_BLOCKER, NULL, true);
		calculateDistances(mapToPit, levels[rogue.depthLevel-1].playerExitedVia[0],
						   levels[rogue.depthLevel-1].playerExitedVia[0], T_PATHING_BLOCKER, NULL, true);
		for (monst = monsters->nextCreature; monst != NULL; monst = monst->nextCreature) {
			restoreMonster(monst, mapToStairs, mapToPit);
		}
		freeDynamicGrid(mapToStairs);
		freeDynamicGrid(mapToPit);
		
		for (i = 0; i < timeAway && i < 100; i++) {
			updateEnvironment();
		}
	}
	
	levels[rogue.depthLevel-1].visited = true;
	
	if (stairDirection == 0) { // fell into the level
		
		getQualifyingLocNear(loc, player.xLoc, player.yLoc, true, 0,
							 (T_OBSTRUCTS_PASSABILITY | T_LAVA_INSTA_DEATH | T_AUTO_DESCENT | T_IS_DF_TRAP),
							 (HAS_MONSTER | HAS_ITEM | HAS_UP_STAIRS | HAS_DOWN_STAIRS | IS_IN_MACHINE), false);
		player.xLoc = loc[0];
		player.yLoc = loc[1];
		pmap[player.xLoc][player.yLoc].flags |= HAS_PLAYER;
		
		if (connectingStairsDiscovered) {
			pmap[rogue.upLoc[0]][rogue.upLoc[1]].flags |= DISCOVERED;
			rogue.xpxpThisTurn++;
		}
		if (cellHasTerrainFlag(player.xLoc, player.yLoc, T_IS_DEEP_WATER) && !player.status.levitating
			&& !cellHasTerrainFlag(player.xLoc, player.yLoc, T_ENTANGLES)
			&& !cellHasTerrainFlag(player.xLoc, player.yLoc, T_OBSTRUCTS_PASSABILITY)) {
			rogue.inWater = true;
		}
	}
	
	updateMapToShore();
	
	updateVision(true);
	
	// update monster states so none are hunting if they can't see the player
	for (monst=monsters->nextCreature; monst != NULL; monst = monst->nextCreature) {
		updateMonsterState(monst);
	}
	
	rogue.playbackBetweenTurns = true;
	displayLevel();
	refreshSideBar(NULL);
	
	if (rogue.turnNumber) {
		rogue.turnNumber++;
	}
	RNGCheck();
	flushBufferToFile();
}

void freeCreature(creature *monst) {
	if (monst->mapToMe) {
		freeDynamicGrid(monst->mapToMe);
	}
	if (monst->safetyMap) {
		freeDynamicGrid(monst->safetyMap);
	}
	if (monst->carriedItem) {
		free(monst->carriedItem);
	}
	if (monst->carriedMonster) {
		freeCreature(monst->carriedMonster);
	}
	free(monst);
}

void emptyGraveyard() {
	creature *monst, *monst2;
	for (monst = graveyard->nextCreature; monst != NULL; monst = monst2) {
		monst2 = monst->nextCreature;
		freeCreature(monst);
	}
	graveyard->nextCreature = NULL;
}

void freeEverything() {
	short i;
	creature *monst, *monst2;
	item *theItem, *theItem2;
	
#ifdef AUDIT_RNG
	fclose(RNGLogFile);
#endif
	
	freeDynamicGrid(scentMap);
	freeDynamicGrid(safetyMap);
	freeDynamicGrid(allySafetyMap);
	freeDynamicGrid(chokeMap);
	if (rogue.mapToShore) {
		freeDynamicGrid(rogue.mapToShore);
		rogue.mapToShore = NULL;
	}
	if (rogue.mapToSafeTerrain) {
		freeDynamicGrid(rogue.mapToSafeTerrain);
		rogue.mapToSafeTerrain = NULL;
	}
	
	for (i=0; i<101; i++) {
		for (monst = levels[1].monsters; monst != NULL; monst = monst2) {
			monst2 = monst->nextCreature;
			freeCreature(monst);
		}
		levels[1].monsters = NULL;
		for (monst = levels[1].dormantMonsters; monst != NULL; monst = monst2) {
			monst2 = monst->nextCreature;
			freeCreature(monst);
		}
		levels[1].dormantMonsters = NULL;
		for (theItem = levels[1].items; theItem != NULL; theItem = theItem2) {
			theItem2 = theItem->nextItem;
			free(theItem);
		}
		levels[1].items = NULL;
	}
}

void gameOver(char *killedBy, boolean useCustomPhrasing) {
	//short i, j;
	short highScoreIndex;
	char buf[COLS];
	rogueHighScoresEntry theEntry;
	cellDisplayBuffer dbuf[COLS][ROWS];
	boolean playback;
	
	rogue.autoPlayingLevel = false;
	
	flushBufferToFile();
	
	playback = rogue.playbackMode;
	rogue.playbackMode = false;
	messageWithColor("You die...", &badCombatMessageColor, true);
	rogue.playbackMode = playback;
	
	if (D_IMMORTAL) {
		message("...but then you get better.", true, false);
		player.currentHP = player.info.maxHP;
		if (player.status.nutrition < 10) {
			player.status.nutrition = STOMACH_SIZE;
		}
		player.bookkeepingFlags &= ~MONST_IS_DYING;
		return;
	}
	
	if (rogue.highScoreSaved) {
		return;
	}
	rogue.highScoreSaved = true;
	
	copyDisplayBuffer(dbuf, displayBuffer);
	funkyFade(dbuf, &black, 0, 30, mapToWindowX(player.xLoc), mapToWindowY(player.yLoc), false);
	
	if (useCustomPhrasing) {
		sprintf(buf, "%s on level %i%s.", killedBy, rogue.depthLevel,
				(numberOfMatchingPackItems(AMULET, 0, 0, false) > 0 ? ", amulet in hand" : ""));
	} else {
		sprintf(buf, "Killed by a%s %s on level %i%s.", (isVowel(killedBy[0]) ? "n" : ""), killedBy,
				rogue.depthLevel, (numberOfMatchingPackItems(AMULET, 0, 0, false) > 0 ? ", amulet in hand" : ""));
	}
	
	theEntry.score = rogue.gold;
	if (rogue.easyMode) {
		theEntry.score /= 10;
	}
	strcpy(theEntry.description, buf);
	
	printString(buf, ((COLS - strLenWithoutEscapes(buf)) / 2), (ROWS / 2), &gray, &black, 0);
	displayMoreSign();
	
	rogue.playbackMode = playback;
	
	if (rogue.playbackMode) {
		highScoreIndex = false;
	} else {
		highScoreIndex = saveHighScore(theEntry);
	}
	saveRecording();
	printHighScores(highScoreIndex);
	
	commitDraws();
	
	rogue.gameHasEnded = true;
}

void victory() {
	char buf[DCOLS];
	item *theItem;
	short i, gemCount = 0;
	unsigned long totalValue = 0;
	rogueHighScoresEntry theEntry;
	boolean qualified, isPlayback;
	cellDisplayBuffer dbuf[COLS][ROWS];
	
	flushBufferToFile();
	
	deleteMessages();
	message("You are bathed in sunlight as you throw open the heavy doors.", true, false);
	
	copyDisplayBuffer(dbuf, displayBuffer);
	funkyFade(dbuf, &white, 0, 100, mapToWindowX(player.xLoc), mapToWindowY(player.yLoc), false);
	displayMoreSign();
	printString("Congratulations; you have escaped from the Dungeons of Doom!     ", mapToWindowX(0), mapToWindowY(-1), &black, &white, 0);
	displayMoreSign();
	
	clearDisplayBuffer(dbuf);
	
	deleteMessages();
	strcpy(displayedMessage[0], "You sell your treasures and live out your days in fame and glory.");
	
	printString(displayedMessage[0], mapToWindowX(0), mapToWindowY(-1), &white, &black, dbuf);
	
	printString("Gold", mapToWindowX(2), 2, &white, &black, dbuf);
	sprintf(buf, "%li", rogue.gold);
	printString(buf, mapToWindowX(60), 2, &yellow, &black, dbuf);
	totalValue += rogue.gold;
	
	for (i = 3, theItem = packItems->nextItem; theItem != NULL; theItem = theItem->nextItem, i++) {
		if (theItem->category & GEM) {
			gemCount += theItem->quantity;
		}
		identify(theItem);
		itemName(theItem, buf, true, true, &gray);
		upperCase(buf);
		printString(buf, mapToWindowX(2), min(ROWS-1, i + 1), &white, &black, dbuf);
		sprintf(buf, "%li", max(0, itemValue(theItem)));
		printString(buf, mapToWindowX(60), min(ROWS-1, i + 1), &yellow, &black, dbuf);
		totalValue += max(0, itemValue(theItem));
	}
	i++;
	printString("TOTAL:", mapToWindowX(2), min(ROWS-1, i + 1), &teal, &black, dbuf);
	sprintf(buf, "%li", totalValue);
	printString(buf, mapToWindowX(60), min(ROWS-1, i + 1), &teal, &black, dbuf);
	
	funkyFade(dbuf, &white, 0, 15, COLS/2, ROWS/2, true);	
	
	if (gemCount == 0) {
		strcpy(theEntry.description, "Escaped the Dungeons of Doom!");
	} else if (gemCount == 1) {
		strcpy(theEntry.description, "Escaped the Dungeons of Doom with a lumenstone!");
	} else {
		sprintf(theEntry.description, "Escaped the Dungeons of Doom with %i lumenstones!", gemCount);
	}
	
	theEntry.score = totalValue;
	
	if (rogue.easyMode) {
		theEntry.score /= 10;
	}
	
	if (!DEBUGGING && !rogue.playbackMode) {
		qualified = saveHighScore(theEntry);
	} else {
		qualified = false;
	}
	
	isPlayback = rogue.playbackMode;
	rogue.playbackMode = false;
	displayMoreSign();
	rogue.playbackMode = isPlayback;
	
	saveRecording();
	
	printHighScores(qualified);
	
	commitDraws();
	
	rogue.gameHasEnded = true;
}

void enableEasyMode() {
	if (rogue.easyMode) {
		message("Alas, all hope of salvation is lost. You shed scalding tears at your plight.", true, false);
		return;
	}
	message("A dark presence surrounds you, whispering promises of stolen power.", true, false);
	if (confirm("Succumb to demonic temptation? (y/n)", false)) {
		recordKeystroke(EASY_MODE_KEY, false, true);
		message("An ancient and terrible evil burrows into your willing flesh!", true, true);
		player.info.displayChar = '&';
		rogue.easyMode = true;
		refreshDungeonCell(player.xLoc, player.yLoc);
		refreshSideBar(NULL);
		message("Wracked by spasms, your body contorts into an ALL-POWERFUL AMPERSAND!!!", true, false);
		message("You have a feeling that you will take 20% as much damage from now on.", true, false);
		message("But great power comes at a great price -- specifically, a 90% income tax rate.", true, false);
	} else {
		message("The evil dissipates, hissing, from the air around you.", true, false);
	}
}

// takes a flag of the form (1 << n) and returns n
short unflag(unsigned long flag) {
	short i;
	for (i=0; i<32; i++) {
		if (flag >> i == 1) {
			return i;
		}
	}
	return -1;
}
