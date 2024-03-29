/*
 *  Globals.c
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

#include "Rogue.h"


tcell tmap[DCOLS][DROWS];						// grids with info about the map
pcell pmap[DCOLS][DROWS];
short **scentMap;
cellDisplayBuffer displayBuffer[COLS][ROWS];	// used to optimize plotCharWithColor
short terrainRandomValues[DCOLS][DROWS][8];
char buffer[DCOLS][DROWS];						// used in cave generation
short **safetyMap;								// used to help monsters flee
short **allySafetyMap;							// used to help allies flee
short **chokeMap;								// used to assess the importance of the map's various chokepoints
short listOfWallsX[4][DROWS*DCOLS];
short listOfWallsY[4][DROWS*DCOLS];
short numberOfWalls[4];
const short nbDirs[8][2] = {{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,-1},{1,1}};
short numberOfRooms;
short numberOfWaypoints;
levelData levels[101];
creature player;
playerCharacter rogue;
creature *monsters;
creature *dormantMonsters;
creature *graveyard;
item *floorItems;
item *packItems;
room *rooms;
waypoint waypoints[MAX_WAYPOINTS];
levelProfile *thisLevelProfile;
char displayedMessage[MESSAGE_LINES][COLS*2];
boolean messageConfirmed[3];
char combatText[COLS * 2];
short brogueCursorX, brogueCursorY;
char currentFilePath[FILENAME_MAX];

#ifdef AUDIT_RNG
FILE *RNGLogFile;
#endif

unsigned char inputRecordBuffer[INPUT_RECORD_BUFFER + 10];
unsigned short locationInRecordingBuffer;
unsigned long randomNumbersGenerated;
unsigned long positionInPlaybackFile;
unsigned long lengthOfPlaybackFile;
unsigned long recordingLocation;
unsigned long maxLevelChanges;
char annotationPathname[FILENAME_MAX];	// pathname of annotation file

const long levelPoints[MAX_EXP_LEVEL] = {
	10L,		// level 2
	20L,		// level 3
	40L,		// level 4
	80L,		// level 5
	160L,		// level 6
	320L,		// level 7
	640L,		// level 8
	1300L,		// level 9
	2600L,		// level 10
	5200L,		// level 11
	10000L,		// level 12
	20000L,		// level 13
	40000L,		// level 14
	80000L,		// level 15
	160000L,	// level 16
	320000L,	// level 17
	1000000L,	// level 18
	3333333L,	// level 19
	6666666L,	// level 20
	MAX_EXP,	// level 21
	99900000L	// level 22
};

#pragma mark Colors
//									Red		Green	Blue	RedRand	GreenRand	BlueRand	Rand	Dances?
// basic colors
const color white =					{100,	100,	100,	0,		0,			0,			0,		false};
const color gray =					{50,	50,		50,		0,		0,			0,			0,		false};
const color darkGray =				{30,	30,		30,		0,		0,			0,			0,		false};
const color veryDarkGray =			{15,	15,		15,		0,		0,			0,			0,		false};
const color black =					{0,		0,		0,		0,		0,			0,			0,		false};
const color yellow =				{100,	100,	0,		0,		0,			0,			0,		false};
const color darkYellow =			{50,	50,		0,		0,		0,			0,			0,		false};
const color teal =					{30,	100,	100,	0,		0,			0,			0,		false};
const color purple =				{100,	0,		100,	0,		0,			0,			0,		false};
const color darkPurple =			{50,	0,		50,		0,		0,			0,			0,		false};
const color brown =					{60,	40,		0,		0,		0,			0,			0,		false};
const color green =					{0,		100,	0,		0,		0,			0,			0,		false};
const color darkGreen =				{0,		50,		0,		0,		0,			0,			0,		false};
const color orange =				{100,	50,		0,		0,		0,			0,			0,		false};
const color darkOrange =			{50,	25,		0,		0,		0,			0,			0,		false};
const color blue =					{0,		0,		100,	0,		0,			0,			0,		false};
const color darkBlue =				{0,		0,		50,		0,		0,			0,			0,		false};
const color lightBlue =				{40,	40,		100,	0,		0,			0,			0,		false};
const color pink =					{100,	60,		66,		0,		0,			0,			0,		false};
const color red  =					{100,	0,		0,		0,		0,			0,			0,		false};
const color darkRed =				{50,	0,		0,		0,		0,			0,			0,		false};
const color tanColor =				{80,	67,		15,		0,		0,			0,			0,		false};

// bolt colors
const color rainbow =				{-70,	-70,	-70,	170,	170,		170,		0,		true};
const color discordColor =			{25,	0,		25,		66,		0,			0,			0,		true};
const color poisonColor =			{0,		0,		0,		10,		50,			10,			0,		true};
const color beckonColor =			{10,	10,		10,		5,		5,			5,			50,		true};
const color invulnerabilityColor =	{25,	0,		25,		0,		0,			66,			0,		true};
const color dominationColor =		{0,		0,		100,	80,		25,			0,			0,		true};
const color fireBoltColor =			{500,	150,	0,		45,		30,			0,			0,		true};
const color flamedancerCoronaColor ={500,	150,	100,	45,		30,			0,			0,		true};

// tile colors
const color undiscoveredColor =		{0,		0,		0,		0,		0,			0,			0,		false};

const color wallForeColor =			{7,		7,		7,		3,		3,			3,			0,		false};
color wallBackColor;
const color wallBackColorStart =	{40,	40,		40,		25,		0,			0,			20,		false};
const color wallBackColorEnd =		{40,	30,		35,		0,		20,			30,			20,		false};
const color graniteBackColor =		{10,	10,		10,		0,		0,			0,			0,		false};
const color floorForeColor =		{30,	30,		30,		0,		0,			0,			35,		false};
color floorBackColor;
const color floorBackColorStart =	{2,		2,		10,		2,		2,			0,			0,		false};
const color floorBackColorEnd =		{5,		5,		5,		2,		2,			0,			0,		false};
const color carpetForeColor =		{23,	30,		38,		0,		0,			0,			0,		false};
const color carpetBackColor =		{15,	8,		5,		0,		0,			0,			0,		false};
const color doorForeColor =			{70,	35,		15,		0,		0,			0,			0,		false};
const color doorBackColor =			{30,	10,		5,		0,		0,			0,			0,		false};
const color ironDoorForeColor =		{40,	40,		40,		0,		0,			0,			0,		false};
const color ironDoorBackColor =		{15,	15,		15,		0,		0,			0,			0,		false};
const color bridgeFrontColor =		{50,	18,		18,		18,		7,			2,			0,		false};
const color bridgeBackColor =		{18,	5,		3,		5,		2,			1,			0,		false};

const color deepWaterForeColor =	{5,		5,		40,		0,		0,			10,			10,		true};
color deepWaterBackColor;
const color deepWaterBackColorStart = {5,	5,		55,		5,		5,			10,			10,		true};
const color deepWaterBackColorEnd =	{5,		5,		45,		2,		2,			5,			5,		true};
const color shallowWaterForeColor =	{40,	40,		90,		0,		0,			10,			10,		true};
color shallowWaterBackColor;
const color shallowWaterBackColorStart ={30,30,		80,		0,		0,			10,			10,		true};
const color shallowWaterBackColorEnd ={20,	20,		60,		0,		0,			5,			5,		true};
const color mudForeColor =			{15,	10,		5,		5,		5,			0,			0,		false};
const color mudBackColor =			{30,	20,		10,		5,		5,			0,			0,		false};
const color chasmForeColor =		{7,		7,		15,		4,		4,			8,			0,		false};
color chasmEdgeBackColor;
const color chasmEdgeBackColorStart ={5,	5,		25,		2,		2,			2,			0,		false};
const color chasmEdgeBackColorEnd =	{8,		8,		20,		2,		2,			2,			0,		false};
const color fireForeColor =			{70,	20,		0,		15,		10,			0,			0,		true};
const color lavaForeColor =			{20,	20,		20,		100,	10,			0,			0,		true};
const color brimstoneForeColor =	{100,	50,		10,		0,		50,			40,			0,		true};
const color brimstoneBackColor =	{18,	12,		9,		0,		0,			5,			0,		false};

const color lavaBackColor =			{70,	20,		0,		15,		10,			0,			0,		true};
const color acidBackColor =			{20,	70,		30,		5,		15,			10,			0,		true};

const color lightningColor =		{60,	80,		90,		40,		20,			10,			0,		true};
const color fungusLightColor =		{2,		11,		11,		4,		3,			3,			0,		true};
const color lavaLightColor =		{47,	13,		0,		10,		7,			0,			0,		true};
const color deepWaterLightColor =	{10,	30,		100,	0,		30,			100,		0,		true};

const color grassColor =			{15,	40,		15,		15,		50,			15,			10,		false};
const color fungusColor =			{15,	50,		50,		0,		25,			0,			30,		true};
const color foliageColor =			{25,	100,	25,		15,		0,			15,			0,		false};
const color ashForeColor =			{20,	20,		20,		0,		0,			0,			20,		false};
const color bonesForeColor =		{80,	80,		50,		5,		5,			35,			5,		false};
const color ectoplasmColor =		{45,	20,		55,		25,		0,			25,			5,		false};
const color forceFieldColor =		{0,		25,		25,		0,		25,			25,			0,		true};
const color wallCrystalColor =		{40,	40,		60,		20,		20,			40,			0,		true};
const color lichenColor =			{50,	5,		25,		10,		0,			5,			0,		true};
const color altarForeColor =		{5,		7,		9,		0,		0,			0,			0,		false};
const color altarBackColor =		{35,	18,		18,		0,		0,			0,			0,		false};
const color pedestalBackColor =		{10,	5,		20,		0,		0,			0,			0,		false};

// monster colors
const color goblinColor =			{40,	30,		20,		0,		0,			0,			0,		false};
const color jackalColor =			{60,	42,		27,		0,		0,			0,			0,		false};
const color ogreColor =				{60,	25,		25,		0,		0,			0,			0,		false};
const color eelColor =				{30,	12,		12,		0,		0,			0,			0,		false};
const color goblinConjurerColor =	{67,	10,		100,	0,		0,			0,			0,		false};
const color spectralBladeColor =	{15,	15,		60,		0,		0,			70,			50,		true};
const color spectralImageColor =	{50,	20,		20,		50,		0,			0,			70,		true};
const color toadColor =				{40,	65,		30,		0,		0,			0,			0,		false};
const color trollColor =			{40,	60,		15,		0,		0,			0,			0,		false};
const color centipedeColor =		{75,	25,		85,		0,		0,			0,			0,		false};
const color dragonColor =			{20,	80,		15,		0,		0,			0,			0,		false};
const color krakenColor =			{100,	55,		55,		0,		0,			0,			0,		false};
const color salamanderColor =		{40,	10,		0,		8,		5,			0,			0,		true};
const color pixieColor =			{60,	60,		60,		40,		40,			40,			0,		true};
const color darPriestessColor =		{0,		50,		50,		0,		0,			0,			0,		false};
const color darMageColor =			{50,	50,		0,		0,		0,			0,			0,		false};
const color wraithColor =			{66,	66,		25,		0,		0,			0,			0,		false};
const color pinkJellyColor =		{100,	40,		40,		5,		5,			5,			20,		true};

// light colors
color minersLightColor;
const color minersLightStartColor =	{180,	180,	180,	0,		0,			0,			0,		false};
const color minersLightEndColor =	{90,	90,		120,	0,		0,			0,			0,		false};
const color torchLightColor =		{100,	50,		20,		0,		20,			10,			0,		true};
const color burningLightColor =		{200,	10,		10,		0,		20,			5,			0,		true};
const color wispLightColor =		{75,	100,	250,	33,		10,			0,			0,		true};
const color summonedImageLightColor ={200,	0,		75,		0,		0,			0,			0,		true};
const color spectralBladeLightColor ={40,	0,		230,	0,		0,			0,			0,		true};
const color ectoplasmLightColor =	{23,	10,		28,		13,		0,			13,			3,		false};
const color explosionColor =		{10,	8,		2,		0,		2,			2,			0,		true};
const color dartFlashColor =		{500,	500,	500,	0,		2,			2,			0,		true};
const color lichLightColor =		{-50,	80,		30,		0,		0,			20,			0,		true};
const color forceFieldLightColor =	{10,	10,		10,		0,		50,			50,			0,		true};
const color crystalWallLightColor =	{10,	10,		10,		0,		0,			50,			0,		true};
const color sunLightColor =			{100,	100,	75,		0,		0,			0,			0,		false};
const color fungusForestLightColor ={30,	40,		60,		0,		0,			0,			40,		true};
const color fungusTrampledLightColor ={10,	10,		10,		0,		50,			50,			0,		true};
const color redFlashColor =			{100,	10,		10,		0,		0,			0,			0,		false};
const color darknessPatchColor =	{-10,	-10,	-10,	0,		0,			0,			0,		false};
const color darknessCloudColor =	{-20,	-20,	-20,	0,		0,			0,			0,		false};
const color magicMapFlashColor =	{60,	20,		60,		0,		0,			0,			0,		false};

// color multipliers
const color colorDim25 =			{25,	25,		25,		25,		25,			25,			25,		false};
const color memoryColor =			{25,	25,		50,		20,		20,			20,			0,		false};
const color memoryOverlay =			{25,	25,		50,		0,		0,			0,			0,		false};
const color magicMapColor =			{60,	20,		60,		60,		20,			60,			0,		false};
const color clairvoyanceColor =		{50,	90,		50,		50,		90,			50,			0,		false};
const color omniscienceColor =		{70,	50,		30,		70,		50,			30,			45,		false};

// blood colors
const color humanBloodColor =		{60,	20,		10,		15,		0,			0,			15,		false};
const color insectBloodColor =		{10,	60,		20,		0,		15,			0,			15,		false};
const color vomitColor =			{60,	50,		5,		0,		15,			15,			0,		false};
const color urineColor =			{70,	70,		40,		0,		0,			0,			10,		false};
const color methaneColor =			{45,	60,		15,		0,		0,			0,			0,		false};

// gas colors
const color poisonGasColor =		{75,	25,		85,		0,		0,			0,			0,		false};
const color confusionGasColor =		{50,	50,		50,		40,		40,			40,			0,		true};

// interface colors
const color blueBar =				{15,	10,		50,		0,		0,			0,			0,		false};
const color redBar =				{45,	10,		15,		0,		0,			0,			0,		false};
const color hiliteColor =			{100,	100,	0,		0,		0,			0,			0,		false};
const color itemBoxColor =			{6,		4,		20,		0,		0,			0,			0,		false};

const color playerInLightColor =	{100,	100,	30,		0,		0,			0,			0,		false};
const color playerInShadowColor =	{60,	60,		100,	0,		0,			0,			0,		false};
const color playerInDarknessColor =	{50,	30,		50,		0,		0,			0,			0,		false};

const color goodCombatMessageColor ={60,	50,		100,	0,		0,			0,			0,		false};
const color badCombatMessageColor =	{100,	50,		60,		0,		0,			0,			0,		false};
const color advancementMessageColor ={50,	100,	60,		0,		0,			0,			0,		false};
const color itemMessageColor =		{100,	100,	50,		0,		0,			0,			0,		false};
const color flavorTextColor =		{45,	45,		90,		0,		0,			0,			0,		false};

#pragma mark Dynamic color references

const color *dynamicColors[NUMBER_DYNAMIC_COLORS][3] = {
	// used color			shallow color				deep color
	{&minersLightColor,		&minersLightStartColor,		&minersLightEndColor},
	{&wallBackColor,		&wallBackColorStart,		&wallBackColorEnd},
	{&deepWaterBackColor,	&deepWaterBackColorStart,	&deepWaterBackColorEnd},
	{&shallowWaterBackColor,&shallowWaterBackColorStart,&shallowWaterBackColorEnd},
	{&floorBackColor,		&floorBackColorStart,		&floorBackColorEnd},
	{&chasmEdgeBackColor,	&chasmEdgeBackColorStart,	&chasmEdgeBackColorEnd},
};

#pragma mark Terrain definitions

const floorTileType tileCatalog[NUMBER_TILETYPES] = {
	
	// promoteChance is in hundredths of a percent per turn
	
	//	char		fore color				back color		priority	ignit	fireType	discovType	promoteType		promoteChance	glowLight		flags																								description			flavorText
	
	// dungeon layer (this layer must have all of fore color, back color and char)
	{	' ',		&black,					&black,					100,0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"a chilly void",		""},
	{WALL_CHAR,		&wallBackColor,			&graniteBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE),															"a rough granite wall",	""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"the ground",			""},
	{FLOOR_CHAR,	&carpetForeColor,		&carpetBackColor,		95,	0,	DF_EMBERS,		0,			0,				0,				NO_LIGHT,		(T_VANISHES_UPON_PROMOTION | T_IS_FLAMMABLE),														"the carpet",			"Ornate carpeting fills this room, a relic of ages past."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE),															"a stone wall",			""},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE),															"a stone wall",			""},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE),															"a stone wall",			""},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE),															"a stone wall",			""},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE),															"a stone wall",			""},
	{DOOR_CHAR,		&doorForeColor,			&doorBackColor,			25,	50,	DF_EMBERS,		0,			DF_OPEN_DOOR,	0,				NO_LIGHT,		(T_OBSTRUCTS_VISION | T_OBSTRUCTS_SCENT | T_OBSTRUCTS_GAS | T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION | T_PROMOTES_ON_STEP | T_STAND_IN_TILE), "a wooden door",	"you pass through the doorway."},
	{OPEN_DOOR_CHAR,&doorForeColor,			&doorBackColor,			25,	50,	DF_EMBERS,		0,			DF_CLOSED_DOOR,	10000,			NO_LIGHT,		(T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),										"an open door",			"you pass through the doorway."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	50,	DF_EMBERS,		DF_SHOW_DOOR,0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_IS_SECRET | T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),	"a stone wall",		"you pass through the doorway."},
	{DOOR_CHAR,		&ironDoorForeColor,		&ironDoorBackColor,		15,	50,	DF_EMBERS,		0,			DF_OPEN_IRON_DOOR_INERT,0,		NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_VANISHES_UPON_PROMOTION | T_PROMOTES_WITH_KEY | T_STAND_IN_TILE),		"a locked iron door",	"you search your pack but do not have a matching key."},
	{OPEN_DOOR_CHAR,&ironDoorForeColor,		&ironDoorBackColor,		90,	50,	DF_EMBERS,		0,			0,				0,				NO_LIGHT,		(T_STAND_IN_TILE | T_OBSTRUCTS_SURFACE_EFFECTS),													"an open iron door",	"you pass through the doorway."},
	{DOWN_CHAR,		&yellow,				&floorBackColor,		30,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS),													"a downward staircase",	"stairs spiral downward into the depths."},
	{UP_CHAR,		&yellow,				&floorBackColor,		30,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS),													"an upward staircase",	"stairs spiral upward."},
	{UP_CHAR,		&blue,					&floorBackColor,		30,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS),													"the dungeon exit",		"the gilded doors leading out of the dungeon are sealed by an invisible force."},
	{WALL_CHAR,		&torchLightColor,		&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE),															"a wall-mounted torch",	""},
	{WALL_CHAR,		&wallCrystalColor,		&wallCrystalColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				CRYSTAL_WALL_LIGHT,(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SCENT | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS | T_STAND_IN_TILE),"a crystal formation",""},
	{WALL_CHAR,		&gray,					&floorBackColor,		10,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SCENT | T_STAND_IN_TILE),				"a heavy portcullis",	""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_DOOR_BLOCKED,0,				NO_LIGHT,		(T_VANISHES_UPON_PROMOTION | T_IS_WIRED),															"the ground",			""},
	{WALL_CHAR,		&doorForeColor,			&floorBackColor,		10,	100,DF_WOODEN_BARRICADE_BURN,0,	DF_ADD_WOODEN_BARRICADE,10000,	NO_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SCENT | T_STAND_IN_TILE | T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION),"a dry wooden barricade",""},
	{WALL_CHAR,		&doorForeColor,			&floorBackColor,		10,	100,DF_WOODEN_BARRICADE_BURN,0,	0,				0,				NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SCENT | T_STAND_IN_TILE | T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION),"a dry wooden barricade",""},
	{WALL_CHAR,		&torchLightColor,		&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_PILOT_LIGHT,	0,				TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE | T_IS_WIRED | T_VANISHES_UPON_PROMOTION),				"a wall-mounted torch",	""},
	{FIRE_CHAR,		&fireForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING | T_STAND_IN_TILE | T_IS_FIRE),												"a fallen torch",		""},
	{STATUE_CHAR,	&wallBackColor,			&graniteBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			DF_CRACKING_STATUE,0,			NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SCENT | T_OBSTRUCTS_GAS | T_VANISHES_UPON_PROMOTION | T_IS_WIRED | T_STAND_IN_TILE),"a marble statue",	""},
	{STATUE_CHAR,	&wallBackColor,			&graniteBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			DF_STATUE_SHATTER,3500,			NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SCENT | T_OBSTRUCTS_GAS | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),"a cracking statue",	""},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_TURRET_EMERGE,0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_VANISHES_UPON_PROMOTION | T_IS_WIRED | T_STAND_IN_TILE),				"a stone wall",	""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_DARKENING_FLOOR,	0,			NO_LIGHT,		(T_IS_WIRED | T_VANISHES_UPON_PROMOTION),															"the ground",			""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_DARK_FLOOR,	1500,			NO_LIGHT,		(T_VANISHES_UPON_PROMOTION),																		"the ground",			""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			0,				0,				DARKNESS_CLOUD_LIGHT,0,																								"the ground",			""},

	// traps (part of dungeon layer):
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_POISON_GAS_CLOUD, DF_SHOW_POISON_GAS_TRAP, 0, 0,			NO_LIGHT,		(T_IS_SECRET | T_IS_DF_TRAP),																		"the ground",			""},
	{TRAP_CHAR,		&centipedeColor,		&floorBackColor,		30,	0,	DF_POISON_GAS_CLOUD, 0,		0,				0,				NO_LIGHT,		(T_IS_DF_TRAP),																						"a poison gas trap",	"there is a hidden pressure plate in the floor above a reserve of poisonous gas."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_POISON_GAS_CLOUD, DF_SHOW_TRAPDOOR/*_HALO*/,0,	0,				NO_LIGHT,		(T_IS_SECRET | T_AUTO_DESCENT),																"the ground",			"you plunge through a hidden trap door!"},
	{CHASM_CHAR,	&chasmForeColor,		&black,					30,	0,	DF_POISON_GAS_CLOUD, 0,		0,				0,				NO_LIGHT,		(T_AUTO_DESCENT),																					"a hole",				"you plunge through a hole in the ground!"},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_PARALYSIS_GAS_CLOUD,	DF_SHOW_PARALYSIS_GAS_TRAP, 0, 0,	NO_LIGHT,		(T_IS_SECRET | T_IS_DF_TRAP),																		"the ground",			""},
	{TRAP_CHAR,		&pink,					&floorBackColor,		30,	0,	DF_PARALYSIS_GAS_CLOUD,	0,	0,				0,				NO_LIGHT,		(T_IS_DF_TRAP),																						"a paralysis trap",		"there is a hidden pressure plate in the floor above a reserve of paralytic gas."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_CONFUSION_GAS_TRAP_CLOUD,DF_SHOW_CONFUSION_GAS_TRAP, 0,0,NO_LIGHT,		(T_IS_SECRET | T_IS_DF_TRAP),																		"the ground",			""},
	{TRAP_CHAR,		&confusionGasColor,		&floorBackColor,		30,	0,	DF_CONFUSION_GAS_TRAP_CLOUD,0,	0,			0,				NO_LIGHT,		(T_IS_DF_TRAP),																						"a confusion trap",		"A hidden pressure plate accompanies a reserve of psychotropic gas."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_FLAMETHROWER,	DF_SHOW_FLAMETHROWER_TRAP, 0,	0,		NO_LIGHT,		(T_IS_SECRET | T_IS_DF_TRAP),																		"the ground",			""},
	{TRAP_CHAR,		&fireForeColor,			&floorBackColor,		30,	0,	DF_FLAMETHROWER,	0,		0,				0,				NO_LIGHT,		(T_IS_DF_TRAP),																						"a fire trap",			"A hidden pressure plate is connected to a crude flamethrower mechanism."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_FLOOD,		DF_SHOW_FLOOD_TRAP, 0,		0,				NO_LIGHT,		(T_IS_SECRET | T_IS_DF_TRAP),																		"the ground",			""},
	{TRAP_CHAR,		&shallowWaterForeColor,	&floorBackColor,		58,	0,	DF_FLOOD,		0,			0,				0,				NO_LIGHT,		(T_IS_DF_TRAP),																						"a flood trap",			"A hidden pressure plate is connected to floodgates in the walls and ceiling."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_PLAIN_FIRE,	0,			DF_POISON_GAS_VENT_OPEN,0,		NO_LIGHT,		(T_IS_WIRED | T_VANISHES_UPON_PROMOTION),															"the ground",			""},
	{WEB_CHAR,		&floorForeColor,		&floorBackColor,		30,	0,	DF_PLAIN_FIRE,	0,			DF_VENT_SPEW_POISON_GAS,10000,	NO_LIGHT,		0,																									"a gas vent",			"Clouds of caustic gas are wafting out of a hidden vent in the floor."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_PLAIN_FIRE,	0,			DF_METHANE_VENT_OPEN,0,			NO_LIGHT,		(T_IS_WIRED | T_VANISHES_UPON_PROMOTION),															"the ground",			""},
	{WEB_CHAR,		&floorForeColor,		&floorBackColor,		30,	15,	DF_EMBERS,		0,			DF_VENT_SPEW_METHANE,5000,		NO_LIGHT,		(T_IS_FLAMMABLE),																					"a gas vent",			"Clouds of explosive gas are wafting out of a hidden vent in the floor."},
	
	// liquid layer
	{WATER_CHAR,	&deepWaterForeColor,	&deepWaterBackColor,	40,	100,DF_STEAM_ACCUMULATION,	0,	0,				0,				NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_IS_FLAMMABLE | T_IS_DEEP_WATER | T_OBSTRUCTS_SCENT | T_ALLOWS_SUBMERGING | T_STAND_IN_TILE),"murky waters","the current tugs you in all directions."},
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	55,	0,	DF_STEAM_ACCUMULATION,	0,	0,				0,				NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_ALLOWS_SUBMERGING | T_STAND_IN_TILE),										"shallow water",		"the water is cold and reaches your knees."},
	{WATER_CHAR,	&mudForeColor,			&mudBackColor,			40,	0,	DF_PLAIN_FIRE,	0,			DF_METHANE_GAS_PUFF, 100,		NO_LIGHT,		(T_ALLOWS_SUBMERGING | T_STAND_IN_TILE),															"a bog",				"you are knee-deep in thick, foul-smelling mud."},
	{CHASM_CHAR,	&chasmForeColor,		&black,					40,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_OBSTRUCTS_SCENT | T_AUTO_DESCENT | T_STAND_IN_TILE),						"a chasm",				"you plunge downward into the chasm!"},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"the brink of a chasm",	"chilly winds blow upward from the stygian depths."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_SPREADABLE_COLLAPSE,0,		NO_LIGHT,		(T_IS_WIRED | T_VANISHES_UPON_PROMOTION),															"the ground",			""},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	85,	0,	DF_PLAIN_FIRE,	0,			DF_COLLAPSE_SPREADS,2500,		NO_LIGHT,		0,																									"the crumbling ground",	"cracks are appearing in the ground beneath your feet!"},
	{CHASM_CHAR,	&chasmForeColor,		&black,					40,	0,	DF_PLAIN_FIRE,	0,			DF_BRIDGE_APPEARS,0,			NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_OBSTRUCTS_SCENT | T_AUTO_DESCENT | T_STAND_IN_TILE | T_IS_WIRED | T_VANISHES_UPON_PROMOTION),"a chasm","you plunge downward into the chasm!"},
	{WATER_CHAR,	&fireForeColor,			&lavaBackColor,			40,	0,	DF_OBSIDIAN,	0,			0,				0,				LAVA_LIGHT,		(T_OBSTRUCTS_SCENT | T_LAVA_INSTA_DEATH | T_ALLOWS_SUBMERGING | T_STAND_IN_TILE),					"lava",					"searing heat rises from the lava."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_PLAIN_FIRE,	0,			0,				0,				SUN_LIGHT,		(T_STAND_IN_TILE),																					"a patch of sunlight",	"sunlight streams through cracks in the ceiling."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		100,0,	DF_PLAIN_FIRE,	0,			0,				0,				DARKNESS_PATCH_LIGHT,	(0),																						"",						""},
	{ASH_CHAR,		&brimstoneForeColor,	&brimstoneBackColor,	40, 100,DF_INERT_BRIMSTONE,	0,		DF_INERT_BRIMSTONE,	10,			NO_LIGHT,		(T_IS_FLAMMABLE | T_SPONTANEOUSLY_IGNITES | T_OBSTRUCTS_SCENT),										"hissing brimstone",	"the jagged brimstone hisses and spits ominously as it crunches under your feet."},
	{ASH_CHAR,		&brimstoneForeColor,	&brimstoneBackColor,	40, 0,	DF_INERT_BRIMSTONE,	0,		DF_ACTIVE_BRIMSTONE, 800,		NO_LIGHT,		(T_SPONTANEOUSLY_IGNITES | T_OBSTRUCTS_SCENT),														"hissing brimstone",	"the jagged brimstone hisses and spits ominously as it crunches under your feet."},
	{FLOOR_CHAR,	&darkGray,				0,						55,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"the obsidian ground",	"the ground has fused into obsidian."},
	{BRIDGE_CHAR,	&bridgeFrontColor,		&bridgeBackColor,		20,	50,	DF_BRIDGE_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION),														"a rickety rope bridge","a rickety rope bridge creaks underfoot."},
	{BRIDGE_CHAR,	&bridgeFrontColor,		&bridgeBackColor,		20,	50,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION),														"a rickety rope bridge","a rickety rope bridge is staked to the edge of the chasm."},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	20,	50,	DF_BRIDGE_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"a stone bridge",		"a stone bridge traces a narrow path across the chasm."},
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	60,	0,	DF_STEAM_ACCUMULATION,	0,	DF_SPREADABLE_WATER,0,			NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_ALLOWS_SUBMERGING | T_STAND_IN_TILE | T_IS_WIRED | T_VANISHES_UPON_PROMOTION),	"shallow water",	"the water is cold and reaches your knees."},
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	60,	0,	DF_STEAM_ACCUMULATION,	0,	DF_WATER_SPREADS,2500,			NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_ALLOWS_SUBMERGING | T_STAND_IN_TILE | T_VANISHES_UPON_PROMOTION),			"shallow water",		"the water is cold and reaches your knees."},
		
	// surface layer
	{CHASM_CHAR,	&chasmForeColor,		&black,					15,	0,	DF_PLAIN_FIRE,	0,			DF_HOLE_DRAIN,	-1000,			NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_OBSTRUCTS_SCENT | T_AUTO_DESCENT | T_STAND_IN_TILE | T_VANISHES_UPON_PROMOTION),"a hole",			"you plunge downward into the hole!"},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	53,	0,	DF_PLAIN_FIRE,	0,			0,				-500,			NO_LIGHT,		(T_VANISHES_UPON_PROMOTION),																		"translucent ground",	"chilly gusts of air blow upward through the translucent floor."},
	{WATER_CHAR,	&deepWaterForeColor,	&deepWaterBackColor,	50,	100,DF_STEAM_ACCUMULATION,	0,	DF_FLOOD_DRAIN,	-200,			NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION | T_IS_DEEP_WATER | T_OBSTRUCTS_SCENT | T_ALLOWS_SUBMERGING | T_STAND_IN_TILE), "sloshing water", "roiling water floods the room."},
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	53,	0,	DF_STEAM_ACCUMULATION,	0,	DF_PUDDLE,		-100,			NO_LIGHT,		(T_EXTINGUISHES_FIRE | T_VANISHES_UPON_PROMOTION | T_ALLOWS_SUBMERGING | T_STAND_IN_TILE),			"shallow water",		"knee-deep water drains slowly into holes in the floor."},
	{GRASS_CHAR,	&grassColor,			0,						60,	15,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_STAND_IN_TILE | T_VANISHES_UPON_PROMOTION),										"grass-like fungus",	"grass-like fungus crunches underfoot."},
	{GRASS_CHAR,	&fungusColor,			0,						60,	10,	DF_PLAIN_FIRE,	0,			0,				0,				FUNGUS_LIGHT,	(T_IS_FLAMMABLE | T_STAND_IN_TILE | T_VANISHES_UPON_PROMOTION),										"luminescent fungus",	"luminescent fungus casts a pale, eerie glow."},
	{GRASS_CHAR,	&lichenColor,			0,						60,	50,	DF_PLAIN_FIRE,	0,			DF_LICHEN_GROW,	10000,			NO_LIGHT,		(T_CAUSES_POISON | T_IS_FLAMMABLE | T_STAND_IN_TILE | T_VANISHES_UPON_PROMOTION),					"deadly lichen",		"venomous barbs cover the quivering tendrils of this fast-growing lichen."},
	{FLOOR_CHAR,	&humanBloodColor,		0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_STAND_IN_TILE),																					"a pool of blood",		"the floor is splattered with blood."},
	{FLOOR_CHAR,	&insectBloodColor,		0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_STAND_IN_TILE),																					"a pool of green blood", "the floor is splattered with green blood."},
	{FLOOR_CHAR,	&poisonGasColor,		0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_STAND_IN_TILE),																					"a pool of purple blood", "the floor is splattered with purple blood."},
	{FLOOR_CHAR,	&acidBackColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"the acid-flecked ground", "the floor is splattered with acid."},
	{FLOOR_CHAR,	&vomitColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_STAND_IN_TILE),																					"a puddle of vomit",	"the floor is caked with vomit."},
	{FLOOR_CHAR,	&urineColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				100,			NO_LIGHT,		(T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),														"a puddle of urine",	"a puddle of urine covers the ground."},
	{ASH_CHAR,		&ashForeColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_STAND_IN_TILE),																					"a pile of ashes",		"charcoal and ash crunch underfoot."},
	{FLOOR_CHAR,	&shallowWaterBackColor,	0,						80,	20,	0,				0,			0,				100,			NO_LIGHT,		(T_IS_FLAMMABLE | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),										"a puddle of water",	"a puddle of water covers the ground."},
	{BONES_CHAR,	&bonesForeColor,		0,						70,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"a pile of bones",		"unidentifiable bones, yellowed with age, litter the ground."},
	{BONES_CHAR,	&gray,					0,						70,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0,																									"a pile of rubble",		"rocky rubble covers the ground."},
	{FLOOR_CHAR,	&ectoplasmColor,		0,						70,	0,	DF_PLAIN_FIRE,	0,			0,				0,				ECTOPLASM_LIGHT,(T_STAND_IN_TILE),																					"ectoplasmic residue",	"a thick, glowing substance has congealed on the ground."},
	{ASH_CHAR,		&fireForeColor,			0,						70,	0,	DF_PLAIN_FIRE,	0,			DF_ASH,			300,			EMBER_LIGHT,	(T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),														"sputtering embers",	"sputtering embers cover the ground."},
	{WEB_CHAR,		&white,					0,						20,	100,DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_ENTANGLES | T_IS_FLAMMABLE | T_STAND_IN_TILE | T_VANISHES_UPON_PROMOTION),						"a thick spiderweb",	"thick, sticky spiderwebs fill the area."},
	{FOLIAGE_CHAR,	&foliageColor,			0,						30,	15,	DF_PLAIN_FIRE,	0,			DF_TRAMPLED_FOLIAGE,	0,		NO_LIGHT,		(T_OBSTRUCTS_VISION | T_OBSTRUCTS_SCENT | T_PROMOTES_ON_STEP | T_VANISHES_UPON_PROMOTION | T_IS_FLAMMABLE | T_STAND_IN_TILE),"dense foliage", "dense foliage fills the area, thriving on what sunlight trickles in."},
	{T_FOLIAGE_CHAR,&foliageColor,			0,						60,	15,	DF_PLAIN_FIRE,	0,			DF_FOLIAGE_REGROW,		100,	NO_LIGHT,		(T_VANISHES_UPON_PROMOTION | T_IS_FLAMMABLE),														"trampled foliage",		"dense foliage fills the area, thriving on what sunlight trickles in."},
	{FOLIAGE_CHAR,	&fungusForestLightColor,0,						30,	15,	DF_PLAIN_FIRE,	0,			DF_TRAMPLED_FUNGUS_FOREST,	0,	FUNGUS_FOREST_LIGHT,(T_OBSTRUCTS_VISION | T_OBSTRUCTS_SCENT | T_PROMOTES_ON_STEP | T_VANISHES_UPON_PROMOTION | T_IS_FLAMMABLE | T_STAND_IN_TILE),"a luminescent fungal forest", "luminescent fungal growth fills the area, groping upward from the rich soil."},
	{T_FOLIAGE_CHAR,&fungusForestLightColor,0,						60,	15,	DF_PLAIN_FIRE,	0,			DF_FUNGUS_FOREST_REGROW, 100,	FUNGUS_LIGHT,	(T_VANISHES_UPON_PROMOTION | T_IS_FLAMMABLE),														"trampled fungal forest", "luminescent fungal growth fills the area, groping upward from the rich soil."},
	{WALL_CHAR,		&forceFieldColor,		&forceFieldColor,		0,	0,	0,				0,			0,				-200,			FORCEFIELD_LIGHT, (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_GAS | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),		"a green crystal",	"you are trapped inside a translucent green crystal."},
	{CHAIN_TOP_LEFT,&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the ceiling."},
	{CHAIN_BOTTOM_RIGHT, &gray,				0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the floor."},
	{CHAIN_TOP_RIGHT, &gray,				0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the ceiling."},
	{CHAIN_BOTTOM_LEFT, &gray,				0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the floor."},
	{CHAIN_TOP,		&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{CHAIN_BOTTOM,	&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{CHAIN_LEFT,	&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{CHAIN_RIGHT,	&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0,																									"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{ALTAR_CHAR,	&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			0,				0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS),																		"a candle-lit altar",	"a gilded altar is adorned with candles that flicker in the breeze."},
	{ALTAR_CHAR,	&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			DF_CAGE_CLOSE,	0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS | T_IS_WIRED | T_VANISHES_UPON_PROMOTION | T_PROMOTES_WITHOUT_KEY),	"a candle-lit altar",	"a cage, open on the bottom, hangs over this altar on a retractable chain."},
	{WALL_CHAR,		&altarBackColor,		&veryDarkGray,			17, 0,	0,				0,			DF_CAGE_OPEN,	0,				CANDLE_LIGHT,	(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS | T_IS_WIRED | T_VANISHES_UPON_PROMOTION | T_PROMOTES_WITH_KEY | T_STAND_IN_TILE),"an iron cage","the missing item must be replaced before you can access the remaining items."},
	{ALTAR_CHAR,	&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			DF_ALTAR_INERT,	0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS | T_VANISHES_UPON_PROMOTION | T_PROMOTES_ON_ITEM_PICKUP | T_IS_WIRED),	"a candle-lit altar",	"a weathered stone altar is adorned with candles that flicker in the breeze."},
	{ALTAR_CHAR,	&altarForeColor,		&pedestalBackColor,		17, 0,	0,				0,			0,				0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS),																		"a stone pedestal",		"elaborate carvings wind around this ancient pedestal."},
	
	// fire tiles
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	0,				0,			DF_EMBERS,		500,			FIRE_LIGHT,		(T_IS_FIRE | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),											"billowing flames",		"flames billow upward."},
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	0,				0,			0,				2500,			BRIMSTONE_FIRE_LIGHT,(T_IS_FIRE | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),										"sulfurous flames",		"sulfurous flames leap from the unstable bed of brimstone."},
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	0,				0,			DF_OBSIDIAN,	5000,			FIRE_LIGHT,		(T_IS_FIRE | T_VANISHES_UPON_PROMOTION) | T_STAND_IN_TILE,											"clouds of infernal flame", "billowing infernal flames eat at the floor."},
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	NOTHING,		0,			NOTHING,		8000,			FIRE_LIGHT,		(T_IS_FIRE | T_VANISHES_UPON_PROMOTION) | T_STAND_IN_TILE,											"a cloud of burning gas", "flammable gas fills the air with flame."},
	{FIRE_CHAR,		&yellow,				0,						10,	0,	0,				0,			DF_GAS_FIRE,	10000,			EXPLOSION_LIGHT,(T_IS_FIRE | T_CAUSES_EXPLOSIVE_DAMAGE) | T_STAND_IN_TILE,											"a violent explosion",	"the force of the explosion slams into you."},
	{FIRE_CHAR,		&white,					0,						10,	0,	0,				0,			0,				10000,			INCENDIARY_DART_LIGHT ,(T_IS_FIRE | T_VANISHES_UPON_PROMOTION | T_STAND_IN_TILE),									"a flash of fire",		"flames burst out of the incendiary dart."},
	
	// gas layer
	{	' ',		0,						&poisonGasColor,		35,	100,DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_GAS_DISSIPATES | T_CAUSES_DAMAGE | T_STAND_IN_TILE),							"a cloud of caustic gas", "you can feel the purple gas eating at your flesh."},
	{	' ',		0,						&confusionGasColor,		35,	100,DF_GAS_FIRE,	0,			0,				0,				CONFUSION_GAS_LIGHT,(T_IS_FLAMMABLE | T_GAS_DISSIPATES_QUICKLY | T_CAUSES_CONFUSION | T_STAND_IN_TILE),				"a cloud of confusion gas", "the rainbow-colored gas tickles your brain."},
	{	' ',		0,						&vomitColor,			35,	100,DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_GAS_DISSIPATES_QUICKLY | T_CAUSES_NAUSEA | T_STAND_IN_TILE),					"a cloud of putrescence", "the stench of rotting flesh is overpowering."},
	{	' ',		0,						&pink,					35,	100,DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_GAS_DISSIPATES_QUICKLY | T_CAUSES_PARALYSIS | T_STAND_IN_TILE),					"a cloud of paralytic gas", "the pale gas causes your muscles to stiffen."},
	{	' ',		0,						&methaneColor,			35,	100,DF_EXPLOSION_FIRE,0,		0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_STAND_IN_TILE),																	"a cloud of explosive gas",	"the smell of explosive swamp gas fills the air."},
	{	' ',		0,						&white,					35,	0,	DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_CAUSES_DAMAGE | T_GAS_DISSIPATES_QUICKLY | T_STAND_IN_TILE),										"a cloud of scalding steam", "scalding steam fills the air!"},
	{	' ',		0,						0,						35,	0,	DF_GAS_FIRE,	0,			0,				0,				DARKNESS_CLOUD_LIGHT,	(T_STAND_IN_TILE),																			"a cloud of supernatural darkness", "everything is obscured by an aura of supernatural darkness."},
};

#pragma mark Dungeon Feature definitions

// Features in the gas layer use the startprob as volume, ignore probdecr, and spawn in only a single point.
// Intercepts and slopes are in units of 0.01.
dungeonFeature dungeonFeatureCatalog[NUMBER_DUNGEON_FEATURES] = {
	// tileType					layer		start	decr	fl	txt	fCol fRad	propTerrain	subseqDF	foundation	>Depth	<Depth	freq	minIncp	minSlope	maxNumber
	{0}, // nothing
	{GRANITE,					DUNGEON,	80,		70,		0,	"",	0,	0,		0,			0,			FLOOR,		1,		100,	60,		100,	0,			4},
	{CRYSTAL_WALL,				DUNGEON,	200,	50,		0,	"",	0,	0,		TOP_WALL,	0,			TOP_WALL,	14,		100,	15,		-325,	25,			5},
	{LUMINESCENT_FUNGUS,		SURFACE,	60,		7,		0,	"",	0,	0,		0,			0,			FLOOR,		7,		100,	15,		-300,	70,			14},
	{GRASS,						SURFACE,	75,		5,		0,	"",	0,	0,		0,			0,			FLOOR,		0,		10,		0,		1000,	-60,		10},
	{BONES,						SURFACE,	75,		23,		0,	"",	0,	0,		0,			0,			FLOOR,		12,		100,	30,		0,		0,			4},
	{RUBBLE,					SURFACE,	45,		23,		0,	"",	0,	0,		0,			0,			FLOOR,		0,		100,	30,		0,		0,			4},
	{FOLIAGE,					SURFACE,	100,	33,		0,	"",	0,	0,		0,			0,			FLOOR,		0,		8,		15,		1000,	-333,		10},
	{FUNGUS_FOREST,				SURFACE,	100,	45,		0,	"",	0,	0,		0,			0,			FLOOR,		13,		100,	30,		-600,	50,			12},
	
	// torches
	{TORCH_WALL,				DUNGEON,	0,		0,		0,	"",	0,	0,		0,			0,			TOP_WALL,	6,		100,	5,		-200,	70,			12},
	
	// traps
	{GAS_TRAP_POISON_HIDDEN,	DUNGEON,	0,		0,		0,	"",	0,	0,		0,			0,			FLOOR,		5,		100,	30,		100,	0,			3},
	{GAS_TRAP_PARALYSIS_HIDDEN,	DUNGEON,	0,		0,		0,	"",	0,	0,		0,			0,			FLOOR,		7,		100,	30,		100,	0,			3},
	{TRAP_DOOR_HIDDEN,			DUNGEON,	0,		0,		0,	"",	0,	0,		0,			0,			FLOOR,		9,		100,	30,		100,	0,			2},
	{GAS_TRAP_CONFUSION_HIDDEN,	DUNGEON,	0,		0,		0,	"",	0,	0,		0,			0,			FLOOR,		11,		100,	30,		100,	0,			3},
	{FLAMETHROWER_HIDDEN,		DUNGEON,	0,		0,		0,	"",	0,	0,		0,			0,			FLOOR,		15,		100,	30,		100,	0,			3},
	{FLOOD_TRAP_HIDDEN,			DUNGEON,	0,		0,		0,	"",	0,	0,		0,			0,			FLOOR,		15,		100,	30,		100,	0,			3},
	
	// misc. liquids
	{MUD,						LIQUID,		75,		5,		0,	"",	0,	0,		0,			0,			FLOOR,		1,		100,	30,		0,		0,			5},
	{SUNLIGHT_POOL,				LIQUID,		65,		6,		0,	"",	0,	0,		0,			0,			FLOOR,		0,		5,		15,		500,	-150,		10},
	{DARKNESS_PATCH,			LIQUID,		65,		11,		0,	"",	0,	0,		0,			0,			FLOOR,		1,		15,		15,		500,	-50,		10},
	
	// Dungeon features spawned during gameplay:
	
	// revealed secrets
	{DOOR,						DUNGEON,	0,		0,		0},
	{GAS_TRAP_POISON,			DUNGEON,	0,		0,		0},
	{GAS_TRAP_PARALYSIS,		DUNGEON,	0,		0,		0},
	{CHASM_EDGE,				LIQUID,		100,	100,	0,	"",	0,	0,		0,			0}, // last was DF_SHOW_TRAPDOOR
	{TRAP_DOOR,					LIQUID,		0,		0,		0,	"", 0,	0,		0,			DF_SHOW_TRAPDOOR_HALO},
	{GAS_TRAP_CONFUSION,		DUNGEON,	0,		0,		0},
	{FLAMETHROWER,				DUNGEON,	0,		0,		0},
	{FLOOD_TRAP,				DUNGEON,	0,		0,		0},
	
	// bloods
	// Start probability is actually a percentage for bloods. Base prob is 15 + (damage * 2/3), and then take the percent of that.
	// If it's a gas, we multiply the base by an additional 100. Thus to get a starting gas volume of a poison potion (1000)
	// with a hit for 10 damage, use a starting probability of 48.
	{HUMAN_BLOOD,				SURFACE,	100,	25,		0},
	{GREEN_BLOOD,				SURFACE,	100,	25,		0},
	{PURPLE_BLOOD,				SURFACE,	100,	25,		0},
	{ACID_SPLATTER,				SURFACE,	200,	25,		0},
	{ASH,						SURFACE,	50,		25,		0},
	{EMBERS,					SURFACE,	125,	25,		0},
	{ECTOPLASM,					SURFACE,	110,	25,		0},
	{RUBBLE,					SURFACE,	33,		25,		0},
	{ROT_GAS,					GAS,		12,		0,		0},
	
	// monster effects
	{VOMIT,						SURFACE,	30,		10,		0},
	{POISON_GAS,				GAS,		2000,	0,		0},
	{GAS_EXPLOSION,				SURFACE,	350,	100,	0,	"",	&darkOrange, 4},
	{HUMAN_BLOOD,				SURFACE,	150,	30,		0},
	{FLAMEDANCER_FIRE,			SURFACE,	200,	75,		0},
	
	// misc
	{ROT_GAS,					GAS,		15,		0,		0},
	{STEAM,						GAS,		325,	0,		0},
	{STEAM,						GAS,		15,		0,		0},
	{METHANE_GAS,				GAS,		2,		0,		0},
	{EMBERS,					SURFACE,	0,		0,		0},
	{URINE,						SURFACE,	65,		25,		0},
	{PUDDLE,					SURFACE,	13,		25,		0},
	{ASH,						SURFACE,	0,		0,		0},
	{ECTOPLASM,					SURFACE,	0,		0,		0},
	{FORCEFIELD,				SURFACE,	100,	50,		0},
	{LICHEN,					SURFACE,	2,		100,	0},
	
	// foliage
	{TRAMPLED_FOLIAGE,			SURFACE,	0,		0,		0},
	{FOLIAGE,					SURFACE,	0,		0,		0},
	{TRAMPLED_FUNGUS_FOREST,	SURFACE,	0,		0,		0},
	{FUNGUS_FOREST,				SURFACE,	0,		0,		0},
	
	// brimstone
	{ACTIVE_BRIMSTONE,			LIQUID,		0,		0,		0},
	{INERT_BRIMSTONE,			LIQUID,		0,		0,		0,	"",	0,	0,		0,			DF_BRIMSTONE_FIRE},
	
	// doors, cages and altars
	{OPEN_DOOR,					DUNGEON,	0,		0,		0},
	{DOOR,						DUNGEON,	0,		0,		0},
	{OPEN_IRON_DOOR_INERT,		DUNGEON,	0,		0,		0},
	{ALTAR_CAGE_OPEN,			DUNGEON,	0,		0,		0,	"the cages lift off of the altars as you approach."},
	{ALTAR_CAGE_CLOSED,			DUNGEON,	0,		0,		(DFF_EVACUATE_CREATURES_FIRST), "the cages lower to cover the altars."},
	{ALTAR_INERT,				DUNGEON,	0,		0,		0},
	
	// fire
	{PLAIN_FIRE,				SURFACE,	0,		0,		0},
	{GAS_FIRE,					SURFACE,	0,		0,		0},
	{GAS_EXPLOSION,				SURFACE,	60,		17,		0},
	{DART_EXPLOSION,			SURFACE,	0,		0,		0},
	{BRIMSTONE_FIRE,			SURFACE,	0,		0,		0},
	{CHASM,						LIQUID,		0,		0,		0,	"",	0,	0,		0,			DF_PLAIN_FIRE},
	{PLAIN_FIRE,				SURFACE,	100,	37,		0},
	{EMBERS,					SURFACE,	0,		0,		0},
	{OBSIDIAN,					SURFACE,	0,		0,		0},
	{FLOOD_WATER_SHALLOW,		SURFACE,	225,	37,		0,	"",	0,	0,		0,			DF_FLOOD_2},
	{FLOOD_WATER_DEEP,			SURFACE,	175,	37,		0,	"the area is flooded as water rises through imperceptible holes in the ground."},
	{FLOOD_WATER_SHALLOW,		SURFACE,	10,		25,		0},
	{HOLE,						SURFACE,	200,	100,	0},
	{HOLE_EDGE,					SURFACE,	0,		0,		0},
	
	// gas trap effects
	{POISON_GAS,				GAS,		1000,	0,		0,	"a cloud of caustic gas sprays upward from the floor!"},
	{PARALYSIS_GAS,				GAS,		1000,	0,		0,	"a cloud of pink paralytic gas sprays upward from the floor!"},
	{CONFUSION_GAS,				GAS,		300,	0,		0,	"a sparkling cloud of confusion gas sprays upward from the floor!"},
	{METHANE_GAS,				GAS,		10000,	0,		0}, // debugging toy
	
	// potions
	{POISON_GAS,				GAS,		1000,	0,		0,	"",	&poisonGasColor,4},
	{PARALYSIS_GAS,				GAS,		1000,	0,		0,	"",	&pink,4},
	{CONFUSION_GAS,				GAS,		1000,	0,		0,	"",	&confusionGasColor, 4},
	{PLAIN_FIRE,				SURFACE,	100,	37,		0,	"",	&darkOrange,4},
	{DARKNESS_CLOUD,			GAS,		200,	0,		0},
	{HOLE_EDGE,					SURFACE,	300,	100,	0,	"",	&darkBlue,3,0,			DF_HOLE_2},
	{LICHEN,					SURFACE,	70,		60,		0},
	
	// machine components
	// wooden barricade:
	{WOODEN_BARRICADE,			DUNGEON,	0,		0,		0},
	{PLAIN_FIRE,				SURFACE,	0,		0,		0,	"flames quickly consume the wooden barricade."},
	
	// shallow water flood machine:
	{MACHINE_FLOOD_WATER_SPREADING,	LIQUID,	0,		0,		0,	"you hear a heavy click, and the nearby water begins flooding the area!"},
	{SHALLOW_WATER,				LIQUID,		0,		0,		0},
	{MACHINE_FLOOD_WATER_SPREADING,LIQUID,	100,	100,	0,	"",	0,	0,		0,			DF_SHALLOW_WATER},
	{MACHINE_FLOOD_WATER_DORMANT,LIQUID,	250,	100,	(DFF_TREAT_AS_BLOCKING), "", 0, 0, 0, DF_DEEP_WATER_POOL},
	{DEEP_WATER,				LIQUID,		90,		100,	(DFF_PERMIT_BLOCKING)},
	
	// unstable floor machine:
	{MACHINE_COLLAPSE_EDGE_SPREADING,LIQUID,0,		0,		0,	"you hear a deep rumbling noise as the floor of the room begins to collapse!"},
	{CHASM,						LIQUID,		0,		0,		0},
	{MACHINE_COLLAPSE_EDGE_SPREADING,LIQUID,100,	100,	0,	"",	0,	0,		0,			DF_COLLAPSE},
	
	// levitation bridge machine:
	{STONE_BRIDGE,				LIQUID,		0,		0,		0,	"a stone bridge extends from the floor with a grinding sound."},
	
	// hidden poison vent machine:
	{MACHINE_POISON_GAS_VENT,	DUNGEON,	0,		0,		0,	"deadly purple gas starts wafting out of hidden vents in the floor!"},
	{PORTCULLIS,				DUNGEON,	0,		0,		(DFF_EVACUATE_CREATURES_FIRST),	"an iron portcullis falls from the ceiling and locks into place!"},
	{POISON_GAS,				GAS,		25,		0,		0},
	
	// hidden methane vent machine:
	{MACHINE_METHANE_VENT,		DUNGEON,	0,		0,		0,	"explosive methane gas starts wafting out of hidden vents in the floor!", 0, 0, 0, DF_VENT_SPEW_METHANE},
	{METHANE_GAS,				GAS,		60,		0,		0},
	{PILOT_LIGHT,				DUNGEON,	0,		0,		0,	"a torch falls from its mount and lies sputtering on the floor."},
	
	// dungeon
	{HUMAN_BLOOD,				SURFACE,	50,		40,		0},
	
	// statuary:
	{STATUE_CRACKING,			DUNGEON,	0,		0,		0,	"cracks begin snaking across the marble surface of the statues!", 0, 0, 0, DF_RUBBLE},
	{RUBBLE,					SURFACE,	120,	100,	DFF_ACTIVATE_DORMANT_MONSTER,	"the statue shatters!", 0, 0, 0, DF_RUBBLE},
	
	// hidden turrets:
	{TOP_WALL,					DUNGEON,	0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"you hear a click, and the stones in the wall shift to reveal turrets!", 0, 0, 0, DF_RUBBLE},
	
	// haunted room:
	{DARK_FLOOR_DARKENING,		DUNGEON,	0,		0,		0,	"the light in the room flickers and you feel a sudden chill in the air."},
	{DARK_FLOOR,				DUNGEON,	0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"", 0, 0, 0, DF_ECTOPLASM_DROPLET},

};

#pragma mark Lights

// radius is in units of 0.01
const lightSource lightCatalog[NUMBER_LIGHT_KINDS] = {
	//color					radius range			fade%	passThroughCreatures
	{0},															// NO_LIGHT
	{&minersLightColor,		{0, 0, 1},				35,		true},		// miners light
	{&fireBoltColor,		{300, 400, 1},			0,		false},		// burning creature light
	{&wispLightColor,		{400, 800, 1},			0,		false},		// will-o'-the-wisp light
	{&fireBoltColor,		{300, 400, 1},			0,		false},		// salamander glow
	{&pink,					{600, 600, 1},			0,		true},		// imp light
	{&pixieColor,			{400, 600, 1},			50,		false},		// pixie light
	{&lichLightColor,		{1500, 1500, 1},		0,		false},		// lich light
	{&flamedancerCoronaColor,{1000, 2000, 1},		0,		false},		// flamedancer light
	{&spectralBladeLightColor,{350, 350, 1},		0,		false},		// spectral blades
	{&summonedImageLightColor,{350, 350, 1},		0,		false},		// weapon images
	{&lightningColor,		{250, 250, 1},			35,		false},		// lightning turret light
	{&lightningColor,		{300, 300, 1},			0,		false},		// bolt glow
	
	// glowing terrain:
	{&torchLightColor,		{1000, 1000, 1},		50,		false},		// torch
	{&lavaLightColor,		{300, 300, 1},			50,		false},		// lava
	{&sunLightColor,		{200, 200, 1},			25,		true},		// sunlight
	{&darknessPatchColor,	{400, 400, 1},			0,		true},		// darkness patch
	{&fungusLightColor,		{300, 300, 1},			50,		false},		// luminescent fungus
	{&fungusForestLightColor,{500, 500, 1},			0,		false},		// luminescent forest
	{&ectoplasmColor,		{200, 200, 1},			50,		false},		// ectoplasm
	{&lavaLightColor,		{200, 200, 1},			50,		false},		// embers
	{&lavaLightColor,		{500, 1000, 1},			0,		false},		// fire
	{&lavaLightColor,		{200, 300, 1},			0,		false},		// brimstone fire
	{&explosionColor,		{DCOLS*100,DCOLS*100,1},100,	false},		// explosions
	{&dartFlashColor,		{15*100,15*100,1},		0,		false},		// incendiary darts
	{&confusionGasColor,	{300, 300, 1},			100,	false},		// confusion gas
	{&darknessCloudColor,	{500, 500, 1},			0,		true},		// darkness cloud
	{&forceFieldLightColor,	{200, 200, 1},			50,		false},		// forcefield
	{&crystalWallLightColor,{300, 500, 1},			50,		false},		// crystal wall
	{&torchLightColor,		{200, 400, 1},			0,		false},		// candle light
};

#pragma mark Blueprints

const blueprint blueprintCatalog[NUMBER_BLUEPRINTS] = {
	//BLUEPRINTS:
	//depths			roomSize	freq	featureCt	flags	(features on subsequent lines)
	
		//FEATURES:
		//DF		terrain		layer		instanceCtRange	minInsts	itemCat		itemKind	itemValMin		monsterKind		reqSpace		hordeFl		itemFlags	featureFlags
	
	// -- MAIN ATTRACTIONS --
	// item library -- can check one item out at a time
	{{1, AMULET_LEVEL},	{30, 50},	50,		4,			(BF_PURGE_INTERIOR | BF_SURROUND_WITH_WALLS | BF_IMPREGNABLE | BF_REWARD_ROOM),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			LOCKED_DOOR, DUNGEON,		{1,1},		1,			KEY,		0,			0,				0,				2,				0,			(ITEM_IS_KEY | ITEM_IS_DISPOSABLE),	(MF_BUILD_IN_DOORWAY | MF_PERMIT_BLOCKING | MF_GENERATE_ITEM | MF_OUTSOURCE_ITEM)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{3,3},		3,			(WEAPON|ARMOR|WAND),-1,	0,				0,				2,				0,			(ITEM_IS_KEY | ITEM_NAMED),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{2,2},		2,			(STAFF|RING),-1,		0,				0,				2,				0,			(ITEM_IS_KEY | ITEM_NAMED),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING)}}},
	// treasure room -- apothecary or archive (potions or scrolls)
	{{1, AMULET_LEVEL},	{30, 50},	20,		6,			(BF_PURGE_INTERIOR | BF_SURROUND_WITH_WALLS | BF_IMPREGNABLE | BF_REWARD_ROOM),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			0,			0,				{5,7},		2,			(POTION),	-1,			0,				0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
		{0,			0,			0,				{4,6},		2,			(SCROLL),	-1,			100,			0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
		{0,			FUNGUS_FOREST,SURFACE,		{3,4},		0,			0,			-1,			0,				0,				2,				0,			0,			0},
		{0,			LOCKED_DOOR, DUNGEON,		{1,1},		1,			KEY,		0,			0,				0,				1,				0,			(ITEM_IS_KEY | ITEM_IS_DISPOSABLE),(MF_BUILD_IN_DOORWAY | MF_PERMIT_BLOCKING | MF_GENERATE_ITEM | MF_OUTSOURCE_ITEM)}}},
	// guaranteed good item on a glowing pedestal
	{{1, AMULET_LEVEL},	{15, 45},	30,		6,			(BF_PURGE_INTERIOR | BF_SURROUND_WITH_WALLS | BF_IMPREGNABLE | BF_REWARD_ROOM),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(WEAPON),	-1,			500,			0,				2,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(ARMOR),	-1,			500,			0,				2,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_TREAT_AS_BLOCKING)},
		{0,			PEDESTAL,	DUNGEON,		{2,2},		2,			(STAFF),	-1,			1000,			0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
		{0,			PEDESTAL,	DUNGEON,		{2,2},		2,			(SCROLL),	SCROLL_ENCHANT_ITEM,0,		0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
		{0,			LOCKED_DOOR, DUNGEON,		{1,1},		1,			KEY,		0,			0,				0,				1,				0,			(ITEM_IS_KEY | ITEM_IS_DISPOSABLE),(MF_BUILD_IN_DOORWAY | MF_PERMIT_BLOCKING | MF_GENERATE_ITEM | MF_OUTSOURCE_ITEM)}}},
	// dungeon -- allies chained up for the taking
	{{5, AMULET_LEVEL},	{30, 80},	20,		6,			(BF_REWARD_ROOM),	{
		{0,			BONES,		SURFACE,		{3,4},		3,			0,			-1,			0,				0,				2,				HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE, 0, MF_GENERATE_HORDE | MF_TREAT_AS_BLOCKING},
		{DF_AMBIENT_BLOOD,MANACLE_TR,SURFACE,	{2,3},		1,			0,			-1,			0,				0,				1,				0,			0,			0},
		{DF_AMBIENT_BLOOD,MANACLE_TL,SURFACE,	{2,3},		1,			0,			-1,			0,				0,				1,				0,			0,			0},
		{DF_BONES,	0,			0,				{2,3},		1,			0,			-1,			0,				0,				1,				0,			0,			0},
		{DF_VOMIT,	0,			0,				{2,3},		1,			0,			-1,			0,				0,				1,				0,			0,			0},
		{0,			LOCKED_DOOR, DUNGEON,		{1,1},		1,			KEY,		0,			0,				0,				1,				0,			(ITEM_IS_KEY | ITEM_IS_DISPOSABLE),(MF_BUILD_IN_DOORWAY | MF_PERMIT_BLOCKING | MF_GENERATE_ITEM | MF_OUTSOURCE_ITEM)}}},
	
	// -- KEY HOLDERS --
	// Secret room -- key on an altar in a secret room
	{{1, AMULET_LEVEL},	{15, 100},	1,		2,			(BF_ADOPT_ITEM_MANDATORY), {
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING)},
		{0,			SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)}}},
	// Flammable barricade -- burn the wooden barricade to enter
	{{1, 15},			{15, 100},	8,		3,			(BF_ADOPT_ITEM_MANDATORY), {
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING)},
		{0,			ADD_WOODEN_BARRICADE,DUNGEON,{1,1},		1,			0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)},
		{0,			0,			0,				{1,1},		1,			WEAPON,		INCENDIARY_DART,0,			0,				1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL)}}},
	// Flood zone -- key on an altar in a room with pools of eel-infested waters; take key to flood room with shallow water
	{{3, AMULET_LEVEL},	{80, 180},	10,		4,			(BF_SURROUND_WITH_WALLS | BF_PURGE_LIQUIDS | BF_PURGE_PATHING_BLOCKERS | BF_ADOPT_ITEM_MANDATORY | BF_IMPREGNABLE),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				5,				0,			0,			(MF_FAR_FROM_GATE | MF_TREAT_AS_BLOCKING)},
		{DF_SPREADABLE_WATER_POOL,0,0,			{2, 4},		1,			0,			-1,			0,				0,				5,				HORDE_MACHINE_WATER_MONSTER,0,MF_GENERATE_HORDE},
		{0,			DOOR,		DUNGEON,		{1,1},		1,			0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)},
		{0,			SHALLOW_WATER,LIQUID,		{1,1},		1,			0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)}}},
	// Collapsing floor -- key on an altar in a plain room; take key to cause the floor of the room to collapse
	{{1, AMULET_LEVEL},	{20, 60},	10,		4,			(BF_SURROUND_WITH_WALLS | BF_PURGE_LIQUIDS | BF_IMPREGNABLE | BF_ADOPT_ITEM_MANDATORY),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				4,				0,			0,			(MF_FAR_FROM_GATE | MF_TREAT_AS_BLOCKING)},
		{0,			MACHINE_COLLAPSE_EDGE_DORMANT,LIQUID,{1, 2},1,		0,			-1,			0,				0,				2,				0,			0,			MF_NEAR_GATE},
		{0,			DOOR,		DUNGEON,		{1,1},		1,			0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)},
		{0,			CHASM_EDGE,	LIQUID,			{1,1},		1,			0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)}}},
	// Pit traps -- key on an altar, room full of pit traps
	{{1, AMULET_LEVEL},	{20, 100},	10,		3,			(BF_ADOPT_ITEM_MANDATORY),	{
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				2,				0,			0,			(MF_FAR_FROM_GATE | MF_TREAT_AS_BLOCKING)},
		{0,			TRAP_DOOR_HIDDEN,DUNGEON,	{30, 40},	1,			0,			-1,			0,				0,				1,				0,			0,			MF_TREAT_AS_BLOCKING},
		{0,			SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)}}},
	// Levitation challenge -- key on an altar, room filled with pit/lava, levitation elsewhere on level, bridge appears when you touch the key
	{{1, AMULET_LEVEL},	{75, 120},	10,		6,			(BF_ADOPT_ITEM_MANDATORY | BF_PURGE_INTERIOR | BF_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				3,				0,			0,			(MF_FAR_FROM_GATE | MF_TREAT_AS_BLOCKING)},
		{0,			0,			0,				{1,1},		1,			0,			0,			0,				0,				3,				0,			0,			MF_BUILD_IN_DOORWAY},
		{DF_SHOW_TRAPDOOR,		0,			0,	{120, 120},	1,			0,			-1,			0,				0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING)},
		{DF_SHOW_TRAPDOOR,		0,			0,	{120, 120},	0,			0,			-1,			0,				0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING)},
		{DF_SHOW_TRAPDOOR_HALO,	CHASM_WITH_HIDDEN_BRIDGE,LIQUID,{1,1},1,0,			0,			0,				0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_EVERYWHERE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_LEVITATION,0,		0,				1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL)}}},
	// Poison gas -- key on an altar; take key to cause a poison gas vent to appear and the door to be blocked; there is a hidden trapdoor somewhere
	{{4, AMULET_LEVEL},	{35, 60},	8,		4,			(BF_PURGE_INTERIOR | BF_SURROUND_WITH_WALLS | BF_ADOPT_ITEM_MANDATORY),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				2,				0,			0,			MF_TREAT_AS_BLOCKING},
		{0,			MACHINE_POISON_GAS_VENT_DORMANT,DUNGEON,{1,3},1,	0,			-1,			0,				0,				2,				0,			0,			0},
		{DF_TRAPDOOR,0,			0,				{1,1},		1,			0,			-1,			0,				0,				2,				0,			0,			0},
		{0,			MACHINE_BLOCKABLE_DOORWAY,DUNGEON,{1,1},1,			0,			0,			0,				0,				1,				0,			0,			(MF_BUILD_IN_DOORWAY | MF_PERMIT_BLOCKING)}}},
	// Explosive situation -- key on an altar; take key to cause a methane gas vent to appear and a pilot light to ignite
	{{7, AMULET_LEVEL},	{80, 90},	10,		3,			(BF_PURGE_LIQUIDS | BF_SURROUND_WITH_WALLS | BF_ADOPT_ITEM_MANDATORY),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				1,				0,			0,			MF_TREAT_AS_BLOCKING | MF_FAR_FROM_GATE},
		{0,			MACHINE_METHANE_VENT_DORMANT,DUNGEON,{1,1}, 1,		0,			-1,			0,				0,				1,				0,			0,			MF_NEAR_GATE},
		{0,			PILOT_LIGHT_DORMANT,DUNGEON,{1,1},		1,			0,			-1,			0,				0,				1,				0,			0,			MF_FAR_FROM_GATE | MF_BUILD_IN_WALLS}}},
	// Burning grass -- key on an altar; take key to cause pilot light to ignite grass in room
	{{1, 7},			{40, 110},	10,		4,			(BF_PURGE_INTERIOR | BF_SURROUND_WITH_WALLS | BF_ADOPT_ITEM_MANDATORY),	{
		{DF_GRASS,	ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				1,				0,			0,			MF_TREAT_AS_BLOCKING | MF_FAR_FROM_GATE},
		{0,			GRASS,		SURFACE,		{0,0},		0,			0,			-1,			0,				0,				0,				0,			0,			(MF_EVERYWHERE)},
		{DF_FOLIAGE,0,			SURFACE,		{1,2},		0,			0,			-1,			0,				0,				1,				0,			0,			0},
		{0,			PILOT_LIGHT_DORMANT,DUNGEON,{1,1},		1,			0,			-1,			0,				0,				1,				0,			0,			MF_NEAR_GATE | MF_BUILD_IN_WALLS}}},
	// Statuary -- key on an altar, room full of statues; take key to cause statues to burst and reveal monsters
	{{10, AMULET_LEVEL},{35, 90},	10,		2,			(BF_ADOPT_ITEM_MANDATORY),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				2,				0,			0,			(MF_FAR_FROM_GATE | MF_TREAT_AS_BLOCKING)},
		{0,			STATUE_DORMANT,DUNGEON,		{3, 5},		3,			0,			-1,			0,				0,				2,				HORDE_MACHINE_STATUE,0,	(MF_TREAT_AS_BLOCKING | MF_GENERATE_HORDE | MF_MONSTERS_DORMANT)}}},
	// Haunted house -- key on an altar; take key to cause the room to darken, ectoplasm to cover everything and phantoms to appear
	{{16, AMULET_LEVEL},{45, 150},	10,		3,			(BF_ADOPT_ITEM_MANDATORY | BF_PURGE_INTERIOR | BF_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				2,				0,			0,			(MF_FAR_FROM_GATE | MF_TREAT_AS_BLOCKING)},
		{0,			DARK_FLOOR_DORMANT,DUNGEON,	{0,0},		0,			0,			-1,			0,				0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			DARK_FLOOR_DORMANT,DUNGEON,	{4,6},		4,			0,			-1,			0,				MK_PHANTOM,		1,				0,			0,			(MF_GENERATE_MONSTER | MF_MONSTERS_DORMANT)}}},
	// Gauntlet -- key on an altar; take key to cause turrets to emerge
	{{5, 24},			{35, 90},	10,		2,			(BF_ADOPT_ITEM_MANDATORY),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				0,				2,				0,			0,			(MF_FAR_FROM_GATE | MF_TREAT_AS_BLOCKING)},
		{0,			TURRET_DORMANT,DUNGEON,		{4, 6},		4,			0,			-1,			0,				0,				2,				HORDE_MACHINE_TURRET,0,	MF_TREAT_AS_BLOCKING | MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS}}},
	// Boss -- key is held by a boss atop a pile of bones in a secret room. A few fungus trees light up the area.
	{{5, AMULET_LEVEL},	{40, 100},	25,		3,			(BF_ADOPT_ITEM_MANDATORY | BF_SURROUND_WITH_WALLS | BF_PURGE_LIQUIDS), {
		{DF_BONES,	0,			0,				{1,1},		1,			0,			-1,			0,				0,				1,				HORDE_MACHINE_BOSS,	0,	(MF_FAR_FROM_GATE | MF_MONSTER_TAKE_ITEM | MF_GENERATE_HORDE | MF_MONSTER_SLEEPING)},
		{DF_BONES,	SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				0,				3,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_IN_DOORWAY)},
		{DF_LUMINESCENT_FUNGUS,	FUNGUS_FOREST,SURFACE,{1,2},0,			0,			-1,			0,				0,				2,				0,			0,			0}}},
};


#pragma mark Monster definitions

// Defines all creatures, which include monsters and the player:
creatureType monsterCatalog[NUMBER_MONSTER_KINDS] = {
	//	name			ch		color			exp		HP		def		acc		damage			reg	sight	scent	move	attack	blood			light	DFChance DFType		behaviorF, abilityF
	{0,	"you",	PLAYER_CHAR,	&playerInLightColor,	0,		15,		0,		75,		{1, 2, 1},		20,	DCOLS,	30,		100,	100,	DF_HUMAN_BLOOD,	0,		0,		0},
	
	{0, "rat",			'r',	&gray,			1,		6,		0,		80,		{1, 3, 1},		20,	20,		30,		100,	100,	DF_HUMAN_BLOOD,	0,		1,		DF_URINE},
	{0, "kobold",		'k',	&goblinColor,	2,		7,		0,		80,		{1, 3, 1},		20,	30,		30,		100,	100,	DF_HUMAN_BLOOD,	0,		0,		0},
	{0,	"jackal",		'j',	&jackalColor,	3,		7,		0,		70,		{1, 4, 1},		20,	50,		50,		50,		100,	DF_HUMAN_BLOOD,	0,		1,		DF_URINE},
	{0,	"eel",			'e',	&eelColor,		15,		18,		20,		100,	{2, 9, 2},		5,	DCOLS,	20,		50,		100,	NOTHING,		0,		0,		0,
		(MONST_RESTRICTED_TO_LIQUID | MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_FLITS | MONST_NEVER_SLEEPS | MONST_WILL_NOT_USE_STAIRS)},
	{0,	"monkey",		'm',	&ogreColor,		3,		12,		17,		100,	{1, 3, 1},		20,	DCOLS,	100,	100,	100,	DF_HUMAN_BLOOD,	0,		1,		DF_URINE,
		(0), (MA_HIT_STEAL_FLEE)},
	{0, "bloat",		'b',	&poisonGasColor,15,		4,		0,		100,	{0, 0, 0},		5,	DCOLS,	100,	100,	100,	DF_PURPLE_BLOOD,0,		0,		DF_BLOAT_DEATH,
		(MONST_FLIES | MONST_FLITS), (MA_KAMIKAZE | MA_DF_ON_DEATH)},
	{0, "goblin",		'g',	&goblinColor,	5,		15,		10,		70,		{2, 6, 1},		20,	30,		20,		100,	100,	DF_HUMAN_BLOOD,	0,		0,		0},
	{0, "goblin conjurer",'g',	&goblinConjurerColor, 15, 7,	10,		70,		{2, 4, 1},		20,	30,		20,		100,	100,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_MAINTAINS_DISTANCE | MONST_CAST_SPELLS_SLOWLY | MONST_CARRY_ITEM_25 | MONST_WILL_NOT_USE_STAIRS), (MA_CAST_SUMMON)},
	{0, "goblin totem",	TOTEM_CHAR,	&orange,	15,		30,		0,		0,		{0, 0, 0},		0,	DCOLS,	200,	100,	300,	DF_RUBBLE_BLOOD,IMP_LIGHT,0,	0,
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_INTRINSIC_LIGHT | MONST_IMMOBILE | MONST_INANIMATE | MONST_ALWAYS_HUNTING | MONST_WILL_NOT_USE_STAIRS), (MA_CAST_HASTE | MA_CAST_SPARK)},
	{0, "pink jelly",	'J',	&pinkJellyColor,10,		60,		0,		100,	{1, 3, 1},		0,	20,		20,		100,	100,	DF_PURPLE_BLOOD,0,		0,		0,
		(MONST_NEVER_SLEEPS), (MA_CLONE_SELF_ON_DEFEND)},
	{0, "toad",			't',	&toadColor,		5,		18,		0,		90,		{1, 4, 1},		10,	15,		15,		100,	100,	DF_GREEN_BLOOD,	0,		0,		0,
		(0), (MA_HIT_HALLUCINATE)},
	{0, "vampire bat",	'v',	&gray,			10,		18,		25,		100,	{2, 6, 1},		20,	DCOLS,	50,		50,		100,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_FLIES | MONST_FLITS), (MA_TRANSFERENCE)},
	{0, "arrow turret", TURRET_CHAR,&black,		10,		30,		0,		90,		{2, 6, 1},		0,	DCOLS,	50,		100,	250,	NOTHING,		0,		0,		0,
		(MONST_TURRET), (MA_ATTACKS_FROM_DISTANCE)},
	{0, "acid mound",	'a',	&acidBackColor,	12,		15,		10,		70,		{1, 3, 1},		5,	15,		15,		100,	100,	DF_ACID_BLOOD,	0,		0,		0,
		(MONST_DEFEND_DEGRADE_WEAPON), (MA_HIT_DEGRADE_ARMOR)},
	{0, "centipede",	'c',	&centipedeColor,10,		20,		20,		80,		{1, 15, 1},		20,	20,		50,		100,	100,	DF_GREEN_BLOOD,	0,		0,		0,
		(0), (MA_POISONS)},
	{0,	"ogre",			'O',	&ogreColor,		50,		55,		80,		125,	{8, 14, 2},		20,	30,		30,		100,	200,	DF_HUMAN_BLOOD,	0,		0,		0,			(0)},
	{0,	"bog monster",	'B',	&krakenColor,	50,		55,		80,		5000,	{3, 4, 1},		3,	30,		30,		200,	100,	NOTHING,		0,		0,		0,
		(MONST_RESTRICTED_TO_LIQUID | MONST_SUBMERGES | MONST_FLITS | MONST_FLEES_NEAR_DEATH), (MA_SEIZES)},
	{0, "ogre totem",	TOTEM_CHAR,	&green,		100,	70,		0,		0,		{0, 0, 0},		0,	DCOLS,	200,	100,	400,	DF_RUBBLE_BLOOD,LICH_LIGHT,0,	0,
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_INTRINSIC_LIGHT | MONST_IMMOBILE | MONST_INANIMATE | MONST_ALWAYS_HUNTING | MONST_WILL_NOT_USE_STAIRS), (MA_CAST_HEAL | MA_CAST_SLOW)},
	{0, "spider",		's',	&white,			60,		20,		90,		90,		{3, 14, 2},		20,	50,		20,		100,	100,	DF_GREEN_BLOOD,	0,		0,		0,
		(MONST_IMMUNE_TO_WEBS | MONST_CAST_SPELLS_SLOWLY), (MA_SHOOTS_WEBS | MA_POISONS)},
	{0, "spark turret", TURRET_CHAR, &lightningColor, 70,80,	0,		100,	{0, 0, 0},		0,	DCOLS,	50,		100,	150,	NOTHING,		SPARK_TURRET_LIGHT,	0,	0,
		(MONST_TURRET | MONST_INTRINSIC_LIGHT), (MA_CAST_SPARK)},
	{0,	"will-o-the-wisp",'w',	&wispLightColor,40,		10,		120,	100,	{3,	10, 2},		5,	90,		15,		100,	100,	DF_ASH_BLOOD,	WISP_LIGHT,	0,	0,
		(MONST_IMMUNE_TO_FIRE | MONST_FLIES | MONST_FLITS | MONST_NEVER_SLEEPS | MONST_FIERY | MONST_INTRINSIC_LIGHT | MONST_DIES_IF_NEGATED)},
	{0, "wraith",		'W',	&wraithColor,	60,		50,		70,		120,	{4, 15, 2},		5,	DCOLS,	100,	50,		100,	DF_GREEN_BLOOD,	0,		0,		0,
		(MONST_FLEES_NEAR_DEATH)},
	{0, "zombie",		'z',	&vomitColor,	45,		55,		0,		120,	{4, 15, 1},		0,	50,		200,	200,	200,	DF_ROT_GAS_BLOOD,0,		100,	DF_ROT_GAS_PUFF, (0)},
	{0, "troll",		'T',	&trollColor,	150,	65,		90,		125,	{5, 20, 3},		1,	DCOLS,	20,		100,	100,	DF_HUMAN_BLOOD,	0,		0,		0,			(0)},
	{0,	"ogre shaman",	'O',	&green,			120,	45,		70,		100,	{4, 10, 1},		20,	DCOLS,	30,		100,	200,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_MAINTAINS_DISTANCE | MONST_CAST_SPELLS_SLOWLY), (MA_CAST_HASTE | MA_CAST_SPARK | MA_CAST_SUMMON)},
	{0, "naga",			'N',	&trollColor,	150,	75,		100,	150,	{5, 13, 4},		10,	DCOLS,	100,	100,	100,	DF_GREEN_BLOOD,	0,		100,	DF_PUDDLE,
		(MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_NEVER_SLEEPS)},
	{0, "salamander",	'S',	&salamanderColor,150,	60,		110,	150,	{5, 15, 3},		10,	DCOLS,	100,	100,	100,	DF_ASH_BLOOD,	SALAMANDER_LIGHT, 100, DF_SALAMANDER_FLAME,
		(MONST_IMMUNE_TO_FIRE | MONST_SUBMERGES | MONST_NEVER_SLEEPS | MONST_FIERY | MONST_INTRINSIC_LIGHT)},
	{0, "explosive bloat",'b',	&orange,		20,		10,		0,		100,	{0, 0, 0},		5,	DCOLS,	100,	100,	100,	DF_HUMAN_BLOOD,	EMBER_LIGHT,0,	DF_BLOAT_EXPLOSION,
		(MONST_FLIES | MONST_FLITS| MONST_INTRINSIC_LIGHT), (MA_KAMIKAZE | MA_DF_ON_DEATH)},
	{0, "dar blademaster",'d',	&purple,		175,	35,		120,	160,	{2, 12, 2},		20,	DCOLS,	100,	100,	100,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_CARRY_ITEM_25), (MA_CAST_BLINK)},
	{0, "dar priestess", 'd',	&darPriestessColor,150,	20,		80,		100,	{2, 5, 1},		20,	DCOLS,	100,	100,	100,	DF_HUMAN_BLOOD, 0,		0,		0,
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25), (MA_CAST_HEAL | MA_CAST_SPARK | MA_CAST_HASTE | MA_CAST_NEGATION)},
	{0, "dar battlemage",'d',	&darMageColor,	150,	20,		80,		100,	{1, 3, 1},		20,	DCOLS,	100,	100,	100,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25), (MA_CAST_FIRE | MA_CAST_SLOW | MA_CAST_DISCORD)},
	{0,	"centaur",		'C',	&tanColor,		175,	35,		50,		175,	{2, 10, 2},		20,	DCOLS,	30,		50,		200,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_MAINTAINS_DISTANCE | MONST_CAST_SPELLS_SLOWLY), (MA_ATTACKS_FROM_DISTANCE)},
	{0, "acid turret", TURRET_CHAR,	&darkGreen,	120,	35,		0,		250,	{1, 2, 1},		0,	DCOLS,	50,		100,	250,	NOTHING,		0,		0,		0,
		(MONST_TURRET), (MA_ATTACKS_FROM_DISTANCE | MA_HIT_DEGRADE_ARMOR)},
	{0,	"kraken",		'K',	&krakenColor,	200,	120,	0,		150,	{10, 25, 3},	1,	DCOLS,	20,		50,		100,	NOTHING,		0,		0,		0,
		(MONST_RESTRICTED_TO_LIQUID | MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_FLITS | MONST_NEVER_SLEEPS | MONST_FLEES_NEAR_DEATH | MONST_WILL_NOT_USE_STAIRS), (MA_SEIZES)},
	{0,	"lich",			'L',	&white,			250,	35,		100,	175,	{1, 7, 1},		0,	DCOLS,	100,	100,	100,	DF_ASH_BLOOD,	LICH_LIGHT,	0,	0,
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25 | MONST_INTRINSIC_LIGHT), (MA_CAST_SUMMON | MA_CAST_FIRE)},
	{0, "pixie",		'p',	&pixieColor,	150,	10,		120,	100,	{1, 3, 1},		20,	100,	100,	50,		100,	DF_GREEN_BLOOD,	PIXIE_LIGHT, 0,	0,
		(MONST_MAINTAINS_DISTANCE | MONST_INTRINSIC_LIGHT | MONST_FLIES | MONST_FLITS), (MA_CAST_SPARK | MA_CAST_SLOW | MA_CAST_NEGATION | MA_CAST_DISCORD)},
	{0,	"phantom",		'P',	&ectoplasmColor,180,	35,		100,	160,	{8, 22, 4},		0,	30,		30,		50,		200,	DF_ECTOPLASM_BLOOD,	0,	2,		DF_ECTOPLASM_DROPLET,
		(MONST_INVISIBLE | MONST_FLITS | MONST_FLIES | MONST_IMMUNE_TO_WEBS)},
	{0, "flame turret", TURRET_CHAR, &lavaForeColor,150,40,		0,		150,	{1, 2, 1},		0,	DCOLS,	50,		100,	250,	NOTHING,		LAVA_LIGHT,	0,	0,
		(MONST_TURRET | MONST_INTRINSIC_LIGHT), (MA_CAST_FIRE)},
	{0, "imp",			'i',	&pink,			150,	35,		110,	225,	{3, 10, 2},		10,	10,		15,		100,	100,	DF_GREEN_BLOOD,	IMP_LIGHT,	0,	0,
		(MONST_INTRINSIC_LIGHT), (MA_HIT_STEAL_FLEE | MA_CAST_BLINK)},
	{0,	"fury",			'f',	&darkRed,		50,		19,		110,	200,	{4, 13, 4},		20,	40,		30,		50,		100,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_NEVER_SLEEPS | MONST_FLIES)},
	{0, "revenant",		'R',	&ectoplasmColor,300,	30,		0,		200,	{5, 30, 5},		0,	DCOLS,	20,		100,	100,	DF_ECTOPLASM_BLOOD,	0,	0,		0,
		(MONST_IMMUNE_TO_WEAPONS)},
	{0, "tentacle horror",'H',	&centipedeColor,2000,	120,	120,	225,	{10, 50, 10},	1,	DCOLS,	50,		100,	100,	NOTHING,		0,		0,		0,			(0)},
	{0, "golem",		'G',	&gray,			1000,	400,	100,	225,	{4, 8, 1},		0,	DCOLS,	200,	100,	100,	DF_RUBBLE_BLOOD,0,		0,		0,
		(MONST_REFLECT_4 | MONST_DIES_IF_NEGATED)},
	{0, "dragon",		'D',	&dragonColor,	5000,	150,	140,	250,	{15, 60, 5},	20,	DCOLS,	120,	50,		200,	DF_GREEN_BLOOD,	0,		0,		0,
		(MONST_IMMUNE_TO_FIRE | MONST_CARRY_ITEM_100), (MA_BREATHES_FIRE)},
	
	// bosses
	{0, "goblin chieftan",'g',	&blue,			40,		30,		17,		100,	{3, 6, 1},		20,	30,		20,		100,	100,	DF_HUMAN_BLOOD,	0,		0,		0,
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25 | MONST_WILL_NOT_USE_STAIRS), (MA_CAST_SUMMON)},
	{0,	"black jelly",	'J',	&black,			150,	120,	0,		130,	{3, 8, 1},		0,	20,		20,		100,	100,	DF_PURPLE_BLOOD,0,		0,		0,
		(0), (MA_CLONE_SELF_ON_DEFEND)},
	{0, "vampire",		'V',	&white,			200,	75,		120,	120,	{4, 15, 2},		6,	DCOLS,	100,	50,		100,	DF_HUMAN_BLOOD,	0,		0,		DF_BLOOD_EXPLOSION,
		(MONST_FLEES_NEAR_DEATH), (MA_CAST_BLINK | MA_CAST_DISCORD | MA_TRANSFERENCE | MA_DF_ON_DEATH | MA_CAST_SUMMON | MA_ENTER_SUMMONS)},
	{0, "flamedancer",	'F',	&white,			200,	65,		120,	120,	{3, 8, 2},		0,	DCOLS,	100,	100,	100,	DF_EMBER_BLOOD,	FLAMEDANCER_LIGHT,100,DF_FLAMEDANCER_CORONA,
		(MONST_MAINTAINS_DISTANCE | MONST_IMMUNE_TO_FIRE | MONST_FIERY | MONST_INTRINSIC_LIGHT), (MA_CAST_FIRE)},
	
	// special effect monsters
	{0, "spectral blade",WEAPON_CHAR, &spectralBladeColor,0,1,	0,		150,	{1, 1, 1},		0,	50,		50,		50,		100,	NOTHING,	SPECTRAL_BLADE_LIGHT,0,0,
		(MONST_INANIMATE | MONST_NEVER_SLEEPS | MONST_FLIES | MONST_WILL_NOT_USE_STAIRS | MONST_INTRINSIC_LIGHT | MONST_DOES_NOT_TRACK_LEADER | MONST_DIES_IF_NEGATED)},
	{0, "spectral sword",WEAPON_CHAR, &spectralImageColor, 0, 1,0,		150,	{1, 1, 1},		0,	50,		50,		50,		100,	NOTHING,	SPECTRAL_IMAGE_LIGHT,0,0,
		(MONST_INANIMATE | MONST_NEVER_SLEEPS | MONST_FLIES | MONST_WILL_NOT_USE_STAIRS | MONST_INTRINSIC_LIGHT | MONST_DOES_NOT_TRACK_LEADER | MONST_DIES_IF_NEGATED)},
};

#pragma mark Monster words

const monsterWords monsterText[NUMBER_MONSTER_KINDS] = {
	{"A naked adventurer in an unforgiving place, bereft of equipment and confused about the circumstances.",
		"you",
		"studying", "Studying",
		{"hit", {0}}},
	{"The rat is a scavenger of the shallows, perpetually in search of decaying animal matter.",
		"it",
		"gnawing at", "Eating",
		{"scratches", "bites", {0}}},
	{"The kobold is a lizardlike humanoid of the upper dungeon.",
		"it",
		"poking at", "Examining",
		{"clubs", "bashes", {0}}},
	{"The jackal prowls the caverns for intruders to rend with its powerful jaws.",
		"it",
		"tearing at", "Eating",
		{"claws", "bites", "mauls", {0}}},
	{"The eel slips silently through the subterranean lake, waiting for unsuspecting prey to set foot in its dark waters.",
		"it",
		"eating", "Eating",
		{"shocks", "bites", {0}}},
	{"Mischievous trickster that it is, the monkey lives to steal shiny trinkets from adventurers -- though it will never lay a hand on cursed items.",
		"it",
		"examining", "Examining",
		{"tweaks", "bites", "punches", {0}}},
	{"A bladder of deadly gas buoys the bloat through the air, its thin veinous membrane ready to rupture at the slightest stress.",
		"it",
		"gazing at", "Gazing",
		{"bumps", {0}},
		"bursts, leaving behind an expanding cloud of caustic gas!"},
	{"A filthy little primate, the tribalistic goblin often travels in packs and carries a makeshift stone blade.",
		"it",
		"chanting over", "Chanting",
		{"slashes", "cuts", "stabs", {0}}},
	{"This goblin is covered with glowing sigils that pulse with power. It can call into existence phantom blades to attack its foes.",
		"it",
		"performing a ritual on", "Performing ritual",
		{"thumps", "whacks", "wallops", {0}},
		{0},
		"gestures ominously!"},
	{"Goblins have created this makeshift totem and imbued it with a shamanistic power.",
		"it",
		"gazing at", "Gazing",
		{"hits", {0}}},
	{"This mass of poisonous pink goo slips across the ground in search of a warm meal.",
		"it",
		"absorbing", "Feeding",
		{"smears", "slimes", "drenches"}},
	{"The enormous, warty toad secretes a powerful hallucinogenic slime to befuddle the senses of any creatures that come in contact with it.",
		"it",
		"eating", "Eating",
		{"slimes", "slams", {0}}},
	{"Often hunting in packs, leathery wings and keen senses guide the vampire bat unerringly to its prey.",
		"it",
		"draining", "Feeding",
		{"nips", "bites", {0}}},
	{"A mechanical contraption embedded in the wall, the spring-loaded arrow turret will fire volley after volley of arrows at intruders.",
		"it",
		"gazing at", "Gazing",
		{"shoots", {0}}},
	{"The acid mound squelches softly across the ground, leaving a trail of acidic goo in its path.",
		"it",
		"liquefying", "Feeding",
		{"slimes", "douses", "drenches", {0}}},
	{"This monstrous centipede's incisors are imbued with a horrible venom that will slowly kill its prey.",
		"it",
		"eating", "Eating",
		{"pricks", "stings", {0}}},
	{"This lumbering creature carries an enormous club that it can swing with incredible force.",
		"he",
		"examining", "Studying",
		{"cudgels", "clubs", "batters", {0}}},
	{"The horrifying bog monster dwells beneath the surface of mud-filled swamps. When its prey ventures into the mud, the bog monster will ensnare the unsuspecting victim with its pale tentacles and squeeze its life away.",
		"it",
		"draining", "Feeding",
		{"squeezes", "strangles", "crushes", {0}}},
	{"Ancient ogres versed in the eldritch arts have assembled this totem and imbued it with occult power.",
		"it",
		"gazing at", "Gazing",
		{"hits", {0}}},
	{"The spider's red eyes pierce the darkness in search of enemies to ensnare with its projectile webs and dissolve with deadly poison.",
		"it",
		"draining", "Feeding",
		{"bites", "stings", {0}}},
	{"This contraption pulses with electrical charge that its embedded crystals and magical sigils can direct at intruders in deadly arcs.",
		"it",
		"gazing at", "Gazing",
		{"shocks", {0}}},
	{"An ethereal blue flame dances through the air, flickering and pulsing in time to an otherworldly beat.",
		"it",
		"consuming", "Feeding",
		{"scorches", "burns", {0}}},
	{"The wraith's hollow eye sockets stare hungrily at the world from its emaciated frame, and its long, bloodstained nails grope ceaselessly at the air for a fresh victim.",
		"it",
		"devouring", "Feeding",
		{"clutches", "claws", "bites", {0}}},
	{"The zombie is the accursed product of a long-forgotten ritual. Its perpetually decaying flesh releases a putrid and flammable stench that will induce violent nausea in any who inhale it. ",
		"it",
		"rending", "Eating",
		{"hits", "bites", {0}}},
	{"An enormous, disfigured creature covered in phlegm and warts, the troll regenerates very quickly and attacks with astonishing strength. Many adventures have ended in its misshapen hands.",
		"he",
		"eating", "Eating",
		{"cudgels", "clubs", "bludgeons", "pummels", "batters"}},
	{"This ogre is bent with age, but what it has lost in physical strength, it has more than gained in occult power.",
		"he",
		"performing a ritual on", "Performing ritual",
		{"cudgels", "clubs", {0}},
		{0},
		"chants in a harsh, gutteral tongue!"},
	{"The serpentine naga live beneath the subterranean waters and emerge to attack unsuspecting adventurers.",
		"she",
		"studying", "Studying",
		{"claws", "bites", "tail-whips", {0}}},
	{"A serpent wreathed in flames and carrying a burning lash, salamanders dwell in lakes of fire and emerge when they sense a nearby victim, leaving behind a trail of glowing embers.",
		"it",
		"studying", "Studying",
		{"claws", "whips", "lashes", {0}}},
	{"This rare subspecies of bloat is little more than a thin membrane surrounding a bladder of highly explosive gases. The slightest stress will cause it to rupture in spectacular and deadly fashion.",
		"it",
		"gazing at", "Gazing",
		{"bumps", {0}},
		"detonates with terrifying force!"},
	{"An elf of the deep, the dar blademaster leaps toward his enemies with frightening speed to engage in deadly swordplay.",
		"he",
		"studying", "Studying",
		{"grazes", "cuts", "slices", "slashes", "stabs"}},
	{"The dar priestess carries a host of religious relics that jangle as she walks and glow faintly with arcane power.",
		"she",
		"praying over", "Praying",
		{"cuts", "slices", {0}}},
	{"The dar battlemage's eyes glow an angry shade of red, and her hands radiate an occult heat.",
		"she",
		"transmuting", "Transmuting",
		{"cuts", {0}}},
	{"Half man and half horse, the centaur carries an enormous bow and quiver of arrows -- hunter and steed fused into a single creature.",
		"he",
		"studying", "Studying",
		{"shoots", {0}}},
	{"A green-flecked nozzle is embedded in the wall, ready to spew a stream of corrosive acid at intruders.",
		"he",
		"gazing at", "Gazing",
		{"douses", "drenches", {0}}},
	{"This tentacled nightmare will emerge from the subterranean waters to ensnare and devour any creature foolish enough to set foot into its lake.",
		"it",
		"devouring", "Feeding",
		{"slaps", "smites", "batters", {0}}},
	{"The desiccated form of a long-dead sorcerer animated by terrible rage and lust for power, the lich commands the obedience of the infernal planes and their foul inhabitants.",
		"it",
		"enchanting", "Enchanting",
		{"touches", {0}},
		{0},
		"rasps a terrifying incantation!"},
	{"A peculiar airborne nymph, the pixie can cause all manner of trouble with a variety of spells. What it lacks in physical endurance, it makes up for with its wealth of mischievous magical abilities.",
		"she",
		"sprinkling dust on", "Dusting",
		{"pokes", {0}}},
	{"A silhouette of mournful rage against an empty backdrop, the phantom slips through the dungeon invisibly in clear air, leaving behind glowing droplets of ectoplasm and the terrified cries of its unsuspecting victims.",
		"it",
		"permeating", "Permeating",
		{"hits", {0}}},
	{"This infernal contraption spits blasts of flame at intruders.",
		"it",
		"incinerating", "Incinerating",
		{"pricks", {0}}},
	{"This trickster demon moves with astonishing speed and delights in stealing from its enemies and teleporting away.",
		"it",
		"dissecting", "Dissecting",
		{"slices", "cuts", {0}}},
	{"A creature of inchoate rage made flesh, the fury's moist wings beat loudly in the darkness.",
		"she",
		"flagellating", "Flagellating",
		{"drubs", "fustigates", "castigates", {0}}},
	{"This unholy specter from the abyss stalks the deep places of the earth without fear, impervious to all conventional attacks.",
		"it",
		"desecrating", "Desecrating",
		{"hits", {0}}},
	{"This seething, towering nightmare of fleshy tentacles slinks through the bowels of the world. Its incredible strength and regeneration make it one of the most fearsome creatures of the dungeon.",
		"it",
		"sucking on", "Consuming",
		{"slaps", "batters", "crushes", {0}}},
	{"A statue animated by a tireless and ancient magic, the golem does not regenerate and attacks with only moderate strength, but its stone form can sustain an incredible amount of damage before being reduced to rubble.",
		"it",
		"cradling", "Cradling",
		{"backhands", "punches", "kicks", {0}}},
	{"An ancient serpent of the world's deepest places, the dragon's immense form belies its lightning-quick speed and testifies to its breathtaking strength. An undying furnace of white-hot flames burns within its scaly hide, and few could withstand a single moment under its infernal lash.",
		"it",
		"consuming", "Consuming",
		{"claws", "bites", {0}}},
	
	{"Taller, stronger and smarter than other goblins, the chieftan commands the loyalty of its kind and can summon them into battle.",
		"it",
		"chanting over", "Chanting",
		{"slashes", "cuts", "stabs", {0}},
		{0},
		"lets loose a deafening war cry!"},
	{"This blob of jet-black goo is as rare as it is deadly. Few creatures of the dungeon can withstand its poisonous assault. Beware.",
		"it",
		"absorbing", "Feeding",
		{"smears", "slimes", "drenches"}},
	{"This vampire lives a solitary life deep underground, consuming any warm-blooded creature unfortunate to venture near his lair.",
		"he",
		"draining", "Drinking",
		{"grazes", "bites", "buries his fangs in", {0}},
		{0},
		"spreads his cape and bursts into a cloud of bats!"},
	{"An elemental creature from another plane of existence, the infernal flamedancer burns with such intensity that it is painful to behold.",
		"it",
		"immolating", "Consuming",
		{"singes", "burns", "immolates", {0}}},
	
	{"Eldritch forces have coalesced to form this flickering, ethereal weapon.",
		"it",
		"gazing at", "Gazing",
		{"nicks",  {0}}},
	{"Mysterious energies bound up in your equipment have leapt forth to project this spectral image.",
		"it",
		"gazing at", "Gazing",
		{"hits",  {0}}},
};

#pragma mark Horde definitions

const hordeType hordeCatalog[NUMBER_HORDES] = {
	// leader		#members	member list								member numbers					minL	maxL	freq	spawnsIn	flags
	{MK_RAT,			0,		{0},									{{0}},							1,		5,		10},
	{MK_KOBOLD,			0,		{0},									{{0}},							1,		6,		10},
	{MK_JACKAL,			0,		{0},									{{0}},							1,		7,		10},
	{MK_EEL,			0,		{0},									{{0}},							2,		17,		10,		DEEP_WATER},
	{MK_MONKEY,			0,		{0},									{{0}},							2,		9,		10},
	{MK_BLOAT,			0,		{0},									{{0}},							2,		13,		3},
	{MK_BLOAT,			1,		{MK_BLOAT},								{{0, 2, 1}},					14,		26,		3},
	{MK_EXPLOSIVE_BLOAT,0,		{0},									{{0}},							10,		26,		1},
	{MK_GOBLIN,			0,		{0},									{{0}},							3,		10,		10},
	{MK_GOBLIN_CONJURER,0,		{0},									{{0}},							3,		10,		6},
	{MK_TOAD,			0,		{0},									{{0}},							4,		11,		10},
	{MK_PINK_JELLY,		0,		{0},									{{0}},							4,		13,		10},
	{MK_GOBLIN_TOTEM,	1,		{MK_GOBLIN},							{{2,4,1}},						5,		13,		10,		0,			NO_PERIODIC_SPAWN},
	{MK_VAMPIRE_BAT,	1,		{MK_VAMPIRE_BAT},						{{0,2,1}},						5,		12,		10},
	{MK_ARROW_TURRET,	0,		{0},									{{0}},							5,		13,		10,		TOP_WALL,	NO_PERIODIC_SPAWN},
	{MK_MONKEY,			0,		{0},									{{0}},							5,		12,		10},
	{MK_MONKEY,			1,		{MK_MONKEY},							{{2,4,1}},						5,		13,		3},
	{MK_ACID_MOUND,		0,		{0},									{{0}},							6,		13,		10},
	{MK_GOBLIN,			3,		{MK_GOBLIN, MK_KOBOLD, MK_JACKAL},		{{1, 3, 1}, {1,2,1}, {1,2,1}},	6,		12,		4},
	{MK_CENTIPEDE,		0,		{0},									{{0}},							7,		14,		10},
	{MK_BOG_MONSTER,	0,		{0},									{{0}},							7,		14,		8,		MUD},
	{MK_OGRE,			0,		{0},									{{0}},							8,		13,		10},
	{MK_EEL,			1,		{MK_EEL},								{{2, 4, 1}},					8,		22,		7,		DEEP_WATER},
	{MK_ACID_MOUND,		1,		{MK_ACID_MOUND},						{{2, 4, 1}},					9,		13,		3},
	{MK_SPIDER,			0,		{0},									{{0}},							9,		16,		10},
	{MK_DAR_BLADEMASTER,1,		{MK_DAR_BLADEMASTER},					{{0, 1, 1}},					10,		14,		10},
	{MK_WILL_O_THE_WISP,0,		{0},									{{0}},							10,		17,		10},
	{MK_WRAITH,			0,		{0},									{{0}},							10,		17,		10},
	{MK_SPARK_TURRET,	0,		{0},									{{0}},							11,		18,		10,		TOP_WALL,	NO_PERIODIC_SPAWN},
	{MK_ZOMBIE,			0,		{0},									{{0}},							11,		18,		10},
	{MK_TROLL,			0,		{0},									{{0}},							12,		19,		10},
	{MK_OGRE_TOTEM,		1,		{MK_OGRE},								{{2,4,1}},						12,		19,		6,		0,			NO_PERIODIC_SPAWN},
	{MK_BOG_MONSTER,	1,		{MK_BOG_MONSTER},						{{2,4,1}},						12,		26,		10,		MUD},
	{MK_NAGA,			0,		{0},									{{0}},							13,		20,		10,		DEEP_WATER},
	{MK_SALAMANDER,		0,		{0},									{{0}},							13,		20,		10,		LAVA},
	{MK_OGRE_SHAMAN,	1,		{MK_OGRE},								{{1, 3, 1}},					14,		20,		10},
	{MK_CENTAUR,		1,		{MK_CENTAUR},							{{1, 1, 1}},					14,		21,		10},
	{MK_ACID_TURRET,	0,		{0},									{{0}},							15,		22,		10,		TOP_WALL,	NO_PERIODIC_SPAWN},
	{MK_PIXIE,			0,		{0},									{{0}},							14,		21,		8},
	{MK_DAR_BLADEMASTER,2,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS},	{{0, 1, 1}, {0, 1, 1}},			15,		17,		10},
	{MK_KRAKEN,			0,		{0},									{{0}},							15,		30,		10,		DEEP_WATER},
	{MK_PHANTOM,		0,		{0},									{{0}},							16,		23,		10},
	{MK_WRAITH,			1,		{MK_WRAITH},							{{1, 4, 1}},					16,		23,		8},
	{MK_FLAME_TURRET,	0,		{0},									{{0}},							17,		24,		10,		TOP_WALL,	NO_PERIODIC_SPAWN},
	{MK_IMP,			0,		{0},									{{0}},							17,		24,		10},
	{MK_DAR_BLADEMASTER,3,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE},{{1,2,1},{1,1,1},{1,1,1}},18,25,10},
	{MK_FURY,			1,		{MK_FURY},								{{2, 4, 1}},					18,		26,		8},
	{MK_REVENANT,		0,		{0},									{{0}},							19,		27,		10},
	{MK_GOLEM,			0,		{0},									{{0}},							21,		30,		10},
	{MK_TENTACLE_HORROR,0,		{0},									{{0}},							22,		40,		10},
	{MK_LICH,			0,		{0},									{{0}},							23,		45,		10},
	{MK_DRAGON,			0,		{0},									{{0}},							24,		50,		7},
	{MK_DRAGON,			1,		{MK_DRAGON},							{{1,1,1}},						27,		50,		3},
	{MK_GOLEM,			3,		{MK_GOLEM, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE}, {{1, 2, 1}, {0,1,1},{0,1,1}},27,100,	8},
	{MK_GOLEM,			1,		{MK_GOLEM},								{{5, 10, 2}},					30,		100,	2},
	{MK_TENTACLE_HORROR,2,		{MK_TENTACLE_HORROR, MK_REVENANT},		{{1, 3, 1}, {2, 4, 1}},			32,		100,	2},
	{MK_DRAGON,			1,		{MK_DRAGON},							{{3, 5, 1}},					34,		100,	2},
	
	// summons
	{MK_GOBLIN_CONJURER,1,		{MK_SPECTRAL_BLADE},					{{3, 5, 1}},					0,		0,		10,		0,			HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},
	{MK_OGRE_SHAMAN,	1,		{MK_OGRE},								{{1, 1, 1}},					0,		0,		10,		0,			HORDE_IS_SUMMONED},
	{MK_VAMPIRE,		1,		{MK_VAMPIRE_BAT},						{{3, 3, 1}},					0,		0,		10,		0,			HORDE_IS_SUMMONED},
	{MK_LICH,			1,		{MK_PHANTOM},							{{2, 3, 1}},					0,		0,		10,		0,			HORDE_IS_SUMMONED},
	{MK_LICH,			1,		{MK_FURY},								{{2, 3, 1}},					0,		0,		10,		0,			HORDE_IS_SUMMONED},
	{MK_GOBLIN_CHIEFTAN,2,		{MK_GOBLIN_CONJURER, MK_GOBLIN},		{{1,1,1}, {2,3,1}},				0,		0,		10,		0,			HORDE_IS_SUMMONED},
	
	// captives
	{MK_MONKEY,			1,		{MK_KOBOLD},							{{1, 2, 1}},					1,		5,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_GOBLIN,			1,		{MK_GOBLIN},							{{1, 2, 1}},					3,		7,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_OGRE,			1,		{MK_GOBLIN},							{{3, 5, 1}},					4,		10,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_OGRE,			1,		{MK_OGRE},								{{1, 2, 1}},					8,		15,		2,		0,			HORDE_LEADER_CAPTIVE},
	{MK_TROLL,			1,		{MK_TROLL},								{{1, 2, 1}},					12,		19,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_CENTAUR,		1,		{MK_TROLL},								{{1, 2, 1}},					12,		19,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_TROLL,			2,		{MK_OGRE, MK_OGRE_SHAMAN},				{{2, 3, 1}, {0, 1, 1}},			14,		19,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_DAR_BLADEMASTER,1,		{MK_TROLL},								{{1, 2, 1}},					12,		19,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_NAGA,			1,		{MK_SALAMANDER},						{{1, 2, 1}},					13,		20,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_SALAMANDER,		1,		{MK_NAGA},								{{1, 2, 1}},					13,		20,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_TROLL,			1,		{MK_SALAMANDER},						{{1, 2, 1}},					13,		19,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_IMP,			1,		{MK_FURY},								{{2, 4, 1}},					18,		26,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_PIXIE,			1,		{MK_IMP, MK_PHANTOM},					{{1, 2, 1}, {1, 2, 1}},			14,		21,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_DAR_BLADEMASTER,1,		{MK_FURY},								{{2, 4, 1}},					18,		26,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_DAR_BLADEMASTER,1,		{MK_IMP},								{{2, 3, 1}},					18,		26,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_DAR_PRIESTESS,	1,		{MK_FURY},								{{2, 4, 1}},					18,		26,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_DAR_BATTLEMAGE,	1,		{MK_IMP},								{{2, 3, 1}},					18,		26,		1,		0,			HORDE_LEADER_CAPTIVE},
	{MK_TENTACLE_HORROR,3,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE},{{1,2,1},{1,1,1},{1,1,1}},18,25,1,	0,			HORDE_LEADER_CAPTIVE},
	{MK_GOLEM,			3,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE},{{1,2,1},{1,1,1},{1,1,1}},18,25,1,	0,			HORDE_LEADER_CAPTIVE},
	
	// bosses
	{MK_GOBLIN_CHIEFTAN,2,		{MK_GOBLIN, MK_GOBLIN_TOTEM},			{{2,3,1}, {2,2,1}},				2,		10,		5,		0,			HORDE_MACHINE_BOSS},
	{MK_BLACK_JELLY,	0,		{0},									{{0}},							5,		15,		5,		0,			HORDE_MACHINE_BOSS},
	{MK_VAMPIRE,		0,		{0},									{{0}},							10,		100,	5,		0,			HORDE_MACHINE_BOSS},
	{MK_FLAMESPIRIT,	0,		{0},									{{0}},							10,		100,	5,		0,			HORDE_MACHINE_BOSS},
	
	// machine water monsters
	{MK_EEL,			0,		{0},									{{0}},							2,		7,		10,		DEEP_WATER,	HORDE_MACHINE_WATER_MONSTER},
	{MK_EEL,			1,		{MK_EEL},								{{2, 4, 1}},					5,		15,		10,		DEEP_WATER,	HORDE_MACHINE_WATER_MONSTER},
	{MK_KRAKEN,			0,		{0},									{{0}},							12,		100,	10,		DEEP_WATER,	HORDE_MACHINE_WATER_MONSTER},
	{MK_KRAKEN,			1,		{MK_EEL},								{{1, 2, 1}},					12,		100,	8,		DEEP_WATER,	HORDE_MACHINE_WATER_MONSTER},
	
	// dungeon captives -- no captors
	{MK_OGRE,			0,		{0},									{{0}},							1,		5,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_NAGA,			0,		{0},									{{0}},							2,		8,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_TROLL,			0,		{0},									{{0}},							5,		11,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_DAR_BLADEMASTER,0,		{0},									{{0}},							8,		14,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_DAR_PRIESTESS,	0,		{0},									{{0}},							8,		14,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_WRAITH,			0,		{0},									{{0}},							11,		17,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_VAMPIRE,		0,		{0},									{{0}},							14,		20,		3,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_GOLEM,			0,		{0},									{{0}},							17,		23,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_TENTACLE_HORROR,0,		{0},									{{0}},							20,		26,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_DRAGON,			0,		{0},									{{0}},							23,		26,		10,		0,			HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	
	// machine statue monsters
	{MK_WRAITH,			0,		{0},									{{0}},							10,		17,		10,		STATUE_DORMANT, HORDE_MACHINE_STATUE},
	{MK_NAGA,			0,		{0},									{{0}},							12,		19,		10,		STATUE_DORMANT, HORDE_MACHINE_STATUE},
	{MK_TROLL,			0,		{0},									{{0}},							14,		21,		10,		STATUE_DORMANT, HORDE_MACHINE_STATUE},
	{MK_GOLEM,			0,		{0},									{{0}},							21,		30,		10,		STATUE_DORMANT, HORDE_MACHINE_STATUE},
	
	// machine turrets
	{MK_ARROW_TURRET,	0,		{0},									{{0}},							5,		13,		10,		TURRET_DORMANT,	HORDE_MACHINE_TURRET},
	{MK_SPARK_TURRET,	0,		{0},									{{0}},							11,		18,		10,		TURRET_DORMANT,	HORDE_MACHINE_TURRET},
	{MK_ACID_TURRET,	0,		{0},									{{0}},							15,		22,		10,		TURRET_DORMANT,	HORDE_MACHINE_TURRET},
	{MK_FLAME_TURRET,	0,		{0},									{{0}},							17,		24,		10,		TURRET_DORMANT,	HORDE_MACHINE_TURRET},
};

// LEVELS

const levelProfile levelProfileCatalog[NUMBER_LEVEL_PROFILES] = {
	//cave?		cross?	corrid?	door?	maxRms	maxLoops
	{33,		100,	80,		60,		99,		30},
};

// ITEMS

#pragma mark Item flavors

char itemTitles[NUMBER_SCROLL_KINDS][30];

const char titlePhonemes[NUMBER_TITLE_PHONEMES][30] = {
	"glorp",
	"snarg",
	"gana",
	"flin",
	"herba",
	"pora",
	"nuglo",
	"greep",
	"nur",
	"lofa",
	"poder",
	"nidge",
	"pus",
	"wooz",
	"flem",
	"bloto",
	"porta"
};

char itemColors[NUMBER_ITEM_COLORS][30];

const char itemColorsRef[NUMBER_ITEM_COLORS][30] = {
	"crimson",
	"scarlet",
	"orange",
	"yellow",
	"green",
	"blue",
	"indigo",
	"violet",
	"puce",
	"mauve",
	"burgundy",
	"turquoise",
	"aquamarine",
	"gray",
	"pink",
	"white",
	"lavender",
	"tan",
	"brown",
	"cyan",
	"black"
};

char itemWoods[NUMBER_ITEM_WOODS][30];

const char itemWoodsRef[NUMBER_ITEM_WOODS][30] = {
	"teak",
	"oak",
	"redwood",
	"rowan",
	"willow",
	"mahogany",
	"pine",
	"maple",
	"bamboo",
	"ironwood",
	"pearwood",
	"birch",
	"cherry",
	"eucalyptus",
	"walnut",
	"cedar",
	"rosewood",
	"yew"
};

char itemMetals[NUMBER_ITEM_METALS][30];

const char itemMetalsRef[NUMBER_ITEM_METALS][30] = {
	"bronze",
	"steel",
	"brass",
	"pewter",
	"nickel",
	"copper",
	"aluminum",
	"tungsten",
	"titanium",
};

char itemGems[NUMBER_ITEM_GEMS][30];

const char itemGemsRef[NUMBER_ITEM_GEMS][30] = {
	"diamond",
	"opal",
	"garnet",
	"ruby",
	"amethyst",
	"topaz",
	"onyx",
	"tourmaline",
	"sapphire",
	"obsidian",
	"malachite",
	"aquamarine",
	"emerald",
	"jade",
	"alexandrite",
	"agate",
	"bloodstone",
	"jasper"
};

#pragma mark Item definitions

//typedef struct itemTable {
//	char *name;
//	char *flavor;
//	short frequency;
//	short marketValue;
//	short number;
//	randomRange range;
//} itemTable;

const itemTable foodTable[NUMBER_FOOD_KINDS] = {
	{"ration of food",	"", "", 3, 25,	1800, {0,0,0}, true, false, "A ration of food. Was it left by former adventurers? Is it a curious byproduct of the subterranean ecosystem?"},
	{"mango",			"", "", 1, 15,	1550, {0,0,0}, true, false, "An odd fruit to be found so deep beneath the surface of the earth, but only slightly less filling than a ration of food."}
};

const itemTable weaponTable[NUMBER_WEAPON_KINDS] = {
	{"dagger",				"", "", 10, 190,		10,	{3,	4,	1},		true, false, "A simple iron dagger with a well-worn wooden handle."},
	{"sword",				"", "", 10, 440,		14, {6,	10,	1},		true, false, "The razor-sharp length of steel blade shines reassuringly."},
	{"broadsword",			"", "", 10, 990,		19,	{14, 22, 1},	true, false, "This towering blade inflicts heavy damage by investing its heft into every cut."},
	
	{"mace",				"", "", 10, 660,		16, {18, 30, 1},	true, false, "The symmetrical iron flanges at the head of this weapon inflict substantial damage, but it requires two turns to attack because of its weight."},
	{"war hammer",			"", "", 10, 1100,		20, {30, 50, 1},	true, false, "Few creatures can withstand the crushing blow of this towering mass of lead and steel, but only the strongest of adventurers can use it effectively, and it requires two turns to attack because of its weight."},
	
	{"spear",				"", "", 10, 330,		13, {4, 5, 1},		true, false, "A slender wooden rod tipped with sharpened iron. The reach of the spear permits you to simultaneously attack an adjacent enemy and the enemy directly behind it."},
	{"war pike",			"", "", 10, 880,		18, {9, 15, 1},		true, false, "A long steel pole ending in a razor-sharp point. The reach of the pike permits you to simultaneously attack an adjacent enemy and the enemy directly behind it."},
	
	{"axe",					"", "", 10, 550,		15, {6, 9, 1},		true, false, "The blunt iron edge on this axe glints in the darkness. The arc of its swing permits you to attack all adjacent enemies simultaneously."},
	{"war axe",				"", "", 10, 990,		19, {10, 17, 1},	true, false, "The enormous steel head of this war axe puts considerable heft behind each stroke. The arc of its swing permits you to attack all adjacent enemies simultaneously."},

	{"dart",				"", "",	0,	15,			10,	{2,	4,	1},		true, false, "These simple metal spikes are weighted to fly true and sting their prey with a flick of the wrist."},
	{"incendiary dart",		"", "",	10, 25,			12,	{1,	2,	1},		true, false, "The spike on each of these darts is designed to pin it to its target while the unstable compounds strapped to its length burst into brilliant flames."},
	{"javelin",				"", "",	10, 40,			15,	{3, 11, 3},		true, false, "This length of metal is weighted to keep the spike at its tip foremost as it sails through the air. "},
};

const itemTable armorTable[NUMBER_ARMOR_KINDS] = {
	{"leather armor",	"", "", 10,	250,		10,	{30,30,0},		true, false, "This lightweight armor offers basic protection."},
	{"scale mail",		"", "", 10, 350,		12, {40,40,0},		true, false, "Bronze scales cover the surface of treated leather, offering greater protection than plain leather with minimal additional weight."},
	{"chain mail",		"", "", 10, 500,		13, {50,50,0},		true, false, "Interlocking metal links make for a tough but flexible suit of armor."},
	{"banded mail",		"", "", 10, 800,		15, {70,70,0},		true, false, "Overlapping strips of metal horizontally encircle a chain mail base, offering an additional layer of protection at the cost of greater weight."},
	{"splint mail",		"", "", 10, 1000,		17, {90,90,0},		true, false, "Thick plates of metal are embedded into a chain mail base, providing the wearer with substantial protection."},
	{"plate armor",		"", "", 10, 1300,		19, {120,120,0},	true, false, "Emormous plates of metal are joined together into a suit that provides unmatched protection to any adventurer strong enough to bear its staggering weight."}
};

const char weaponRunicNames[NUMBER_WEAPON_RUNIC_KINDS][30] = {
	"speed",
	"quietus",
	"paralysis",
	"multiplicity",
	"slowing",
	"confusion",
	"slaying",
	"mercy",
	"plenty"
};

const char armorRunicNames[NUMBER_ARMOR_ENCHANT_KINDS][30] = {
	"multiplicity",
	"mutuality",
	"absorption",
	"reprisal",
	"immunity",
	"reflection",
	"burden",
	"vulnerability"
};

itemTable scrollTable[NUMBER_SCROLL_KINDS] = {
	{"identify",			itemTitles[0], "",	30,	300,	0,{0,0,0}, false, false, "Permanently reveals all of the secrets of a single item."},
	{"teleportation",		itemTitles[1], "",	10,	500,	0,{0,0,0}, false, false, "The spell on this parchment instantly transports the reader to a random location on the dungeon level. It can be used to escape a dangerous situation, but the unlucky reader might find himself in an even more dangerous place."},
	{"remove curse",		itemTitles[2], "",	15,	150,	0,{0,0,0}, false, false, "The incantation on this scroll will instantly strip from the reader's weapon, armor, rings and carried items any evil enchantments that might prevent the wearer from removing them."},
	{"enchanting",			itemTitles[3], "",	0,	550,	0,{0,0,0}, false, false, "This indispensable scroll will imbue a single item with a powerful and permanent magical charge. A staff will increase in power and in number of charges; a weapon will inflict more damage or find its mark more frequently; a suit of armor will deflect additional blows; the effect of a ring on its wearer will intensify; and a wand will gain a single expendable charge. Weapons and armor will also require less strength to use."}, // frequency is dynamically adjusted
	{"recharging",			itemTitles[4], "",	12,	375,	0,{0,0,0}, false, false, "The raw magical power bound up in this parchment will, when released, recharge all of the reader's staffs to full power and will add a charge to each wand."},
	{"protect armor",		itemTitles[5], "",	10,	400,	0,{0,0,0}, false, false, "The armor worn by the reader of this scroll will be permanently proofed against degradation from acid."},
	{"protect weapon",		itemTitles[6], "",	10,	400,	0,{0,0,0}, false, false, "The weapon held by the reader of this scroll will be permanently proofed against degradation from acid."},
	{"magic mapping",		itemTitles[7], "",	12,	500,	0,{0,0,0}, false, false, "When this scroll is read, a purple-hued image of crystal clarity will be etched into the reader's memory, alerting the reader to the precise layout of the level and revealing all hidden secrets, though the locations of items and creatures will remain unknown."},
	{"cause fear",			itemTitles[10], "",	10,	500,	0,{0,0,0}, false, false, "A flash of red light will overwhelm all creatures in your field of view with terror, and they will turn and flee. Attacking a fleeing enemy will dispel the effect, and even fleeing creatures will turn to fight when they are cornered. The magic is powerful and will forever sweep away the loyalty of any allies caught within its blast."},
	{"negation",			itemTitles[11], "",	10,	400,	0,{0,0,0}, false, false, "This scroll contains a powerful anti-magic. When it is released, all creatures (including yourself) and all items lying on the ground within your field of view will be exposed to its blast and stripped of magic -- and creatures animated purely by magic will die. Potions, scrolls, items being held by other creatures and items in your inventory will not be affected."},
	{"aggravate monsters",	itemTitles[8], "",	15,	50,		0,{0,0,0}, false, false, "When read aloud, this scroll will unleash a piercing shriek that will awaken all monsters and alert them to the reader's location."},
	{"summon monsters",		itemTitles[9], "",	10,	50,		0,{0,0,0}, false, false, "The incantation on this scroll will call out to creatures in other planes of existence, drawing them through the fabric of reality to confront the reader."},
};

itemTable potionTable[NUMBER_POTION_KINDS] = {
	{"healing",				itemColors[1], "",	45,	500,	0,{0,0,0}, false, false, "An elixir that will instantly return you to full health and cure confusion, nausea, poison, magical slowness, magical darkness and hallucination."},
	{"gain level",			itemColors[2], "",	4,	850,	0,{0,0,0}, false, false, "The storied experiences of a multitude of battles fought reduced to liquid form, this draught will instantly raise your experience level."},
	{"telepathy",			itemColors[3], "",	20,	350,	0,{0,0,0}, false, false, "After drinking this, your mind will become attuned to the psychic signature of distant creatures, enabling you to sense biological presences through walls. Its effects will not reveal inanimate objects, such as totems, turrets and traps."},
	{"levitation",			itemColors[4], "",	15,	250,	0,{0,0,0}, false, false, "Drinking this curious liquid will cause you to hover five to ten feet in the air, able to drift effortlessly over lava, water, chasms and traps. Flames, gases and spiderwebs fill the air, however, and cannot be bypassed while airborne. Creatures submerged beneath the surface of water or mud will be unable to attack you while you levitate."},
	{"detect magic",		itemColors[5], "",	20,	300,	0,{0,0,0}, false, false, "This drink will sensitize your mind to the radiance of magic. Items imbued with helpful enchantments will be marked on the map with a full magical sigil; items corrupted by curses or intended to inflict harm will be marked on the map with a hollow sigil. The Amulet of Yendor, if in the vicinity, will be revealed by its unique aura."},
	{"speed",				itemColors[6], "",	10,	500,	0,{0,0,0}, false, false, "Quaffing the contents of this flask will enable you to move at blinding speed for several minutes."},
	{"fire immunity",		itemColors[7], "",	15,	500,	0,{0,0,0}, false, false, "The only way for a human to experience immunity to fire, a drink of this potion will render you impervious to heat and permit you to wander through fire and lava and ignore otherwise deadly bolts of flame. It will not guard against the concussive impact of an explosion, however."},
	{"strength",			itemColors[8], "",	0,	400,	0,{0,0,0}, false, false, "This powerful liquid will course through your muscles, permanently increasing your strength by one point."}, // frequency is dynamically adjusted
	{"poisonous gas",		itemColors[9], "",	15,	200,	0,{0,0,0}, false, false, "Uncorking or shattering this pressurized glass will cause its contents to explode into a deadly cloud of caustic purple gas. You might choose to fling this potion at distant enemies instead of uncorking it by hand."},
	{"paralysis",			itemColors[10], "",	10, 250,	0,{0,0,0}, false, false, "Upon exposure to open air, the liquid in this flask will vaporize into a numbing pink haze. Anyone who inhales the cloud will be paralyzed instantly, unable to move for some time after the cloud dissipates. This item can be thrown at distant enemies to catch them within the effect of the gas."},
	{"hallucination",		itemColors[11], "",	10,	500,	0,{0,0,0}, false, false, "This flask contains a vicious and long-lasting hallucinogen. Under its dazzling effect, you will wander through a rainbow wonderland, unable to discern the form of any creatures or items you see."},
	{"confusion",			itemColors[12], "",	15,	450,	0,{0,0,0}, false, false, "This unstable chemical will quickly vaporize into a glittering cloud upon contact with open air, causing any creature that inhales it to lose control of the direction of its movements until the effect wears off (although its ability to aim projectile attacks will not be affected). Its vertiginous intoxication can cause creatures and adventurers to careen into one another or into chasms or lava pits, so extreme care should be taken when under its effect. Its contents can be weaponized by throwing the flask at distant enemies."},
	{"incineration",		itemColors[13], "",	15,	500,	0,{0,0,0}, false, false, "This flask contains an unstable compound which will burst violently into flame upon exposure to open air. You might throw the flask at distant enemies -- or into a deep lake, to fill the cavern with scalding steam."},
	{"darkness",			itemColors[14], "",	7,	150,	0,{0,0,0}, false, false, "Drinking this potion will plunge you into darkness. At first, you will be completely blind to anything not illuminated by an independent light source, but over time your vision will regain its former strength. Throwing the potion will create a cloud of supernatural darkness, and enemies will have difficulty seeing or following you if you take refuge under its cover."},
	{"descent",				itemColors[15], "",	15,	500,	0,{0,0,0}, false, false, "When this flask is uncorked or shattered, the fog that seeps out will temporarily cause the ground in the vicinity to vanish."},
	{"creeping death",		itemColors[16], "",	7,	450,	0,{0,0,0}, false, false, "When the cork is popped or the flask is broken, tiny spores will immediately cover the floor and begin to grow a deadly lichen. Anything that steps on the lichen will be poisoned for ten turns by its clinging tendrils, and the lichen will slowly grow to fill the area. Fire is the only way to remove it."},
};

itemTable wandTable[NUMBER_WAND_KINDS] = {
	{"teleportation",	itemMetals[0], "",	1,	800,	0,{2,5,1}, false, false, "A blast from this wand will teleport a creature against its will to a random place on the current level. This can be particularly effective against aquatic or mud-bound creatures, which are helpless on dry land."},
	{"slowness",		itemMetals[2], "",	1,	800,	0,{2,5,1}, false, false, "This wand will cause a creature to move at half its ordinary speed for several turns."},
	{"polymorphism",	itemMetals[4], "",	1,	700,	0,{3,5,1}, false, false, "This mischievous magic can transform any creature into another creature at random. Beware: the tamest of creatures might turn into the most fearsome."},
	{"negation",		itemMetals[6], "",	1,	550,	0,{4,6,1}, false, false, "This powerful anti-magic will strip a creature of a host of magical traits, including flight, invisibility, acidic corrosiveness, telepathy, speed or slowness, hypnosis, magical fear, immunity to physical attack, fire resistance and the ability to blink at will. Spell casters will lose their magical abilities and magical totems will be reduced to decoration."},
	{"domination",		itemMetals[7], "",	1,	1000,	0,{1,2,1}, false, false, "This wand can forever bind an enemy to the caster's will, turning it into a steadfast ally. However, the magic works only against enemies that are near death."},
	{"beckoning",		itemMetals[1], "",	1,	500,	0,{2,4,1}, false, false, "The force of this wand will yank the targeted creature into direct proximity."},
	{"plenty",			itemMetals[3], "",	1,	700,	0,{2,4,1}, false, false, "The creature at the other end of this mischievous bit of metal will be beside itself -- literally! Cloning an enemy is ill-advised, but the effect can be invaluable on a powerful ally."},
	{"invisibility",	itemMetals[5], "",	1,	100,	0,{3,5,1}, false, false, "A charge from this wand will render a creature completely invisible to the naked eye. Only with telepathy or in the silhouette of a thick gas will an observer discern the creature's hazy outline."},
};

itemTable staffTable[NUMBER_STAFF_KINDS] = {
	{"lightning",		itemWoods[0], "",	3,	1300,	0,{2,4,1}, false, false, "This staff conjures forth deadly arcs of electricity, which deal damage to any number of creatures in a straight line."},
	{"firebolt",		itemWoods[1], "",	3,	1300,	0,{2,4,1}, false, false, "This staff unleashes bursts of magical fire. It will ignite flammable terrain, and will damage and burn a creature it hits. Creatures with an immunity to fire will not be affected by the bolt."},
	{"poison",			itemWoods[2], "",	2,	1200,	0,{2,4,1}, false, false, "The vile blast of this twisted bit of wood will imbue its target with a deadly venom. A creature that is poisoned will suffer one point of damage per turn and will not regenerate lost health until the effect ends. The duration of the effect increases exponentially with the level of the staff, and a level 10 staff can fell even a deadly dragon with a single use -- eventually."},
	{"tunneling",		itemWoods[3], "",	2,	900,	0,{2,4,1}, false, false, "Bursts of magic from this staff will pass harmlessly through creatures but will reduce walls and other inanimate obstructions to rubble."},
	{"blinking",		itemWoods[4], "",	2,	1100,	0,{2,4,1}, false, false, "This staff will allow you to teleport in the chosen direction. Creatures and inanimate obstructions will block the teleportation. Be careful around dangerous terrain, as nothing will prevent you from teleporting to a fiery death in a lake of lava."},
	{"entrancement",	itemWoods[5], "",	1,	1000,	0,{2,4,1}, false, false, "This curious staff will send creatures into a deep but temporary trance, in which state they will mindlessly mimic your movements. You can use the effect to cause one creature to attack another, but the spell will be broken if you attack the creature under the effect. Entranced creatures can be commanded to step into lava and other hazardous terrain."},
	{"obstruction",		itemWoods[8], "",	2,	1000,	0,{2,4,1}, false, false, "A mass of impenetrable green crystal will spring forth from the point at which this staff is aimed, obstructing any who wish to move through the affected area and temporarily entombing any who are already there. The crystal will dissolve into the air as time passes. Higher level staffs will create larger obstructions."},
	{"discord",			itemWoods[9], "",	2,	1000,	0,{2,4,1}, false, false, "The purple light that this staff emits will alter the perceptions of all creatures to think the target is their enemy. Strangers and allies alike will turn on an affected creature."},
	{"conjuration",		itemWoods[10],"",	2,	1000,	0,{2,4,1}, false, false, "A flick of this staff summons a number of phantom blades to fight on the user's behalf."},
	{"healing",			itemWoods[6], "",	2,	1100,	0,{2,4,1}, false, false, "The crimson bolts from this staff will heal the injuries of any it hits. This can be an unfortunate result against enemies but can prove useful when aimed at your allies. Unfortunately, you cannot use this or any staff on yourself."},
	{"haste",			itemWoods[7], "",	2,	1000,	0,{2,4,1}, false, false, "The magical bolt from this staff will temporarily double the speed of any monster it hits."},
};

itemTable ringTable[NUMBER_RING_KINDS] = {
	{"clairvoyance",	itemGems[0], "",	1,	900,	0,{1,3,1}, false, false, "Wearing this ring will permit you to see through nearby walls and doors, within a radius determined by the level of the ring. A cursed ring of clairvoyance will blind you to your immediate surroundings."},
	{"stealth",			itemGems[1], "",	1,	800,	0,{1,3,1}, false, false, "Enemies will be less likely to notice you if you wear this ring. Staying motionless and lurking in the shadows will make you even harder to spot. At very high levels, even enemies giving chase may sometimes lose track of you. Cursed rings of stealth will alert enemies who might otherwise not have noticed your presence."},
	{"regeneration",	itemGems[2], "",	1,	750,	0,{1,3,1}, false, false, "This ring increases the body's regenerative properties, allowing one to recover lost health at an accelerated rate. Cursed rings will decrease or even halt one's natural regeneration."},
	{"transference",	itemGems[3], "",	1,	750,	0,{1,3,1}, false, false, "Landing a melee attack while wearing this ring will cause a proportion of the inflicted damage to transfer to benefit of the attacker's own health. Cursed rings will cause you to lose health with each attack you land."},
	{"light",			itemGems[4], "",	1,	600,	0,{1,3,1}, false, false, "This ring subtly enhances your vision, enabling you to see farther in the dimming light of the deeper dungeon levels. It will not make you more visible to enemies."},
	{"awareness",		itemGems[5], "",	1,	700,	0,{1,3,1}, false, false, "Wearing this ring will allow the wearer to notice hidden secrets -- traps and secret doors -- without taking time to search. Cursed rings of awareness will dull your senses, making it harder to notice secrets even when actively searching for them."},
	{"wisdom",			itemGems[6], "",	1,	700,	0,{1,3,1}, false, false, "Your staffs will recharge at an accelerated rate in the energy field that radiates from this ring. Cursed rings of wisdom will instead cause your staffs to recharge more slowly."},
};

#pragma mark Miscellaneous definitions

const color *boltColors[NUMBER_BOLT_KINDS] = {
	&blue,				// teleport other
	&green,				// slow
	&purple,			// polymorph
	&pink,				// negation
	&dominationColor,	// domination
	&beckonColor,		// beckoning
	&rainbow,			// plenty
	&darkBlue,			// invisibility
	&lightningColor,	// lightning
	&fireBoltColor,		// fire
	&poisonColor,		// poison
	&brown,				// tunneling
	&white,				// blinking
	&yellow,			// entrancement
	&forceFieldColor,	// obstruction
	&discordColor,		// discord
	&spectralBladeColor,// conjuration
	&darkRed,			// healing
	&orange,			// haste
};

const short goodItems[NUMBER_GOOD_ITEMS][2] = {
	{WEAPON,	BROADSWORD},
	{WEAPON,	HAMMER},
	{WEAPON,	PIKE},
	{WEAPON,	WAR_AXE},
	{ARMOR,		PLATE_MAIL},
	{STAFF,		STAFF_LIGHTNING},
	{STAFF,		STAFF_FIRE},
	{STAFF,		STAFF_POISON},
	{STAFF,		STAFF_BLINKING},
	{STAFF,		STAFF_OBSTRUCTION},
	{STAFF,		STAFF_DISCORD},
	{RING,		RING_CLAIRVOYANCE},
	{RING,		RING_REGENERATION},
	{RING,		RING_TRANSFERENCE},
	{RING,		RING_LIGHT},
	{RING,		RING_AWARENESS},
};

const char monsterBehaviorFlagDescriptions[32][COLS] = {
	"is invisible",								// MONST_INVISIBLE
	"is an inanimate object",					// MONST_INANIMATE
	"cannot move",								// MONST_IMMOBILE
	"carries an item",							// MONST_CARRY_ITEM_100
	"sometimes carries an item",				// MONST_CARRY_ITEM_25
	"never wanders",							// MONST_ALWAYS_HUNTING
	"flees at low health",						// MONST_FLEES_NEAR_DEATH
	"",											// MONST_ATTACKABLE_THRU_WALLS
	"corrodes weapons when hit",				// MONST_DEFEND_DEGRADE_WEAPON
	"is immune to physical damage",				// MONST_IMMUNE_TO_WEAPONS
	"flies",									// MONST_FLIES
	"moves erratically",						// MONST_FLITS
	"is immune to fire",						// MONST_IMMUNE_TO_FIRE
	"",											// MONST_CAST_SPELLS_SLOWLY
	"cannot be entangled",						// MONST_IMMUNE_TO_WEBS
	"sometimes reflects magic spells",			// MONST_REFLECT_4
	"never sleeps",								// MONST_NEVER_SLEEPS
	"burns unceasingly",						// MONST_FIERY
	"",											// MONST_INTRINSIC_LIGHT
	"is at home in water",						// MONST_IMMUNE_TO_WATER
	"cannot venture onto dry land",				// MONST_RESTRICTED_TO_LIQUID
	"submerges",								// MONST_SUBMERGES
	"keeps its distance",						// MONST_MAINTAINS_DISTANCE
	"",											// MONST_WILL_NOT_USE_STAIRS
	"is animated purely by magic",				// MONST_DIES_IF_NEGATED
};

const char monsterAbilityFlagDescriptions[32][COLS] = {
	"can induce hallucinations",				// MA_HIT_HALLUCINATE
	"can steal items",							// MA_HIT_STEAL_FLEE
	"can possess its summoned allies",			// MA_ENTER_SUMMONS
	"corrodes armor upon hitting",				// MA_HIT_DEGRADE_ARMOR
	"can heal its allies",						// MA_CAST_HEAL
	"can haste its allies",						// MA_CAST_HASTE
	"can summon allies",						// MA_CAST_SUMMON
	"can blink towards enemies",				// MA_CAST_BLINK
	"can cast negation",						// MA_CAST_NEGATION
	"can throw sparks of lightning",			// MA_CAST_SPARK
	"can throw bolts of fire",					// MA_CAST_FIRE
	"can slow its enemies",						// MA_CAST_SLOW
	"can turn friends against one another",		// MA_CAST_DISCORD
	"can breathe gouts of white-hot flame",		// MA_BREATHES_FIRE
	"can launch sticky webs",					// MA_SHOOTS_WEBS
	"attacks from a distance",					// MA_ATTACKS_FROM_DISTANCE
	"immobilizes its prey",						// MA_SEIZES
	"injects poison upon hitting",				// MA_POISONS
	"",											// MA_DF_ON_DEATH
	"divides in two when struck",				// MA_CLONE_SELF_ON_DEFEND
	"dies when attacking",						// MA_KAMIKAZE
	"recovers health when inflicting damage",	// MA_TRANSFERENCE
};

const char monsterBookkeepingFlagDescriptions[32][COLS] = {
	"",											// MONST_WAS_VISIBLE
	"",											// MONST_ADDED_TO_STATS_BAR
	"",											// MONST_PREPLACED
	"",											// MONST_APPROACHING_UPSTAIRS
	"",											// MONST_APPROACHING_DOWNSTAIRS
	"",											// MONST_APPROACHING_PIT
	"",											// MONST_LEADER
	"",											// MONST_FOLLOWER
	"",											// MONST_CAPTIVE
	"has been immobilized",						// MONST_SEIZED
	"is currently holding prey immobile",		// MONST_SEIZING
	"is submerged",								// MONST_SUBMERGED
	"",											// MONST_JUST_SUMMONED
	"",											// MONST_WILL_FLASH
	"is anchored to reality by its summoner",	// MONST_BOUND_TO_LEADER
};
