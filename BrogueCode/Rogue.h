//
//  RogueMain.h
//  Brogue
//
//  Created by Brian Walker on 12/26/08.
//  Copyright 2010. All rights reserved.
//  
//  This file is part of Brogue.
//
//  Brogue is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Brogue is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Brogue.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PlatformDefines.h"

// unicode: comment this line to revert to ASCII

#define USE_UNICODE

// version string -- no more than 16 bytes:
#define BROGUE_VERSION_STRING "1.5"

// debug macros -- define DEBUGGING as 1 to enable debugging.

#define DEBUGGING			0
#define DUNGEON_SEED		0 // for debugging, insert desired level sequence ID instead of 0

#define DEBUG				if (DEBUGGING)
#define MONSTERS_ENABLED	(!DEBUGGING || 0) // Quest room monsters can be generated regardless.
#define ITEMS_ENABLED		(!DEBUGGING || 1)

#define D_BULLET_TIME		(DEBUGGING && 0)
#define D_SAFETY_VISION		(DEBUGGING && 0)
#define D_WORMHOLING		(DEBUGGING && 1)
#define D_IMMORTAL			(DEBUGGING && 1)
#define D_INSPECT_MACHINES	(DEBUGGING && 0)

// set to false to allow multiple loads from the same saved file:
#define DELETE_SAVE_FILE_AFTER_LOADING	true

//#define BROGUE_ASSERTS	// introduces several assert()s -- slower but useful to find certain buffer overruns
//#define AUDIT_RNG		// VERY slow, but sometimes necessary to debug out-of-sync recording errors

#ifdef BROGUE_ASSERTS
#include <assert.h>
#endif

#define boolean					char

#define false					0
#define true					1

#define Fl(N)					(1 << (N))

// recording and save filenames
#define LAST_GAME_PATH			"LastGame.broguesave"
#define RECORDING_SUFFIX		".broguerec"
#define GAME_SUFFIX				".broguesave"
#define ANNOTATION_SUFFIX		".txt"
#define RNG_LOG					"RNGLog.txt"

// Allows unicode characters:
#define uchar					unsigned short

#define MESSAGE_LINES			3

// Size of the entire terminal window. These need to be hard-coded here and in Viewport.h
#define COLS					100			// was 80
#define ROWS					(29 + MESSAGE_LINES)

// Size of the portion of the terminal window devoted to displaying the dungeon:
#define DCOLS					79
#define DROWS					(ROWS - MESSAGE_LINES - 1)	// n lines at the top for messages;
															// one line at the bottom for flavor text.

#define STAT_BAR_WIDTH			20			// number of characters in the stats bar to the left of the map

#define LOS_SLOPE_GRANULARITY	32768		// how finely we divide up the squares when calculating slope;
											// higher numbers mean fewer artifacts but more memory and processing
#define INTERFACE_OPACITY		85

#define MAX_BOLT_LENGTH			DCOLS*10

#define VISIBILITY_THRESHOLD	50			// how bright cumulative light has to be before the cell is marked visible

#define AMULET_LEVEL			26			// how deep before the amulet appears

#define DEFENSE_FACTOR			0.986		// each point of armor multiplies enemies' accuracy by this value
											// (displayed armor value is 10% of the real value)
#define WEAPON_ENCHANT_FACTOR	1.065		// each marginal point of weapon enchantment
											// multiplies accuracy and damage by this factor

#define INPUT_RECORD_BUFFER		1000		// how many bytes of input data to keep in memory before vomiting it
											// to the text file
#define DEFAULT_PLAYBACK_DELAY	50

// color escapes
#define COLOR_ESCAPE			25
#define COLOR_VALUE_INTERCEPT	25

// display characters:

#ifdef USE_UNICODE

#define FLOOR_CHAR		0x00b7
#define WATER_CHAR		'~'
#define CHASM_CHAR		0x2237
#define TRAP_CHAR		0x25c7
#define FIRE_CHAR		0x22CF
#define GRASS_CHAR		'"'
#define BRIDGE_CHAR		'='
#define DOWN_CHAR		'>'
#define UP_CHAR			'<'
#define WALL_CHAR		'#'
#define DOOR_CHAR		'+'
#define OPEN_DOOR_CHAR	'\''
#define ASH_CHAR		'\''
#define BONES_CHAR		','
#define WEB_CHAR		':'
//#define FOLIAGE_CHAR	0x03A8 // lower-case psi
#define FOLIAGE_CHAR	0x2648 // Aries symbol
#define ALTAR_CHAR		'|'
#define STATUE_CHAR		0x00df

#define T_FOLIAGE_CHAR	'"'		// 0x2034 // 0x2037

#define PLAYER_CHAR		'@'

#define AMULET_CHAR		0x2640
#define FOOD_CHAR		':'
#define SCROLL_CHAR		0x266A//'?'		// 0x039E
#define RING_CHAR		0xffee
#define POTION_CHAR		'!'
#define ARMOR_CHAR		'['
#define WEAPON_CHAR		0x2191
#define STAFF_CHAR		'\\'
#define WAND_CHAR		'~'
#define GOLD_CHAR		'*'
#define GEM_CHAR		0x25cf
#define TOTEM_CHAR		0x26b2
#define TURRET_CHAR		0x25cf
#define KEY_CHAR		'-'

#define CHAIN_TOP_LEFT		'\\'
#define CHAIN_BOTTOM_RIGHT	'\\'
#define CHAIN_TOP_RIGHT		'/'
#define CHAIN_BOTTOM_LEFT	'/'
#define CHAIN_TOP			'|'
#define CHAIN_BOTTOM		'|'
#define CHAIN_LEFT			'-'
#define CHAIN_RIGHT			'-'

#define BAD_MAGIC_CHAR		0x29F2
#define GOOD_MAGIC_CHAR		0x29F3

#else

#define FLOOR_CHAR		'.'
#define WATER_CHAR		'~'
#define CHASM_CHAR		':'
#define TRAP_CHAR		'%'
#define FIRE_CHAR		'^'
#define GRASS_CHAR		'"'
#define BRIDGE_CHAR		'='
#define DOWN_CHAR		'>'
#define UP_CHAR			'<'
#define WALL_CHAR		'#'
#define DOOR_CHAR		'+'
#DEFINE OPEN_DOOR_CHAR	'\''
#define ASH_CHAR		'\''
#define BONES_CHAR		','
#define WEB_CHAR		':'
#define FOLIAGE_CHAR	'&'
#define ALTAR_CHAR		'|'
#define STATUE_CHAR		'&'

#define T_FOLIAGE_CHAR	'"'

#define PLAYER_CHAR		'@'

#define AMULET_CHAR		','
#define FOOD_CHAR		':'
#define SCROLL_CHAR		'?'
#define RING_CHAR		'='
#define POTION_CHAR		'!'
#define ARMOR_CHAR		'['
#define WEAPON_CHAR		'('
#define STAFF_CHAR		'\\'
#define WAND_CHAR		'~'
#define GOLD_CHAR		'*'
#define GEM_CHAR		'+'
#define TOTEM_CHAR		'0'
#define TURRET_CHAR		'*'
#define KEY_CHAR		'-'

#define CHAIN_TOP_LEFT		'\\'
#define CHAIN_BOTTOM_RIGHT	'\\'
#define CHAIN_TOP_RIGHT		'/'
#define CHAIN_BOTTOM_LEFT	'/'
#define CHAIN_TOP			'|'
#define CHAIN_BOTTOM		'|'
#define CHAIN_LEFT			'-'
#define CHAIN_RIGHT			'-'

#define BAD_MAGIC_CHAR		'+'
#define GOOD_MAGIC_CHAR		'$'

#endif

enum eventTypes {
	KEYSTROKE,
	MOUSE_UP,
	MOUSE_DOWN, // unused
	MOUSE_ENTERED_CELL,
	RNG_CHECK,
	SAVED_GAME_LOADED,
	END_OF_RECORDING,
	EVENT_ERROR,
	NUMBER_OF_EVENT_TYPES, // unused
};

typedef struct rogueEvent {
	enum eventTypes eventType;
	unsigned short int param1;
	char param2;
	boolean controlKey;
	boolean shiftKey;
} rogueEvent;

typedef struct rogueHighScoresEntry {
	signed long score;
	char date[DCOLS];
	char description[DCOLS];
} rogueHighScoresEntry;


enum RNGs {
	RNG_SUBSTANTIVE,
	RNG_COSMETIC,
	NUMBER_OF_RNGS,
};

enum directions {
	// Cardinal directions; must be 0-3:
	UP				= 0,
	DOWN			= 1,
	LEFT			= 2,
	RIGHT			= 3,
	// Secondary directions; must be 4-7:
	UPLEFT			= 4,
	DOWNLEFT		= 5,
	UPRIGHT			= 6,
	DOWNRIGHT		= 7
};

enum textEntryTypes {
	TEXT_INPUT_NORMAL = 0,
	TEXT_INPUT_NUMBERS,
	TEXT_INPUT_TYPES,
};

#define NUMBER_DYNAMIC_COLORS	6

enum tileType {
	NOTHING = 0,
	GRANITE,
	FLOOR,
	CARPET,
	TOP_WALL,
	BOTTOM_WALL,
	LEFT_WALL,
	RIGHT_WALL,
	PERM_WALL, // e.g. corners of rooms: any tiles that should show up as '#' but cannot be cut into a door.
	DOOR,
	OPEN_DOOR,
	SECRET_DOOR,
	LOCKED_DOOR,
	OPEN_IRON_DOOR_INERT,
	DOWN_STAIRS,
	UP_STAIRS,
	DUNGEON_EXIT,
	TORCH_WALL, // wall lit with a torch
	CRYSTAL_WALL,
	PORTCULLIS,
	MACHINE_BLOCKABLE_DOORWAY,
	ADD_WOODEN_BARRICADE,
	WOODEN_BARRICADE,
	PILOT_LIGHT_DORMANT,
	PILOT_LIGHT,
	STATUE_DORMANT,
	STATUE_CRACKING,
	TURRET_DORMANT,
	DARK_FLOOR_DORMANT,
	DARK_FLOOR_DARKENING,
	DARK_FLOOR,

	GAS_TRAP_POISON_HIDDEN,
	GAS_TRAP_POISON,
	TRAP_DOOR_HIDDEN,
	TRAP_DOOR,
	GAS_TRAP_PARALYSIS_HIDDEN,
	GAS_TRAP_PARALYSIS,
	GAS_TRAP_CONFUSION_HIDDEN,
	GAS_TRAP_CONFUSION,
	FLAMETHROWER_HIDDEN,
	FLAMETHROWER,
	FLOOD_TRAP_HIDDEN,
	FLOOD_TRAP,
	MACHINE_POISON_GAS_VENT_DORMANT,
	MACHINE_POISON_GAS_VENT,
	MACHINE_METHANE_VENT_DORMANT,
	MACHINE_METHANE_VENT,
	
	DEEP_WATER,
	SHALLOW_WATER,
	MUD,
	CHASM,
	CHASM_EDGE,
	MACHINE_COLLAPSE_EDGE_DORMANT,
	MACHINE_COLLAPSE_EDGE_SPREADING,
	CHASM_WITH_HIDDEN_BRIDGE,
	LAVA,
	SUNLIGHT_POOL,
	DARKNESS_PATCH,
	ACTIVE_BRIMSTONE,
	INERT_BRIMSTONE,
	OBSIDIAN,
	BRIDGE,
	BRIDGE_EDGE,
	STONE_BRIDGE,
	MACHINE_FLOOD_WATER_DORMANT,
	MACHINE_FLOOD_WATER_SPREADING,
		
	HOLE,
	HOLE_EDGE,
	FLOOD_WATER_DEEP,
	FLOOD_WATER_SHALLOW,
	GRASS,
	LUMINESCENT_FUNGUS,
	LICHEN,
	HUMAN_BLOOD,
	GREEN_BLOOD,
	PURPLE_BLOOD,
	ACID_SPLATTER,
	VOMIT,
	URINE,
	ASH,
	PUDDLE,
	BONES,
	RUBBLE,
	ECTOPLASM,
	EMBERS,
	SPIDERWEB,
	FOLIAGE,
	TRAMPLED_FOLIAGE,
	FUNGUS_FOREST,
	TRAMPLED_FUNGUS_FOREST,
	FORCEFIELD,
	MANACLE_TL,
	MANACLE_BR,
	MANACLE_TR,
	MANACLE_BL,
	MANACLE_T,
	MANACLE_B,
	MANACLE_L,
	MANACLE_R,
	ALTAR_INERT,
	ALTAR_CAGE_OPEN,
	ALTAR_CAGE_CLOSED,
	ALTAR_SWITCH,
	PEDESTAL,
	
	PLAIN_FIRE,
	BRIMSTONE_FIRE,
	FLAMEDANCER_FIRE,
	GAS_FIRE,
	GAS_EXPLOSION,
	DART_EXPLOSION,
	POISON_GAS,
	CONFUSION_GAS,
	ROT_GAS,
	PARALYSIS_GAS,
	METHANE_GAS,
	STEAM,
	DARKNESS_CLOUD,
	NUMBER_TILETYPES,
	UNFILLED_LAKE = 120	// used to mark lakes not yet assigned a liquid type
};

enum lightType {
	NO_LIGHT,
	MINERS_LIGHT,
	BURNING_CREATURE_LIGHT,
	WISP_LIGHT,
	SALAMANDER_LIGHT,
	IMP_LIGHT,
	PIXIE_LIGHT,
	LICH_LIGHT,
	FLAMEDANCER_LIGHT,
	SPECTRAL_BLADE_LIGHT,
	SPECTRAL_IMAGE_LIGHT,
	SPARK_TURRET_LIGHT,
	BOLT_LIGHT_SOURCE,
	
	TORCH_LIGHT,
	LAVA_LIGHT,
	SUN_LIGHT,
	DARKNESS_PATCH_LIGHT,
	FUNGUS_LIGHT,
	FUNGUS_FOREST_LIGHT,
	ECTOPLASM_LIGHT,
	EMBER_LIGHT,
	FIRE_LIGHT,
	BRIMSTONE_FIRE_LIGHT,
	EXPLOSION_LIGHT,
	INCENDIARY_DART_LIGHT,
	CONFUSION_GAS_LIGHT,
	DARKNESS_CLOUD_LIGHT,
	FORCEFIELD_LIGHT,
	CRYSTAL_WALL_LIGHT,
	CANDLE_LIGHT,
	NUMBER_LIGHT_KINDS
};

#define NUMBER_BLUEPRINTS	17

// Item categories
enum itemCategory {
	FOOD		= Fl(0),
	WEAPON		= Fl(1),
	ARMOR		= Fl(2),
	POTION		= Fl(3),
	SCROLL		= Fl(4),
	STAFF		= Fl(5),
	WAND		= Fl(6),
	RING		= Fl(7),
	GOLD		= Fl(8),
	AMULET		= Fl(9),
	GEM			= Fl(10),
	KEY			= Fl(11),
	ALL_ITEMS	= (FOOD|POTION|WEAPON|ARMOR|STAFF|WAND|SCROLL|RING|GOLD|AMULET|GEM|KEY)
};

enum foodKind {
	RATION,
	FRUIT,
	NUMBER_FOOD_KINDS
};

enum potionKind {
	POTION_HEALING,
	POTION_GAIN_LEVEL,
	POTION_TELEPATHY,
	POTION_LEVITATION,
	POTION_DETECT_MAGIC,
	POTION_HASTE_SELF,
	POTION_FIRE_IMMUNITY,
	POTION_GAIN_STRENGTH,
	POTION_POISON,
	POTION_PARALYSIS,
	POTION_HALLUCINATION,
	POTION_CONFUSION,
	POTION_INCINERATION,
	POTION_DARKNESS,
	POTION_DESCENT,
	POTION_LICHEN,
	NUMBER_POTION_KINDS
};

enum weaponKind {
	DAGGER,
	SWORD,
	BROADSWORD,
	
	MACE,
	HAMMER,
	
	SPEAR,
	PIKE,
	
	AXE,
	WAR_AXE,
	
	DART,
	INCENDIARY_DART,
	JAVELIN,
	NUMBER_WEAPON_KINDS
};

enum weaponEnchants {
	W_SPEED,
	W_QUIETUS,
	W_PARALYSIS,
	W_MULTIPLICITY,
	W_SLOWING,
	W_CONFUSION,
	W_SLAYING,
	W_MERCY,
	NUMBER_GOOD_WEAPON_ENCHANT_KINDS = W_MERCY,
	W_PLENTY,
	NUMBER_WEAPON_RUNIC_KINDS
};

enum armorKind {
	LEATHER_ARMOR,
	SCALE_MAIL,
	CHAIN_MAIL,
	BANDED_MAIL,
	SPLINT_MAIL,
	PLATE_MAIL,
	NUMBER_ARMOR_KINDS
};

enum armorEnchants {
	A_MULTIPLICITY,
	A_MUTUALITY,
	A_ABSORPTION,
	A_REPRISAL,
	A_IMMUNITY,
	A_REFLECTION,
	A_BURDEN,
	NUMBER_GOOD_ARMOR_ENCHANT_KINDS = A_BURDEN,
	A_VULNERABILITY,
	NUMBER_ARMOR_ENCHANT_KINDS,
};

enum wandKind {
	WAND_TELEPORT,
	WAND_SLOW,
	WAND_POLYMORPH,
	WAND_NEGATION,
	WAND_DOMINATION,
	WAND_BECKONING,
	WAND_PLENTY,
	WAND_INVISIBILITY,
	NUMBER_WAND_KINDS
};

enum staffKind {
	STAFF_LIGHTNING,
	STAFF_FIRE,
	STAFF_POISON,
	STAFF_TUNNELING,
	STAFF_BLINKING,
	STAFF_ENTRANCEMENT,
	STAFF_OBSTRUCTION,
	STAFF_DISCORD,
	STAFF_CONJURATION,
	STAFF_HEALING,
	STAFF_HASTE,
	NUMBER_STAFF_KINDS
};

// these must be wand bolts, in order, and then staff bolts, in order:
enum boltType {
	BOLT_TELEPORT,
	BOLT_SLOW,
	BOLT_POLYMORPH,
	BOLT_NEGATION,
	BOLT_DOMINATION,
	BOLT_BECKONING,
	BOLT_PLENTY,
	BOLT_INVISIBILITY,
	BOLT_LIGHTNING,
	BOLT_FIRE,
	BOLT_POISON,
	BOLT_TUNNELING,
	BOLT_BLINKING,
	BOLT_ENTRANCEMENT,
	BOLT_OBSTRUCTION,
	BOLT_DISCORD,
	BOLT_CONJURATION,
	BOLT_HEALING,
	BOLT_HASTE,
	NUMBER_BOLT_KINDS
};

enum ringKind {
	RING_CLAIRVOYANCE,
	RING_STEALTH,
	RING_REGENERATION,
	RING_TRANSFERENCE,
	RING_LIGHT,
	RING_AWARENESS,
	RING_WISDOM,
	NUMBER_RING_KINDS
};

enum scrollKind {
	SCROLL_IDENTIFY,
	SCROLL_TELEPORT,
	SCROLL_REMOVE_CURSE,
	SCROLL_ENCHANT_ITEM,
	SCROLL_RECHARGING,
	SCROLL_PROTECT_ARMOR,
	SCROLL_PROTECT_WEAPON,
	SCROLL_MAGIC_MAPPING,
	SCROLL_CAUSE_FEAR,
	SCROLL_NEGATION,
	SCROLL_AGGRAVATE_MONSTER,
	SCROLL_SUMMON_MONSTER,
	NUMBER_SCROLL_KINDS
};

#define MAX_PACK_ITEMS				26

enum monsterTypes {
	MK_YOU,
	MK_RAT,
	MK_KOBOLD,
	MK_JACKAL,
	MK_EEL,
	MK_MONKEY,
	MK_BLOAT,
	MK_GOBLIN,
	MK_GOBLIN_CONJURER,
	MK_GOBLIN_TOTEM,
	MK_PINK_JELLY,
	MK_TOAD,
	MK_VAMPIRE_BAT,
	MK_ARROW_TURRET,
	MK_ACID_MOUND,
	MK_CENTIPEDE,
	MK_OGRE,
	MK_BOG_MONSTER,
	MK_OGRE_TOTEM,
	MK_SPIDER,
	MK_SPARK_TURRET,
	MK_WILL_O_THE_WISP,
	MK_WRAITH,
	MK_ZOMBIE,
	MK_TROLL,
	MK_OGRE_SHAMAN,
	MK_NAGA,
	MK_SALAMANDER,
	MK_EXPLOSIVE_BLOAT,
	MK_DAR_BLADEMASTER,
	MK_DAR_PRIESTESS,
	MK_DAR_BATTLEMAGE,
	MK_CENTAUR,
	MK_ACID_TURRET,
	MK_KRAKEN,
	MK_LICH,
	MK_PIXIE,
	MK_PHANTOM,
	MK_FLAME_TURRET,
	MK_IMP,
	MK_FURY,
	MK_REVENANT,
	MK_TENTACLE_HORROR,
	MK_GOLEM,
	MK_DRAGON,
	
	MK_GOBLIN_CHIEFTAN,
	MK_BLACK_JELLY,
	MK_VAMPIRE,
	MK_FLAMESPIRIT,
	
	MK_SPECTRAL_BLADE,
	MK_SPECTRAL_IMAGE,
	
	NUMBER_MONSTER_KINDS
};

#define	NUMBER_HORDES				108

// flavors

#define NUMBER_ITEM_COLORS			21
#define NUMBER_TITLE_PHONEMES		17
#define NUMBER_ITEM_WOODS			18
#define NUMBER_POTION_DESCRIPTIONS	18
#define NUMBER_ITEM_METALS			9
#define NUMBER_ITEM_GEMS			18
#define NUMBER_GOOD_ITEMS			16

// Dungeon flags
enum tileFlags {
	DISCOVERED					= 1 << 0,
	VISIBLE						= 1 << 1,	// cell has sufficient light and is in field of view, ready to draw.
	HAS_PLAYER					= 1 << 2,
	HAS_MONSTER					= 1 << 3,
	HAS_ITEM					= 1 << 4,
	IN_FIELD_OF_VIEW			= 1 << 5,	// player has unobstructed line of sight whether or not there is enough light
	WAS_VISIBLE					= 1 << 6,
	HAS_DOWN_STAIRS				= 1 << 7,
	HAS_UP_STAIRS				= 1 << 8,
	IS_IN_SHADOW				= 1 << 9,	// so that a player gains an automatic stealth bonus
	MAGIC_MAPPED				= 1 << 10,
	ITEM_DETECTED				= 1 << 11,
	CLAIRVOYANT_VISIBLE			= 1 << 12,
	WAS_CLAIRVOYANT_VISIBLE		= 1 << 13,
	CLAIRVOYANT_DARKENED		= 1 << 14,	// magical blindness from a cursed ring of clairvoyance
	CAUGHT_FIRE_THIS_TURN		= 1 << 15,	// so that fire does not spread asymmetrically
	PRESSURE_PLATE_DEPRESSED	= 1 << 16,	// so that traps do not trigger repeatedly while you stand on them
	DOORWAY						= 1 << 17,	// so that waypoint paths don't cross open doorways
	STABLE_MEMORY				= 1 << 18,	// redraws will simply be pulled from the memory array, not recalculated
	PLAYER_STEPPED_HERE			= 1 << 19,	// keep track of where the player has stepped as he knows no traps are there
	TERRAIN_COLORS_DANCING		= 1 << 20,
	IN_LOOP						= 1 << 21,	// this cell is part of a terrain loop
	IS_CHOKEPOINT				= 1 << 22,	// if this cell is blocked, part of the map will be rendered inaccessible
	IS_GATE_SITE				= 1 << 23,	// consider placing a locked door here
	IS_IN_MACHINE				= 1 << 24,	// sacred ground; don't spawn monsters here, or generate items, or teleport randomly to it
	IS_POWERED					= 1 << 25,	// has been activated by machine power this turn (can probably be eliminate if needed)
	IMPREGNABLE					= 1 << 26,	// no tunneling allowed!
	
	PERMANENT_TILE_FLAGS = (DISCOVERED | MAGIC_MAPPED | ITEM_DETECTED | DOORWAY | PRESSURE_PLATE_DEPRESSED
							| STABLE_MEMORY | PLAYER_STEPPED_HERE | TERRAIN_COLORS_DANCING | IN_LOOP
							| IS_CHOKEPOINT | IS_GATE_SITE | IS_IN_MACHINE | IMPREGNABLE),
};

#define TURNS_FOR_FULL_REGEN				300
#define STOMACH_SIZE						2150
#define HUNGER_THRESHOLD					300
#define WEAK_THRESHOLD						150
#define FAINT_THRESHOLD						50
#define MAX_EXP_LEVEL						21
#define MAX_EXP								10000000L

#define ROOM_MIN_WIDTH						4
#define ROOM_MAX_WIDTH						20
#define ROOM_MIN_HEIGHT						3
#define ROOM_MAX_HEIGHT						7
#define HORIZONTAL_CORRIDOR_MIN_LENGTH		5
#define HORIZONTAL_CORRIDOR_MAX_LENGTH		15
#define VERTICAL_CORRIDOR_MIN_LENGTH		2
#define VERTICAL_CORRIDOR_MAX_LENGTH		10
#define CROSS_ROOM_MIN_WIDTH				3
#define CROSS_ROOM_MAX_WIDTH				12
#define CROSS_ROOM_MIN_HEIGHT				2
#define CROSS_ROOM_MAX_HEIGHT				5
#define MIN_SCALED_ROOM_DIMENSION			2

#define CORRIDOR_WIDTH						1

#define MAX_WAYPOINTS						200
#define WAYPOINT_SIGHT_RADIUS				10

typedef struct levelSpecProfile {
	short roomMinWidth;
	short roomMaxWidth;
	short roomMinHeight;
	short roomMaxHeight;
	short horCorrMinLength;
	short horCorrMaxLength;
	short vertCorrMinLength;
	short vertCorrMaxLength;
	short crossRoomMinWidth;
	short crossRoomMaxWidth;
	short crossRoomMinHeight;
	short crossRoomMaxHeight;
	short secretDoorChance;
	short numberOfTraps;
} levelSpecProfile;

// Making these larger means cave generation will take more trials; set them too high and the program will hang.
#define CAVE_MIN_WIDTH						50
#define CAVE_MIN_HEIGHT						20

// Keyboard commands:
#define UP_KEY				'k'
#define DOWN_KEY			'j'
#define LEFT_KEY			'h'
#define RIGHT_KEY			'l'
#define UP_ARROW			63232
#define LEFT_ARROW			63234
#define DOWN_ARROW			63233
#define RIGHT_ARROW			63235
#define UPLEFT_KEY			'y'
#define UPRIGHT_KEY			'u'
#define DOWNLEFT_KEY		'b'
#define DOWNRIGHT_KEY		'n'
#define DESCEND_KEY			'>'
#define ASCEND_KEY			'<'
#define REST_KEY			'z'
#define AUTO_REST_KEY		'Z'
#define SEARCH_KEY			's'
#define INVENTORY_KEY		'i'
#define ACKNOWLEDGE_KEY		' '
#define EQUIP_KEY			'e'
#define UNEQUIP_KEY			'r'
#define APPLY_KEY			'a'
#define THROW_KEY			't'
#define DROP_KEY			'd'
#define CALL_KEY			'c'
#define FIGHT_KEY			'f'
#define FIGHT_TO_DEATH_KEY	'F'
#define HELP_KEY			'?'
#define DISCOVERIES_KEY		'D'
#define REPEAT_TRAVEL_KEY	RETURN_KEY
#define EXAMINE_KEY			'x'
#define EXPLORE_KEY			'X'
#define AUTOPLAY_KEY		'A'
#define SEED_KEY			'~'
#define EASY_MODE_KEY		'&'
#define ESCAPE_KEY			'\033'
#define RETURN_KEY			'\015'
#define ENTER_KEY			'\012'
#define DELETE_KEY			'\177'
#define TAB_KEY				'\t'
#define PERIOD_KEY			'.'
#define VIEW_RECORDING_KEY	'V'
#define LOAD_SAVED_GAME_KEY	'O'
#define SAVE_GAME_KEY		'S'
#define NEW_GAME_KEY		'N'
#define NUMPAD_0			48
#define NUMPAD_1			49
#define NUMPAD_2			50
#define NUMPAD_3			51
#define NUMPAD_4			52
#define NUMPAD_5			53
#define NUMPAD_6			54
#define NUMPAD_7			55
#define NUMPAD_8			56
#define NUMPAD_9			57

#define UNKNOWN_KEY			(128+19)

#define min(x, y)		(((x) < (y)) ? (x) : (y))
#define max(x, y)		(((x) > (y)) ? (x) : (y))
#define clamp(x, low, hi)	(min(hi, max(x, low))) // pins x to the [y, z] interval

#define terrainFlags(x, y)					(tileCatalog[pmap[x][y].layers[DUNGEON]].flags \
											| tileCatalog[pmap[x][y].layers[LIQUID]].flags \
											| tileCatalog[pmap[x][y].layers[SURFACE]].flags \
											| tileCatalog[pmap[x][y].layers[GAS]].flags)

#ifdef BROGUE_ASSERTS
boolean cellHasTerrainFlag(short x, short y, unsigned long flagMask);
#else
#define cellHasTerrainFlag(x, y, flagMask)	((flagMask) & terrainFlags((x), (y)) ? true : false)
#endif

#define cellHasTerrainType(x, y, terrain)	((pmap[x][y].layers[DUNGEON] == (terrain) \
											|| pmap[x][y].layers[LIQUID] == (terrain) \
											|| pmap[x][y].layers[SURFACE] == (terrain) \
											|| pmap[x][y].layers[GAS] == (terrain)) ? true : false)

#define coordinatesAreInMap(x, y)			((x) >= 0 && (x) < DCOLS	&& (y) >= 0 && (y) < DROWS)
#define coordinatesAreInWindow(x, y)		((x) >= 0 && (x) < COLS		&& (y) >= 0 && (y) < ROWS)
#define mapToWindowX(x)						((x) + STAT_BAR_WIDTH + 1)
#define mapToWindowY(y)						((y) + MESSAGE_LINES)
#define windowToMapX(x)						((x) - STAT_BAR_WIDTH - 1)
#define windowToMapY(y)						((y) - MESSAGE_LINES)

#define playerCanSee(x, y)					(pmap[x][y].flags & (VISIBLE | CLAIRVOYANT_VISIBLE))
#define playerCanSeeOrSense(x, y)			((pmap[x][y].flags & (VISIBLE | CLAIRVOYANT_VISIBLE)) \
											|| (rogue.playbackOmniscience && (pmap[x][y].layers[DUNGEON] != GRANITE)))

#define CYCLE_MONSTERS_AND_PLAYERS(x)	for ((x) = &player; (x) != NULL; (x) = ((x) == &player ? monsters->nextCreature : (x)->nextCreature))

#define assureCosmeticRNG					short oldRNG = rogue.RNG; rogue.RNG = RNG_COSMETIC;
#define restoreRNG							rogue.RNG = oldRNG;

// game data formulae:

#define staffDamageLow(enchant)				((int) (3 * (2 + (enchant)) / 4))
#define staffDamageHigh(enchant)			((int) (4 + 5 * (enchant) / 2))
#define staffDamage(enchant)				(randClumpedRange(staffDamageLow(enchant), staffDamageHigh(enchant), 1 + (enchant) / 3))
#define staffPoison(enchant)				((int) (5 * pow(1.53, (double) (enchant) - 2)))
#define staffBlinkDistance(enchant)			((int) ((enchant) * 2 + 2))
#define staffHasteDuration(enchant)			((int) (2 + (enchant) * 4))
#define staffBladeCount(enchant)			((int) ((enchant) * 3 / 2))
#define staffDiscordDuration(enchant)		((int) ((enchant) * 4))

#define wandDominate(monst)					(((monst)->currentHP * 5 < (monst)->info.maxHP) ? 100 : \
											100 * ((monst)->info.maxHP - (monst)->currentHP) / (monst)->info.maxHP)

#define weaponParalysisDuration(enchant)	(max(2, (int) (2 + (enchant / 2))))
#define weaponConfusionDuration(enchant)	(max(3, (int) (1.5 * (enchant))))
#define weaponSlowDuration(enchant)			(max(3, (int) (2 * (enchant))))
#define weaponImageCount(enchant)			(clamp((int) (enchant) / 3, 1, 3))		//(max((int) (((enchant) + 1) / 2), 1))
#define weaponImageDuration(enchant)		3										//(max((int) (1 + (enchant) / 3), 2))

#define armorReprisalPercent(enchant)		max(5, (int) (enchant * 5))

// structs

enum dungeonLayers {
	DUNGEON = 0,		// dungeon-level tile	(e.g. walls)
	LIQUID,				// liquid-level tile	(e.g. lava)
	GAS,				// gas-level tile		(e.g. fire, smoke, swamp gas)
	SURFACE,			// surface-level tile	(e.g. grass)
	NUMBER_TERRAIN_LAYERS
};

// keeps track of graphics so we only redraw if the cell has changed:
typedef struct cellDisplayBuffer {
	uchar character;
	char foreColorComponents[3];
	char backColorComponents[3];
	char opacity;
	boolean needsUpdate;
} cellDisplayBuffer;

typedef struct pcell {								// permanent cell; have to remember this stuff to save levels
	enum tileType layers[NUMBER_TERRAIN_LAYERS];	// terrain
	unsigned long flags;							// non-terrain cell flags
	unsigned short volume;							// quantity of volumetric medium in cell
	unsigned char machineNumber;
	cellDisplayBuffer rememberedAppearance;			// how the player remembers the cell to look
	enum itemCategory rememberedItemCategory;		// what category of item the player remembers lying there
	enum tileType rememberedTerrain;				// what the player remembers as the terrain (i.e. highest priority terrain upon last seeing)
} pcell;

typedef struct tcell {			// transient cell; stuff we don't need to remember between levels
	short light[3];				// RGB components of lighting
	short oldLight[3];			// compare with subsequent lighting to determine whether to refresh cell
	char connected;				// used in dungeon generation to keep track of connected regions
} tcell;

typedef struct randomRange {
	short lowerBound;
	short upperBound;
	short clumpFactor;
} randomRange;

typedef struct color {
	
	// base RGB components:
	short red;
	short green;
	short blue;
	
	// random RGB components to add to base components:
	short redRand;
	short greenRand;
	short blueRand;
	
	// random scalar to add to all components:
	short rand;
	
	// Flag: this color "dances" with every refresh:
	boolean colorDances;
	
} color;

typedef struct door {
	short x;
	short y;
	enum directions direction;
} door;

typedef struct room {
	short roomX;
	short roomY;
	short width;
	short height;
	short roomX2;
	short roomY2;
	short width2;
	short height2;
	short numberOfSiblings;
	struct door doors[20];
	struct room *siblingRooms[20];
	struct room *nextRoom;
	short pathNumber; // used to calculate the distance (in rooms) between rooms
} room;

typedef struct waypoint {
	short x;
	short y;
	short pointsTo[2];
	short connectionCount;
	short connection[10][2];
} waypoint;

enum itemFlags {
	ITEM_IDENTIFIED			= Fl(0),
	ITEM_EQUIPPED			= Fl(1),
	ITEM_CURSED				= Fl(2),
	ITEM_PROTECTED			= Fl(3),
	ITEM_NO_PICKUP			= Fl(4),
	ITEM_RUNIC				= Fl(5),
	ITEM_RUNIC_HINTED		= Fl(6),
	ITEM_RUNIC_IDENTIFIED	= Fl(7),
	ITEM_CAN_BE_IDENTIFIED	= Fl(8),
	ITEM_PREPLACED			= Fl(9),
	ITEM_FLAMMABLE			= Fl(10),
	ITEM_MAGIC_DETECTED		= Fl(11),
	ITEM_NAMED				= Fl(12),
	ITEM_MAX_CHARGES_KNOWN	= Fl(13),
	ITEM_IS_KEY				= Fl(14),
	ITEM_IS_DISPOSABLE		= Fl(15),	// key disappears after being used
	ITEM_ATTACKS_SLOWLY		= Fl(16),	// mace, hammer
	ITEM_ATTACKS_PENETRATE	= Fl(17),	// spear, pike
	ITEM_ATTACKS_ALL_ADJACENT=Fl(18),	// axe, war axe
};

typedef struct item {
	unsigned short category;
	short kind;
	unsigned long flags;
	randomRange damage;
	short armor;
	short charges;
	short enchant1;
	short enchant2;
	enum monsterTypes vorpalEnemy;
	short strengthRequired;
	unsigned short quiverNumber;
	uchar displayChar;
	color *foreColor;
	color *inventoryColor;
	short quantity;
	char inventoryLetter;
	char inscription[DCOLS];
	short xLoc;
	short yLoc;
	short keyX;
	short keyY;
	short keyZ;
	struct item *nextItem;
} item;

typedef struct itemTable {
	char *name;
	char *flavor;
	char callTitle[30];
	short frequency;
	short marketValue;
	short strengthRequired;
	randomRange range;
	boolean identified;
	boolean called;
	char description[1500];
} itemTable;

enum dungeonFeatureTypes {
	DF_GRANITE_COLUMN = 1,
	DF_CRYSTAL_WALL,
	DF_LUMINESCENT_FUNGUS,
	DF_GRASS,
	DF_BONES,
	DF_RUBBLE,
	DF_FOLIAGE,
	DF_FUNGUS_FOREST,
	DF_WALL_TORCH_TOP,
	DF_POISON_GAS_TRAP,
	DF_PARALYSIS_GAS_TRAP,
	DF_TRAPDOOR,
	DF_CONFUSION_GAS_TRAP,
	DF_FLAMETHROWER_TRAP,
	DF_FLOOD_TRAP,
	DF_MUD,
	DF_SUNLIGHT,
	DF_DARKNESS,
	
	DF_SHOW_DOOR,
	DF_SHOW_POISON_GAS_TRAP,
	DF_SHOW_PARALYSIS_GAS_TRAP,
	DF_SHOW_TRAPDOOR_HALO,
	DF_SHOW_TRAPDOOR,
	DF_SHOW_CONFUSION_GAS_TRAP,
	DF_SHOW_FLAMETHROWER_TRAP,
	DF_SHOW_FLOOD_TRAP,
	
	DF_HUMAN_BLOOD,
	DF_GREEN_BLOOD,
	DF_PURPLE_BLOOD,
	DF_ACID_BLOOD,
	DF_ASH_BLOOD,
	DF_EMBER_BLOOD,
	DF_ECTOPLASM_BLOOD,
	DF_RUBBLE_BLOOD,
	DF_ROT_GAS_BLOOD,
	
	DF_VOMIT,
	DF_BLOAT_DEATH,
	DF_BLOAT_EXPLOSION,
	DF_BLOOD_EXPLOSION,
	DF_FLAMEDANCER_CORONA,
	
	DF_ROT_GAS_PUFF,
	DF_STEAM_PUFF,
	DF_STEAM_ACCUMULATION,
	DF_METHANE_GAS_PUFF,
	DF_SALAMANDER_FLAME,
	DF_URINE,
	DF_PUDDLE,
	DF_ASH,
	DF_ECTOPLASM_DROPLET,
	DF_FORCEFIELD,
	DF_LICHEN_GROW,
	//DF_LICHEN_PLANTED,
	
	DF_TRAMPLED_FOLIAGE,
	DF_FOLIAGE_REGROW,
	DF_TRAMPLED_FUNGUS_FOREST,
	DF_FUNGUS_FOREST_REGROW,
	
	DF_ACTIVE_BRIMSTONE,
	DF_INERT_BRIMSTONE,
	
	DF_OPEN_DOOR,
	DF_CLOSED_DOOR,
	DF_OPEN_IRON_DOOR_INERT,
	DF_CAGE_OPEN,
	DF_CAGE_CLOSE,
	DF_ALTAR_INERT,
	
	DF_PLAIN_FIRE,
	DF_GAS_FIRE,
	DF_EXPLOSION_FIRE,
	DF_DART_EXPLOSION,
	DF_BRIMSTONE_FIRE,
	DF_BRIDGE_FIRE,
	DF_FLAMETHROWER,
	DF_EMBERS,
	DF_OBSIDIAN,
	DF_FLOOD,
	DF_FLOOD_2,
	DF_FLOOD_DRAIN,
	DF_HOLE_2,
	DF_HOLE_DRAIN,
	
	DF_POISON_GAS_CLOUD,
	DF_PARALYSIS_GAS_CLOUD,
	DF_CONFUSION_GAS_TRAP_CLOUD,
	DF_METHANE_GAS_ARMAGEDDON,
	
	// potions
	DF_POISON_GAS_CLOUD_POTION,
	DF_PARALYSIS_GAS_CLOUD_POTION,
	DF_CONFUSION_GAS_CLOUD_POTION,
	DF_INCINERATION_POTION,
	DF_DARKNESS_POTION,
	DF_HOLE_POTION,
	DF_LICHEN_PLANTED,
	
	// wooden barricade
	DF_ADD_WOODEN_BARRICADE,
	DF_WOODEN_BARRICADE_BURN,
	
	// pools of water that, when triggered, slowly expand to fill the room
	DF_SPREADABLE_WATER,
	DF_SHALLOW_WATER,
	DF_WATER_SPREADS,
	DF_SPREADABLE_WATER_POOL,
	DF_DEEP_WATER_POOL,
	
	// when triggered, the ground gradually turns into chasm:
	DF_SPREADABLE_COLLAPSE,
	DF_COLLAPSE,
	DF_COLLAPSE_SPREADS,
	
	// when triggered, a bridge appears:
	DF_BRIDGE_APPEARS,
	
	// when triggered, the door seals and poison gas fills the room
	DF_POISON_GAS_VENT_OPEN,
	DF_DOOR_BLOCKED,
	DF_VENT_SPEW_POISON_GAS,
	
	// when triggered, pilot light ignites and explosive gas fills the room
	DF_METHANE_VENT_OPEN,
	DF_VENT_SPEW_METHANE,
	DF_PILOT_LIGHT,
	
	// thematic dungeon
	DF_AMBIENT_BLOOD,
	
	// statues crack for a few turns and then shatter, revealing the monster inside
	DF_CRACKING_STATUE,
	DF_STATUE_SHATTER,
	
	// a turret appears:
	DF_TURRET_EMERGE,
	
	// the room gradually darkens
	DF_DARKENING_FLOOR,
	DF_DARK_FLOOR,
	
	NUMBER_DUNGEON_FEATURES,
};

typedef struct lightSource {
	const color *lightColor;
	randomRange lightRadius;
	short radialFadeToPercent;
	boolean passThroughCreatures; // generally no, but miner light does
} lightSource;

enum DFFlags {
	DFF_EVACUATE_CREATURES_FIRST	= Fl(0),
	DFF_SUBSEQ_EVERYWHERE			= Fl(1),
	DFF_TREAT_AS_BLOCKING			= Fl(2),
	DFF_PERMIT_BLOCKING				= Fl(3),
	DFF_ACTIVATE_DORMANT_MONSTER	= Fl(4),
};

// Dungeon features, spawned from Architect.c:
typedef struct dungeonFeature {
	// tile info:
	enum tileType tile;
	enum dungeonLayers layer;
	
	// spawning pattern:
	short startProbability;
	short probabilityDecrement;
	unsigned long flags;
	char description[DCOLS];
	const color *flashColor;
	short flashRadius;
	enum tileType propagationTerrain;
	enum dungeonFeatureTypes subsequentDF;
	
	enum tileType requiredDungeonFoundationType;
	
	// prevalence:
	short minDepth;
	short maxDepth;
	short frequency;
	short minNumberIntercept;
	short minNumberSlope; // actually slope * 100
	short maxNumber;
	boolean messageDisplayed;
} dungeonFeature;

/*typedef struct blueprintFeature {
	short DF;
	short requiredTerrain;
	unsigned long requiredTFlags;
	unsigned long forbiddenTFlags;
	unsigned long requiredMFlags;
	unsigned long forbiddenMFlags;
	short secondaryFeatureIndex;
	short minPathingDistance;
	short maxPathingDistance;
	short minChokeScore;
	short maxChokeScore;
	unsigned long flags;
	short mandatoryMinimum;
	short preferentialMin;
	short preferentialMax;
	short percent;
} blueprintFeature;

typedef struct blueprint {
	short minDepth;
	short maxDepth;
	short spawnPercent;
	short maxCount;
	blueprintFeature feature[10];
} blueprint;*/

typedef struct floorTileType {
	// appearance:
	uchar displayChar;
	const color *foreColor;
	const color *backColor;
	// draw priority (lower number means higher priority):
	short drawPriority;
	// settings related to fire:
	char chanceToIgnite;					// doubles as chance to extinguish once ignited, if T_IS_FIRE set
	enum dungeonFeatureTypes fireType;		// doubles as terrain type remaining after extinguished, if T_IS_FIRE is set
	enum dungeonFeatureTypes discoverType;	// spawn this DF when successfully searched if T_IS_SECRET is set
	enum dungeonFeatureTypes promoteType;	// creates this dungeon spawn type
	short promoteChance;					// percent chance per turn to spawn the promotion type; will also vanish upon doing so if T_VANISHES_UPON_PROMOTION is set
	short glowLight;						// if it glows, this is the ID of the light type
	unsigned long flags;
	char description[COLS];
	char flavorText[COLS];
} floorTileType;

enum terrainFlags {
	T_OBSTRUCTS_PASSABILITY			= 1 << 0,		// cannot be walked through
	T_OBSTRUCTS_VISION				= 1 << 1,		// blocks line of sight
	T_OBSTRUCTS_ITEMS				= 1 << 2,		// items can't be on this tile
	T_OBSTRUCTS_SCENT				= 1 << 3,		// blocks the permeation of scent
	T_OBSTRUCTS_SURFACE_EFFECTS		= 1 << 4,		// grass, blood, etc. cannot exist on this tile
	T_OBSTRUCTS_GAS					= 1 << 5,		// blocks the permeation of gas
	T_PROMOTES_WITH_KEY				= 1 << 6,		// promotes if the key is present on the tile (in your pack, carried by monster, or lying on the ground)
	T_PROMOTES_WITHOUT_KEY			= 1 << 7,		// promotes if the key is NOT present on the tile (in your pack, carried by monster, or lying on the ground)
	T_SPONTANEOUSLY_IGNITES			= 1 << 8,		// monsters avoid unless chasing player or immune to fire
	T_AUTO_DESCENT					= 1 << 9,		// automatically drops the player a depth level and does some damage (2d6)
	T_PROMOTES_ON_STEP				= 1 << 10,		// promotes when a creature, player or item is on the tile
	T_PROMOTES_ON_ITEM_PICKUP		= 1 << 11,		// promotes when the player (and not a creature or item) is on the tile
	T_LAVA_INSTA_DEATH				= 1 << 12,		// kills any non-levitating non-fire-immune creature instantly
	T_CAUSES_POISON					= 1 << 13,		// any non-levitating creature gets 10 poison
	T_IS_FLAMMABLE					= 1 << 14,		// terrain can catch fire
	T_IS_FIRE						= 1 << 15,		// terrain is a type of fire; ignites neighboring flammable cells
	T_EXTINGUISHES_FIRE				= 1 << 16,		// extinguishes burning terrain or creatures
	T_ENTANGLES						= 1 << 17,		// entangles players and monsters like a spiderweb
	T_IS_DEEP_WATER					= 1 << 18,		// steals items 50% of the time and moves them around randomly
	T_ALLOWS_SUBMERGING				= 1 << 19,		// allows submersible monsters to submerge in this terrain
	T_IS_SECRET						= 1 << 20,		// successful search or being stepped on while visible transforms it into discoverType
	T_IS_WIRED						= 1 << 21,		// if wired, promotes when powered, and sends power when promoting
	T_GAS_DISSIPATES				= 1 << 22,		// does not just hang in the air forever
	T_GAS_DISSIPATES_QUICKLY		= 1 << 23,		// dissipates quickly
	T_CAUSES_DAMAGE					= 1 << 24,		// anything on the tile takes max(1-2, 10%) damage per turn
	T_CAUSES_NAUSEA					= 1 << 25,		// any creature on the tile becomes nauseous
	T_CAUSES_PARALYSIS				= 1 << 26,		// anything caught on this tile is paralyzed
	T_CAUSES_CONFUSION				= 1 << 27,		// causes creatures on this tile to become confused
	T_IS_DF_TRAP					= 1 << 28,		// spews gas of type specified in fireType with volume 1000 when stepped on
	T_STAND_IN_TILE					= 1 << 29,		// earthbound creatures will be said to stand "in" the tile, not on it
	T_VANISHES_UPON_PROMOTION		= 1 << 30,		// vanishes when randomly creating promotion dungeon feature;
													// can be overwritten anyway depending on priorities and layers of dungeon feature
	T_CAUSES_EXPLOSIVE_DAMAGE		= 1 << 31,		// is an explosion; deals 15-20 or 50% damage instantly but not again for five turns
	
	T_PATHING_BLOCKER				= (T_OBSTRUCTS_PASSABILITY | T_AUTO_DESCENT | T_IS_DF_TRAP | T_LAVA_INSTA_DEATH | T_IS_DEEP_WATER | T_IS_FIRE | T_SPONTANEOUSLY_IGNITES),
	T_WAYPOINT_BLOCKER				= (T_OBSTRUCTS_PASSABILITY | T_AUTO_DESCENT | T_IS_DF_TRAP | T_LAVA_INSTA_DEATH | T_IS_DEEP_WATER | T_SPONTANEOUSLY_IGNITES),
	T_MOVES_ITEMS					= (T_IS_DEEP_WATER | T_LAVA_INSTA_DEATH),
	T_CAN_BE_BRIDGED				= (T_AUTO_DESCENT), // | T_ALLOWS_SUBMERGING | T_LAVA_INSTA_DEATH),
	T_OBSTRUCTS_EVERYTHING			= (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_VISION | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS
									   | T_OBSTRUCTS_SCENT | T_OBSTRUCTS_SURFACE_EFFECTS),
	T_HARMFUL_TERRAIN				= (T_CAUSES_POISON | T_IS_FIRE | T_CAUSES_DAMAGE | T_CAUSES_PARALYSIS | T_CAUSES_CONFUSION | T_CAUSES_EXPLOSIVE_DAMAGE),
};

typedef struct statusEffects {
	short telepathic;			// can locate monsters for this long
	short hallucinating;
	short blind;
	short levitating;			// levitating for this long
	short slowed;				// speed is halved for this long
	short hasted;				// speed is doubled for this long
	short confused;				// movement and aiming is random
	short burning;				// takes damage per turn for this long or until entering certain liquids
	short paralyzed;			// creature gets no turns
	short poisoned;				// creature loses 1hp per turn and does not regenerate
	short stuck;				// creature can do anything but move
	short nauseous;				// every so often the creature will vomit instead of moving
	short discordant;			// creature is enemies with all other creatures
	short immuneToFire;
	short explosionImmunity;	// so that repeat explosions only have an effect once every five turns at the most
	short nutrition;			// opposite of hunger; monsters do not use this unless allied
	short entersLevelIn;		// this many ticks until it follows the player up/down the stairs
	short magicalFear;			// monster will flee
	short entranced;			// monster will mimic player movements
	short darkness;				// player's miner's light is diminished
	short lifespanRemaining;	// turns until the creature dissipates into thin air
} statusEffects;

enum hordeFlags {
	HORDE_DIES_ON_LEADER_DEATH		= Fl(0),	// if the leader dies, the horde will die instead of electing new leader
	HORDE_IS_SUMMONED				= Fl(1),	// minions summoned when any creature is the same species as the leader and casts summon
	HORDE_LEADER_CAPTIVE			= Fl(2),	// the leader is in chains and the followers are guards
	NO_PERIODIC_SPAWN				= Fl(3),	// can spawn only when the level begins -- not afterwards
	
	HORDE_MACHINE_BOSS				= Fl(4),	// used in machines for a boss challenge
	HORDE_MACHINE_WATER_MONSTER		= Fl(5),	// used in machines where the room floods with shallow water
	HORDE_MACHINE_CAPTIVE			= Fl(6),	// powerful captive monsters without any captors
	HORDE_MACHINE_STATUE			= Fl(7),	// the kinds of monsters that make sense in a statue
	HORDE_MACHINE_TURRET			= Fl(8),	// turrets, for hiding in walls
	
	HORDE_MACHINE_ONLY				= (HORDE_MACHINE_BOSS | HORDE_MACHINE_WATER_MONSTER | HORDE_MACHINE_CAPTIVE | HORDE_MACHINE_STATUE | HORDE_MACHINE_TURRET),
};

enum monsterBehaviorFlags {
	MONST_INVISIBLE					= Fl(0),	// monster is invisible
	MONST_INANIMATE					= Fl(1),	// monster has abbreviated stat bar display and is immune to many things
	MONST_IMMOBILE					= Fl(2),	// monster won't move or perform melee attacks
	MONST_CARRY_ITEM_100			= Fl(3),	// monster carries an item 100% of the time
	MONST_CARRY_ITEM_25				= Fl(4),	// monster carries an item 25% of the time
	MONST_ALWAYS_HUNTING			= Fl(5),	// monster is never asleep or in wandering mode
	MONST_FLEES_NEAR_DEATH			= Fl(6),	// monster flees when under 25% health and re-engages when over 75%
	MONST_ATTACKABLE_THRU_WALLS		= Fl(7),	// can be attacked when embedded in a wall
	MONST_DEFEND_DEGRADE_WEAPON		= Fl(8),	// hitting the monster damages the weapon
	MONST_IMMUNE_TO_WEAPONS			= Fl(9),	// weapons ineffective
	MONST_FLIES						= Fl(10),	// permanent levitation
	MONST_FLITS						= Fl(11),	// moves randomly a third of the time
	MONST_IMMUNE_TO_FIRE			= Fl(12),	// won't burn, won't die in lava
	MONST_CAST_SPELLS_SLOWLY		= Fl(13),	// takes twice the attack duration to cast a spell
	MONST_IMMUNE_TO_WEBS			= Fl(14),	// monster passes freely through webs
	MONST_REFLECT_4					= Fl(15),	// monster reflects projectiles as though wearing +4 armor of reflection
	MONST_NEVER_SLEEPS				= Fl(16),	// monster is always awake
	MONST_FIERY						= Fl(17),	// monster carries an aura of flame (but no automatic fire light)
	MONST_INTRINSIC_LIGHT			= Fl(18),	// monster carries an automatic light of the specified kind
	MONST_IMMUNE_TO_WATER			= Fl(19),	// monster moves at full speed in deep water and (if player) doesn't drop items
	MONST_RESTRICTED_TO_LIQUID		= Fl(20),	// monster can move only on tiles that allow submersion
	MONST_SUBMERGES					= Fl(21),	// monster can submerge in appropriate terrain
	MONST_MAINTAINS_DISTANCE		= Fl(22),	// monster tries to keep a distance of 3 tiles between it and player
	MONST_WILL_NOT_USE_STAIRS		= Fl(23),	// monster won't chase the player between levels
	MONST_DIES_IF_NEGATED			= Fl(24),	// monster is purely magical and will die if subjected to negation magic
	
	NEGATABLE_TRAITS				= (MONST_INVISIBLE | MONST_DEFEND_DEGRADE_WEAPON | MONST_IMMUNE_TO_WEAPONS | MONST_FLIES
									   | MONST_FLITS | MONST_IMMUNE_TO_FIRE | MONST_IMMUNE_TO_WEBS | MONST_FIERY
									   | MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_MAINTAINS_DISTANCE),
	MONST_TURRET					= (MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE |
									   MONST_ALWAYS_HUNTING | MONST_ATTACKABLE_THRU_WALLS | MONST_WILL_NOT_USE_STAIRS),
	LEARNABLE_BEHAVIORS				= (MONST_FLIES | MONST_IMMUNE_TO_FIRE | MONST_REFLECT_4),
	MONST_NEVER_VORPAL_ENEMY		= (MONST_INANIMATE | MONST_IMMOBILE | MONST_RESTRICTED_TO_LIQUID),
};

enum monsterAbilityFlags {
	MA_HIT_HALLUCINATE				= 1 << 0,	// monster can hit to cause hallucinations
	MA_HIT_STEAL_FLEE				= 1 << 1,	// monster can steal an item and then run away
	MA_ENTER_SUMMONS				= 1 << 2,	// monster will "become" its summoned leader, reappearing when that leader is defeated
	MA_HIT_DEGRADE_ARMOR			= 1 << 3,	// monster damages armor
	MA_CAST_HEAL					= 1 << 4,
	MA_CAST_HASTE					= 1 << 5,
	MA_CAST_SUMMON					= 1 << 6,	// requires that there be one or more summon hordes with this monster type as the leader
	MA_CAST_BLINK					= 1 << 7,
	MA_CAST_NEGATION				= 1 << 8,
	MA_CAST_SPARK					= 1 << 9,
	MA_CAST_FIRE					= 1 << 10,
	MA_CAST_SLOW					= 1 << 11,
	MA_CAST_DISCORD					= 1 << 12,
	MA_BREATHES_FIRE				= 1 << 13,	// shoots dragonfire at player from a distance
	MA_SHOOTS_WEBS					= 1 << 14,	// monster shoots webs at the player
	MA_ATTACKS_FROM_DISTANCE		= 1 << 15,	// monster shoots from a distance for its attack
	MA_SEIZES						= 1 << 16,	// monster seizes enemies before attacking and cannot attack flying enemies
	MA_POISONS						= 1 << 17,	// monster's damage is dealt in the form of poison, and it flees a poisoned player
	MA_DF_ON_DEATH					= 1 << 18,	// monster spawns its DF when it dies
	MA_CLONE_SELF_ON_DEFEND			= 1 << 19,	// monster splits in two when struck
	MA_KAMIKAZE						= 1 << 20,	// monster dies instead of attacking
	MA_TRANSFERENCE					= 1 << 21,	// monster recovers 90% of the damage that it inflicts as health
	
	MAGIC_ATTACK					= (MA_CAST_HEAL | MA_CAST_HASTE | MA_CAST_NEGATION | MA_CAST_SPARK | MA_CAST_FIRE | MA_CAST_SUMMON
									   | MA_CAST_SLOW | MA_CAST_DISCORD | MA_BREATHES_FIRE | MA_SHOOTS_WEBS | MA_ATTACKS_FROM_DISTANCE),
	SPECIAL_HIT						= (MA_HIT_HALLUCINATE | MA_HIT_STEAL_FLEE | MA_HIT_DEGRADE_ARMOR | MA_POISONS | MA_TRANSFERENCE),
	LEARNABLE_ABILITIES				= (MA_CAST_HEAL | MA_CAST_HASTE | MA_CAST_BLINK | MA_CAST_NEGATION | MA_CAST_SPARK | MA_CAST_FIRE
									   | MA_CAST_SLOW | MA_CAST_DISCORD | MA_TRANSFERENCE),
};

enum monsterBookkeepingFlags {
	MONST_WAS_VISIBLE				= 1 << 0,	// monster was visible to player last turn
	MONST_ADDED_TO_STATS_BAR		= 1 << 1,	// monster has been polled for the stats bar
	MONST_PREPLACED					= 1 << 2,	// monster dropped onto the level and requires post-processing
	MONST_APPROACHING_UPSTAIRS		= 1 << 3,	// following the player up the stairs
	MONST_APPROACHING_DOWNSTAIRS	= 1 << 4,	// following the player down the stairs
	MONST_APPROACHING_PIT			= 1 << 5,	// following the player down a pit
	MONST_LEADER					= 1 << 6,	// monster is the leader of a horde
	MONST_FOLLOWER					= 1 << 7,	// monster is a member of a horde
	MONST_CAPTIVE					= 1 << 8,	// monster is all tied up
	MONST_SEIZED					= 1 << 9,	// monster is being held
	MONST_SEIZING					= 1 << 10,	// monster is holding another creature immobile
	MONST_SUBMERGED					= 1 << 11,	// monster is currently submerged and hence invisible until it attacks
	MONST_JUST_SUMMONED				= 1 << 12,	// used to mark summons so they can be post-processed
	MONST_WILL_FLASH				= 1 << 13,	// this monster will flash as soon as control is returned to the player
	MONST_BOUND_TO_LEADER			= 1 << 14,	// monster will die if the leader dies or becomes separated from the leader
	MONST_ABSORBING					= 1 << 15,	// currently learning a skill by absorbing an enemy corpse
	MONST_DOES_NOT_TRACK_LEADER		= 1 << 16,	// monster will not follow its leader around
	MONST_IS_FALLING				= 1 << 17,	// monster is plunging downward at the end of the turn
	MONST_IS_DYING					= 1 << 18,	// monster has already been killed and is awaiting the end-of-turn graveyard sweep.
	MONST_GIVEN_UP_ON_SCENT			= 1 << 19,	// to help the monster remember that the scent map is a dead end
};

// Defines all creatures, which include monsters and the player:
typedef struct creatureType {
	enum monsterTypes monsterID; // index number for the monsterCatalog
	char monsterName[COLS];
	uchar displayChar;
	const color *foreColor;
	short expForKilling;
	short maxHP;
	short defense;
	short accuracy;
	randomRange damage;
	long turnsBetweenRegen;		// turns to wait before regaining 1 HP
	short sightRadius;
	short scentThreshold;
	short movementSpeed;
	short attackSpeed;
	enum dungeonFeatureTypes bloodType;
	enum lightType intrinsicLightType;
	short DFChance;						// percent chance to spawn the dungeon feature per awake turn
	enum dungeonFeatureTypes DFType;	// kind of dungeon feature
	unsigned long flags;
	unsigned long abilityFlags;
} creatureType;

typedef struct monsterWords {
	char flavorText[COLS*5];
	char pronoun[10]; // subjective
	char absorbing[40];
	char absorbStatus[40];
	char attack[5][30];
	char DFMessage[DCOLS];
	char summonMessage[DCOLS];
} monsterWords;

//#define PLAYER_BASE_MOVEMENT_SPEED		100
//#define PLAYER_BASE_ATTACK_SPEED		100

enum creatureStates {
	MONSTER_SLEEPING,
	MONSTER_TRACKING_SCENT,
	MONSTER_WANDERING,
	MONSTER_FLEEING,
	MONSTER_ALLY,
};

enum creatureModes {
	MODE_NORMAL,
	MODE_PERM_FLEEING
};

typedef struct hordeType {
	enum monsterTypes leaderType;
	
	// membership information
	short numberOfMemberTypes;
	enum monsterTypes memberType[5];
	randomRange memberCount[5];
	
	// spawning information
	short minLevel;
	short maxLevel;
	short frequency;
	enum tileType spawnsIn;
	
	enum hordeFlags flags;
} hordeType;

typedef struct creature {
	creatureType info;
	short xLoc;
	short yLoc;
	short currentHP;
	long turnsUntilRegen;
	short regenPerTurn;					// number of HP to regenerate every single turn
	enum creatureStates creatureState;	// current behavioral state
	enum creatureModes creatureMode;	// current behavioral mode (higher-level than state)
	short destination[2][2];			// the waypoints the monster is walking towards
	short comingFrom[2];				// the location the monster is walking from when wandering to avoid going back and forth
	short targetCorpseLoc[2];			// location of the corpse that the monster is approaching to gain its abilities
	char targetCorpseName[30];			// name of the deceased monster that we're approaching to gain its abilities
	unsigned long absorptionFlags;		// ability/behavior flags that the monster will gain when absorption is complete
	boolean absorbBehavior;				// above flag is behavior instead of ability
	short corpseAbsorptionCounter;		// used to measure both the time until the monster stops being interested in the corpse,
										// and, later, the time until the monster finishes absorbing the corpse.
	short **mapToMe;					// if a pack leader, this is a periodically updated pathing map to get to the leader
	short **safetyMap;					// fleeing monsters store their own safety map when out of player FOV to avoid omniscience
	short ticksUntilTurn;				// how long before the creature gets its next move
	short movementSpeed;
	short attackSpeed;
	short turnsSpentStationary;			// how many (subjective) turns it's been since the creature moved between tiles
	short flashStrength;				// monster will flash soon; this indicates the percent strength of flash
	color flashColor;					// the color that the monster will flash
	statusEffects status;
	statusEffects maxStatus;			// used to set the max point on the status bars
	unsigned long bookkeepingFlags;
	short spawnDepth;					// because monster doesn't earn xpxp on shallower levels than it spawned
	short xpxp;							// exploration experience
	short absorbsAllowed;				// how many abilities it can absorb
	struct creature *leader;			// only if monster is a follower
	struct creature *carriedMonster;	// when vampires turn into bats, one of the bats restores the vampire when it dies
	struct creature *nextCreature;
	struct item *carriedItem;			// only used for monsters
} creature;

// these are basically global variables pertaining to the game state and player's unique variables:
typedef struct playerCharacter {
	short depthLevel;					// which dungeon level are we on
	long foodSpawned;					// amount of nutrition units spawned so far this game
	boolean disturbed;					// player should stop auto-acting
	boolean gainedLevel;				// player gained at least one level this turn
	boolean gameHasEnded;				// stop everything and go to death screen
	boolean highScoreSaved;				// so that it saves the high score only once
	boolean blockCombatText;			// busy auto-fighting
	boolean autoPlayingLevel;			// seriously, don't interrupt
	boolean automationActive;			// cut some corners during redraws to speed things up
	boolean justRested;					// previous turn was a rest -- used in stealth
	boolean cautiousMode;				// used to prevent careless deaths caused by holding down a key
	boolean receivedLevitationWarning;	// only warn you once when you're hovering dangerously over liquid
	boolean updatedSafetyMapThisTurn;	// so it's updated no more than once per turn
	boolean updatedAllySafetyMapThisTurn;	// so it's updated no more than once per turn
	boolean updatedMapToSafeTerrainThisTurn;// so it's updated no more than once per turn
	boolean easyMode;					// enables easy mode
	boolean inWater;					// helps with the blue water filter effect
	boolean heardCombatThisTurn;		// so you get only one "you hear combat in the distance" per turn
	boolean creaturesWillFlashThisTurn;	// there are creatures out there that need to flash before the turn ends
	boolean staleLoopMap;				// recalculate the loop map at the end of the turn
	boolean alreadyFell;				// so the player can fall only one depth per turn
	unsigned long seed;					// the master seed for generating the entire dungeon
	short RNG;							// which RNG are we currently using?
	long gold;							// how much gold we have
	short experienceLevel;				// what experience level the player is
	unsigned long experience;			// number of experience points
	short strength;
	unsigned short monsterSpawnFuse;	// how much longer till a random monster spawns
	item *weapon;
	item *armor;
	item *ringLeft;
	item *ringRight;
	lightSource minersLight;
	float minersLightRadius;
	short ticksTillUpdateEnvironment;	// so that some periodic things happen in objective time
	unsigned short scentTurnNumber;		// helps make scent-casting work
	unsigned long turnNumber;
	signed long milliseconds;			// milliseconds since launch, to decide whether to engage cautious mode
	short xpxpThisTurn;					// how many squares the player explored this turn
	
	short upLoc[2];						// upstairs location this level
	short downLoc[2];					// downstairs location this level
	
	short lastTravelLoc[2];				// used for the return key functionality
	
	short luckyLevels[3];				// guaranteed to have generated at least n good items by the nth lucky level
	short goodItemsGenerated;			// how many generated so far
	short rewardRoomsGenerated;			// to meter the number of machines
	
	short machineNumber;				// so each machine on a level gets a unique number
	
	// maps
	short **mapToShore;					// how many steps to get back to shore
	short **chokepointMap;				// how much territory a chokepoint chokes
	short **mapToSafeTerrain;			// so monsters can get to safety
	
	// recording info
	boolean playbackMode;				// whether we're merely viewing a recording instead of playing
	unsigned long currentTurnNumber;	// how many turns have elapsed
	unsigned long howManyTurns;			// how many turns are in this recording
	short howManyDepthChanges;			// how many times the player changes depths
	short playbackDelayPerTurn;			// base playback speed; modified per turn by events
	short playbackDelayThisTurn;		// playback speed as modified
	boolean playbackPaused;
	boolean playbackFastForward;		// for loading saved games and such -- disables drawing and prevents pauses
	boolean playbackOOS;				// playback out of sync -- no unpausing allowed
	boolean playbackOmniscience;		// whether to reveal all the map during playback
	boolean playbackBetweenTurns;		// i.e. waiting for a top-level input -- iff, permit playback commands
	unsigned long nextAnnotationTurn;	// the turn number during which to display the next annotation
	char nextAnnotation[5000];			// the next annotation
	unsigned long locationInAnnotationFile; // how far we've read in the annotations file
	
	// metered items
	short strengthPotionFrequency;
	short enchantScrollFrequency;
	
	// ring bonuses:
	short clairvoyance;
	short stealthBonus;
	short regenerationBonus;
	short lightMultiplier;
	short aggravating;
	short awarenessBonus;
	short transference;
	short wisdomBonus;
} playerCharacter;

// Probably need to ditch this crap:
typedef struct levelProfile {
	short caveLevelChance;
	short crossRoomChance;
	short corridorChance;
	short doorChance;
	short maxNumberOfRooms;
	short maxNumberOfLoops;
} levelProfile;

// Stores the necessary info about a level so it can be regenerated:
typedef struct levelData {
	boolean visited;
	short numberOfRooms;
	room *roomStorage;
	pcell mapStorage[DCOLS][DROWS];
	struct item *items;
	struct creature *monsters;
	struct creature *dormantMonsters;
	unsigned long levelSeed;
	short upStairsLoc[2];
	short downStairsLoc[2];
	short playerExitedVia[2];
	unsigned long awaySince;
} levelData;

enum machineFeatureFlags {
	MF_GENERATE_ITEM			= Fl(0),	// feature entails generating an item (overridden if the machine is adopting an item)
	MF_OUTSOURCE_ITEM			= Fl(1),	// item must be adopted by another machine
	MF_NO_THROWING_WEAPONS		= Fl(2),	// the generated item cannot be a throwing weapon
	MF_GENERATE_HORDE			= Fl(3),	// generate a monster horde that has all of the horde flags
	MF_BUILD_IN_DOORWAY			= Fl(4),	// generate this feature at the room entrance
	MF_WIRE_TO_MACHINE			= Fl(5),	// wire it up to the machine
	MF_PERMIT_BLOCKING			= Fl(6),	// permit the feature to block the map's passability (e.g. to add a locked door)
	MF_TREAT_AS_BLOCKING		= Fl(7),	// treat this terrain as though it blocks, for purposes of deciding whether it can be placed there
	MF_NEAR_GATE				= Fl(8),	// feature must spawn in the rough third of tiles closest to the door
	MF_FAR_FROM_GATE			= Fl(9),	// feature must spawn in the rough third of tiles farthest from the door
	MF_MONSTER_TAKE_ITEM		= Fl(10),	// the item associated with this feature (including if adopted) will be in possession of the horde leader that's generated
	MF_MONSTER_SLEEPING			= Fl(11),	// the monsters should be asleep when generated
	MF_EVERYWHERE				= Fl(12),	// generate the feature on every tile of the machine (e.g. carpeting)
	MF_ALTERNATIVE				= Fl(13),	// build only one feature that has this flag per machine; the rest are skipped
	MF_REQUIRE_GOOD_RUNIC		= Fl(14),	// generated item must be uncursed runic
	MF_MONSTERS_DORMANT			= Fl(15),	// monsters are dormant, and appear when a dungeon feature with DFF_ACTIVATE_DORMANT_MONSTER spawns on their tile
	MF_GENERATE_MONSTER			= Fl(16),	// generate a single monster
	MF_BUILD_IN_WALLS			= Fl(17),	// build in an impassable tile that is adjacent to the interior
	MF_BUILD_ANYWHERE_ON_LEVEL	= Fl(18),	// build anywhere on the level, whether or not inside the machine
};

typedef struct machineFeature {
	// terrain
	enum dungeonFeatureTypes featureDF;	// generate this DF at the feature location (0 for none)
	enum tileType terrain;				// generate this terrain tile at the feature location (0 for none)
	enum dungeonLayers layer;			// generate the terrain tile in this layer
	
	short instanceCountRange[2];		// generate this range of instances of this feature
	short minimumInstanceCount;			// abort if fewer than this
	
	// items: these will be ignored if the feature is adopting an item
	short itemCategory;					// generate this category of item (or -1 for random)
	short itemKind;						// generate this kind of item (or -1 for random)
	short itemValueMinimum;				// generate an item worth at least this much
	
	short monsterID;					// generate a monster of this kind if MF_GENERATE_MONSTER is set
	
	short personalSpace;				// subsequent features must be generated more than this many tiles away from this feature
	unsigned long hordeFlags;			// choose a monster horde based on this
	unsigned long itemFlags;			// assign these flags to the item
	unsigned long flags;				// feature flags
} machineFeature;

enum blueprintFlags {
	BF_ADOPT_ITEM_MANDATORY			= Fl(0),	// first feature MUST adopt an item
	BF_PURGE_PATHING_BLOCKERS		= Fl(1),	// clean out traps and other T_PATHING_BLOCKERs
	BF_PURGE_INTERIOR				= Fl(2),	// clean out all of the terrain inside of the room before generating the machine
	BF_PURGE_LIQUIDS				= Fl(3),	// clean out all of the liquids inside of the room before generating the machine
	BF_SURROUND_WITH_WALLS			= Fl(4),	// fill in any impassable gaps in the perimeter (e.g. water, lava, brimstone, traps) with wall
	BF_IMPREGNABLE					= Fl(5),	// impassable perimeter tiles are locked; tunneling bolts will bounce off harmlessly
	BF_REWARD_ROOM					= Fl(6),	// metered
};

typedef struct blueprint {
	short depthRange[2];				// machine must be built between these dungeon depths
	short roomSize[2];					// machine must be generated in a room of this size
	short frequency;					// frequency (number of tickets this blueprint enters in the blueprint selection raffle)
	short featureCount;					// how many different types of features follow (max of 20)
	unsigned long flags;				// blueprint flags
	machineFeature feature[20];			// the features themselves
} blueprint;

#define NUMBER_LEVEL_PROFILES 1

#define PDS_FORBIDDEN   -1
#define PDS_OBSTRUCTION -2
#define PDS_CELL(map, x, y) ((map)->links + ((x) + DCOLS * (y)))

typedef struct pdsLink pdsLink;
typedef struct pdsMap pdsMap;


#if defined __cplusplus
extern "C" {
#endif
	
	void rogueMain();
	void executeEvent(rogueEvent *theEvent);
	boolean fileExists(char *pathname);
	boolean openFile(char *prompt, char *defaultName, char *suffix);
	void initializeRogue();
	void gameOver(char *killedBy, boolean useCustomPhrasing);
	void victory();
	void enableEasyMode();
	int rand_range(int lowerBound, int upperBound);
	unsigned long seedRandomGenerator(unsigned long seed);
	short randClumpedRange(short lowerBound, short upperBound, short clumpFactor);
	short randClump(randomRange theRange);
	boolean rand_percent(short percent);
	void shuffleList(short *list, short listLength);
	short unflag(unsigned long flag);
	void considerCautiousMode();
	void refreshScreen();
	void displayLevel();
	void shuffleTerrainColors(short percentOfCells, boolean refreshCells);
	void getCellAppearance(short x, short y, uchar *returnChar, color *returnForeColor, color *returnBackColor);
	void logLevel();
	void logBuffer(char array[DCOLS][DROWS]);
	//void logBuffer(short **array);
	boolean search(short searchStrength);
	boolean useStairs(short stairDirection);
	short passableArcCount(short x, short y);
	void analyzeMap(boolean calculateChokeMap);
	void digDungeon();
	boolean buildABridge();
	void updateMapToShore();
	void generateCave();
	boolean levelIsConnectedWithBlockingMap(char blockingMap[DCOLS][DROWS]);
	boolean checkLakePassability(short lakeX, short lakeY);
	void liquidType(short *deep, short *shallow, short *shallowWidth);
	void fillLake(short x, short y, short liquid, short scanWidth, char wreathMap[DCOLS][DROWS]);
	void resetDFMessageEligibility();
	void fillSpawnMap(enum dungeonLayers layer, enum tileType surfaceTileType, char spawnMap[DCOLS][DROWS], boolean refresh, boolean duringGame);
	boolean spawnDungeonFeature(short x, short y, dungeonFeature *feat, boolean refreshCell, boolean abortIfBlocking);
	void restoreMonster(creature *monst, short **mapToStairs, short **mapToPit);
	void restoreItem(item *theItem);
	void cellularAutomata(short minBlobWidth, short minBlobHeight,
						  short maxBlobWidth, short maxBlobHeight, short percentSeeded,
						  char birthParameters[9], char survivalParameters[9]);
	short markBlobCellAndIterate(short xLoc, short yLoc, short blobNumber);
	boolean checkRoom(short roomX, short roomY, short roomWidth, short roomHeight);
	room *attemptRoom(short doorCandidateX, short doorCandidateY, short direction,
					  boolean isCorridor, boolean isCross, short numAttempts);
	void setUpWaypoints();
	void zeroOutGrid(char grid[DCOLS][DROWS]);
	void copyGrid(char to[DCOLS][DROWS], char from[DCOLS][DROWS]);
	void createWreath(short shallowLiquid, short wreathWidth, char wreathMap[DCOLS][DROWS]);
	short oppositeDirection(short theDir);
	void connectRooms(room *fromRoom, room *toRoom, short x, short y, short direction);
	room *allocateRoom(short roomX, short roomY, short width, short height,
					   short roomX2, short roomY2, short width2, short height2);
	room *roomContainingCell(short x, short y);
	short distanceBetweenRooms(room *fromRoom, room *toRoom);
	
	void carveRectangle(short roomX, short roomY, short roomWidth, short roomHeight);
	void markRectangle(short roomX, short roomY, short roomWidth, short roomHeight);
	void addWallToList(short direction, short xLoc, short yLoc);
	void removeWallFromList(short direction, short xLoc, short yLoc);
	void plotChar(uchar inputChar,
				  short xLoc, short yLoc,
				  short backRed, short backGreen, short backBlue,
				  short foreRed, short foreGreen, short foreBlue);
	void pausingTimerStartsNow();
	boolean pauseForMilliseconds(short milliseconds);
	void nextKeyOrMouseEvent(rogueEvent *returnEvent, boolean colorsDance);
	short getHighScoresList(rogueHighScoresEntry returnList[20]);
	boolean saveHighScore(rogueHighScoresEntry theEntry);
	void initializeBrogueSaveLocation();
	char nextKeyPress();
	void refreshSideBar(creature *focusMonst);
	void printHelpScreen();
	void printDiscoveriesScreen();
	void printHighScores(boolean hiliteMostRecent);
	void showWaypoints();
	void displaySafetyMap();
	void printSeed();
	void printProgressBar(short x, short y, char barLabel[COLS], long amtFilled, long amtMax, color *fillColor, boolean dim);
	short printMonsterInfo(creature *monst, short y, boolean dim);
	void rectangularShading(short minX, short minY, short maxX, short maxY, const color *backColor, cellDisplayBuffer dbuf[COLS][ROWS]);
	void printTextBox(char *textBuf, short x, short y, short width,
					  color *foreColor, color *backColor, cellDisplayBuffer rbuf[COLS][ROWS]);
	void printMonsterDetails(creature *monst, cellDisplayBuffer rbuf[COLS][ROWS]);
	void printItemDetails(item *theItem, cellDisplayBuffer rbuf[COLS][ROWS]);
	void funkyFade(cellDisplayBuffer displayBuf[COLS][ROWS], color *colorStart, color *colorEnd, short stepCount, short x, short y, boolean invert);
	void displayCenteredAlert(char *message);
	void flashTemporaryAlert(char *message, int time);
	void waitForAcknowledgment();
	void waitForKeystrokeOrMouseClick();
	boolean confirm(char *prompt, boolean alsoDuringPlayback);
	void refreshDungeonCell(short x, short y);
	void applyColorMultiplier(color *baseColor, color *multiplierColor);
	void applyColorAverage(color *baseColor, color *newColor, short augmentWeight);
	void applyColorAugment(color *baseColor, const color *augmentingColor, short augmentWeight);
	void desaturate(color *baseColor, short weight);
	void randomizeColor(color *baseColor, short randomizePercent);
	void colorBlendCell(short x, short y, color *hiliteColor, short hiliteStrength);
	void hiliteCell(short x, short y, const color *hiliteColor, short hiliteStrength, boolean distinctColors);
	void colorMultiplierFromDungeonLight(short x, short y, color *editColor);
	void plotCharWithColor(uchar inputChar, short xLoc, short yLoc, color cellForeColor, color cellBackColor);
	void plotCharToBuffer(uchar inputChar, short x, short y, color *foreColor, color *backColor, cellDisplayBuffer dbuf[COLS][ROWS]);
	void commitDraws();
	void dumpLevelToScreen();
	void hiliteGrid(char hiliteGrid[DCOLS][DROWS], color *hiliteColor, short hiliteStrength);
	void blackOutScreen();
	void copyDisplayBuffer(cellDisplayBuffer toBuf[COLS][ROWS], cellDisplayBuffer fromBuf[COLS][ROWS]);
	void clearDisplayBuffer(cellDisplayBuffer dbuf[COLS][ROWS]);
	color colorFromComponents(char rgb[3]);
	void overlayDisplayBuffer(cellDisplayBuffer overBuf[COLS][ROWS], cellDisplayBuffer previousBuf[COLS][ROWS]);
	void flashForeground(short *x, short *y, color **flashColor, short *flashStrength, short count, short frames);
	void flash(color *theColor, short frames, short x, short y);
	void lightFlash(const color *theColor, unsigned long reqTerrainFlags, unsigned long reqTileFlags, short frames, short maxRadius, short x, short y);
	void printString(const char *theString, short x, short y, color *foreColor, color*backColor, cellDisplayBuffer dbuf[COLS][ROWS]);
	short printStringWithWrapping(char *theString, short x, short y, short width, color *foreColor,
								  color*backColor, cellDisplayBuffer dbuf[COLS][ROWS]);
	boolean getInputTextString(char *inputText, char *prompt, short maxLength, char *defaultEntry, char *suffix, short textEntryType);
	void displayChokeMap();
	void displayLoops();
	boolean pauseBrogue(short milliseconds);
	void nextBrogueEvent(rogueEvent *returnEvent, boolean colorsDance, boolean realInputEvenInPlayback);
	void executeMouseClick(rogueEvent *theEvent);
	void executeKeystroke(unsigned short keystroke, boolean controlKey, boolean shiftKey);
	void initializeLevel(short stairDirection);
	void startLevel (short oldLevelNumber, short stairDirection);
	void updateMinersLightRadius();
	void emptyGraveyard();
	void freeEverything();
	boolean randomMatchingLocation(short *loc, short dungeonType, short liquidType, short terrainType);
	enum dungeonLayers highestPriorityLayer(short x, short y, boolean skipGas);
	char *tileFlavor(short x, short y);
	char *tileText(short x, short y);
	void describeLocation(char buf[DCOLS], short x, short y);
	void printLocationDescription(short x, short y);
	void playerRuns(short direction);
	void exposeCreatureToFire(creature *monst);
	void updateFlavorText();
	void applyInstantTileEffectsToCreature(creature *monst);
	void vomit(creature *monst);
	void freeCaptive(creature *monst);
	boolean playerMoves(short direction);
	void calculateDistances(short **distanceMap,
							short destinationX, short destinationY,
							unsigned long blockingTerrainFlags,
							creature *traveler,
							boolean canUseSecretDoors);
	short pathingDistance(short x1, short y1, short x2, short y2, unsigned long blockingTerrainFlags);
	short nextStep(short **distanceMap, short x, short y);
	void travel(short x, short y, boolean autoConfirm);
	boolean explore(short frameDelay);
	void examineMode();
	boolean isDisturbed(short x, short y);
	void discover(short x, short y);
	short randValidDirectionFrom(creature *monst, short x, short y, boolean respectAvoidancePreferences);
	boolean exposeTileToFire(short x, short y, boolean alwaysIgnite);
	boolean cellCanHoldGas(short x, short y);
	void updateEnvironment();
	void updateAllySafetyMap();
	void updateSafetyMap();
	void updateSafeTerrainMap();
	void extinguishFireOnCreature(creature *monst);
	void autoRest();
	void startFighting(enum directions dir, boolean tillDeath);
	void autoFight(boolean tillDeath);
	void playerTurnEnded();
	void resetTurnNumber();
	void displayMonsterFlashes(boolean flashingEnabled);
	void temporaryMessage(char *msg1, boolean requireAcknowledgment);
	void messageWithColor(char *msg, color *theColor, boolean requireAcknowledgment);
	void message(char *message, boolean primaryMessage, boolean requireAcknowledgment);
	void displayMoreSign();
	short encodeMessageColor(char *msg, short i, const color *theColor);
	short decodeMessageColor(const char *msg, short i, color *returnColor);
	color *messageColorFromVictim(creature *monst);
	void upperCase(char *theChar);
	void updateMessageDisplay();
	void deleteMessages();
	void confirmMessages();
	void stripShiftFromMovementKeystroke(unsigned short *keystroke);
	
	void updateFieldOfViewDisplay(boolean updateDancingTerrain, boolean refreshDisplay);
	void updateFieldOfView(short xLoc, short yLoc, short radius, boolean paintScent,
						   boolean passThroughCreatures, boolean setFieldOfView, short theColor[3], short fadeToPercent);
	void betweenOctant1andN(short *x, short *y, short x0, short y0, short n);
	
	void getFOVMask(char grid[DCOLS][DROWS], short xLoc, short yLoc, float maxRadius,
					unsigned long forbiddenTerrain,	unsigned long forbiddenFlags, boolean cautiousOnWalls);
	void scanOctantFOV(char grid[DCOLS][DROWS], short xLoc, short yLoc, short octant, float maxRadius,
					   short columnsRightFromOrigin, long startSlope, long endSlope, unsigned long forbiddenTerrain,
					   unsigned long forbiddenFlags, boolean cautiousOnWalls);
	
	creature *generateMonster(short monsterID, boolean itemPossible);
	short chooseMonster(short forLevel);
	creature *spawnHorde(short hordeID, short x, short y, unsigned long forbiddenFlags, unsigned long requiredFlags);
	void fadeInMonster(creature *monst);
	boolean removeMonsterFromChain(creature *monst, creature *theChain);
	boolean monstersAreTeammates(creature *monst1, creature *monst2);
	boolean monstersAreEnemies(creature *monst1, creature *monst2);
	short pickHordeType(short depth, enum monsterTypes summonerType, unsigned long forbiddenFlags, unsigned long requiredFlags);
	creature *cloneMonster(creature *monst, boolean announce);
	short **allocDynamicGrid();
	void freeDynamicGrid(short **array);
	void copyDynamicGrid(short **to, short **from);
	void fillDynamicGrid(short **grid, short fillValue);
	unsigned long forbiddenFlagsForMonster(creature *monst);
	boolean monsterCanSubmergeNow(creature *monst);
	void populateMonsters();
	void updateMonsterState(creature *monst);
	void decrementMonsterStatus(creature *monst);
	boolean specifiedPathBetween(short x1, short y1, short x2, short y2,
								 unsigned long blockingTerrain, unsigned long blockingFlags);
	boolean openPathBetween(short x1, short y1, short x2, short y2);
	creature *monsterAtLoc(short x, short y);
	void perimeterCoords(short returnCoords[2], short n);
	boolean monsterBlinkToPreferenceMap(creature *monst, short **preferenceMap, boolean blinkUphill);
	void unAlly(creature *monst);
	void monstersTurn(creature *monst);
	void spawnPeriodicHorde();
	void clearStatus(creature *monst);
	void monsterShoots(creature *attacker, short targetLoc[2], uchar projChar, color *projColor);
	void shootWeb(creature *breather, short targetLoc[2], short kindOfWeb);
	short runicWeaponChance(item *theItem, boolean customEnchantLevel, float enchantLevel);
	void magicWeaponHit(creature *defender, item *theItem, boolean backstabbed);
	void teleport(creature *monst);
	void splitMonster(creature *monst);
	void chooseNewWanderDestination(creature *monst);
	boolean isPassableOrSecretDoor(short x, short y);
	boolean moveMonster(creature *monst, short dx, short dy);
	boolean monsterAvoids(creature *monst, short x, short y);
	short distanceBetween(short x1, short y1, short x2, short y2);
	void wakeUp(creature *monst);
	boolean canSeeMonster(creature *monst);
	void monsterName(char *buf, creature *monst, boolean includeArticle);
	float strengthModifier(item *theItem);
	float netEnchant(item *theItem);
	short hitProbability(creature *attacker, creature *defender);
	boolean attackHit(creature *attacker, creature *defender);
	void applyArmorRunicEffect(char returnString[DCOLS], creature *attacker, short *damage, boolean melee);
	boolean attack(creature *attacker, creature *defender);
	boolean inflictDamage(creature *attacker, creature *defender, short damage, const color *flashColor);
	void killCreature(creature *decedent, boolean administrativeDeath);
	void addExperience(unsigned long exp);
	void addScentToCell(short x, short y, short distance);
	void populateItems(short upstairsX, short upstairsY);
	item *placeItem(item *theItem, short x, short y);
	void removeItemFrom(short x, short y);
	void pickUpItemAt(short x, short y);
	item *addItemToPack(item *theItem);
	short getLineCoordinates(short listOfCoordinates[][2], short originLoc[2], short targetLoc[2]);
	void getImpactLoc(short returnLoc[2], short originLoc[2], short targetLoc[2],
					  short maxDistance, boolean returnLastEmptySpace);
	void negate(creature *monst);
	void slow(creature *monst, short turns);
	void haste(creature *monst, short turns);
	void heal(creature *monst, short percent);
	boolean projectileReflects(creature *attacker, creature *defender);
	short reflectBolt(short targetX, short targetY, short listOfCoordinates[][2], short kinkCell, boolean retracePath);
	void checkForMissingKeys(short x, short y);
	boolean zap(short originLoc[2], short targetLoc[2], enum boltType bolt, short boltLevel, boolean hideDetails);
	creature *nextTargetAfter(short targetX, short targetY, boolean targetAllies, boolean requireOpenPath);
	void moveCursor(boolean *targetConfirmed, boolean *canceled, boolean *tabKey, short targetLoc[2]);
	short numberOfItemsInPack();
	char nextAvailableInventoryCharacter();
	void itemName(item *theItem, char *root, boolean includeDetails, boolean includeArticle, color *suffixColor);
	char displayInventory(unsigned short categoryMask,
						  unsigned long requiredFlags,
						  unsigned long forbiddenFlags,
						  boolean waitForAcknowledge);
	short numberOfMatchingPackItems(unsigned short categoryMask,
									unsigned long requiredFlags, unsigned long forbiddenFlags,
									boolean displayErrors);
	void clearInventory(char keystroke);
	item *generateItem(unsigned short theCategory, short theKind);
	short chooseKind(itemTable *theTable, short numKinds);
	item *makeItemInto(item *theItem, unsigned short itemCategory, short itemKind);
	void strengthCheck(item *theItem);
	void recalculateEquipmentBonuses();
	void equipItem(item *theItem, boolean force);
	void equip();
	item *keyInPackFor(short x, short y);
	item *keyOnTileAt(short x, short y);
	void unequip();
	void drop();
	boolean getQualifyingLocNear(short loc[2], short x, short y, boolean hallwaysAllowed, char blockingMap[DCOLS][DROWS],
								 unsigned long forbiddenTerrainFlags, unsigned long forbiddenMapFlags, boolean forbidLiquid);
	void demoteMonsterFromLeadership(creature *monst);
	void toggleMonsterDormancy(creature *monst);
	void monsterDetails(char buf[], creature *monst);
	void makeMonsterDropItem(creature *monst);
	void throwCommand();
	void apply(item *theItem);
	void call();
	enum monsterTypes chooseVorpalEnemy();
	void identify(item *theItem);
	void readScroll(item *theItem);
	void updateRingBonuses();
	void updatePlayerRegenerationDelay();
	boolean removeItemFromChain(item *theItem, item *theChain);
	void drinkPotion(item *theItem);
	item *promptForItemOfType(unsigned short category,
							  unsigned long requiredFlags,
							  unsigned long forbiddenFlags,
							  char *prompt);
	item *itemOfPackLetter(char letter);
	void unequipItem(item *theItem, boolean force);
	short magicCharDiscoverySuffix(short category, short kind);
	uchar itemMagicChar(item *theItem);
	item *itemAtLoc(short x, short y);
	item *dropItem(item *theItem);
	itemTable *tableForItemCategory(enum itemCategory theCat);
	boolean isVowel(char theChar);
	void itemDetails(char *buf, item *theItem);
	void deleteItem(item *theItem);
	void shuffleFlavors();
	unsigned long itemValue(item *theItem);
	short strLenWithoutEscapes(char *str);
	void combatMessage(char *theMsg, color *theColor);
	void displayCombatText();
	void flashMonster(creature *monst, const color *theColor, short strength);
	
	void paintLight(lightSource *theLight, short x, short y, boolean isMinersLight);
	void updateLighting();
	boolean playerInDarkness();
	void demoteVisibility();
	void updateVision(boolean refreshDisplay);
	void burnItem(item *theItem);
	void promoteTile(short x, short y, enum dungeonLayers layer, boolean useFireDF);
	void autoPlayLevel(boolean fastForward);
	void updateClairvoyance();
	
	void initRecording();
	void flushBufferToFile();
	void fillBufferFromFile();
	void recordEvent(rogueEvent *event);
	void recallEvent(rogueEvent *event);
	void displayAnnotation();
	void loadSavedGame();
	void recordKeystroke(uchar keystroke, boolean controlKey, boolean shiftKey);
	void recordKeystrokeSequence(unsigned char *commandSequence);
	void recordMouseClick(short x, short y, boolean controlKey, boolean shiftKey);
	void OOSCheck(unsigned long x, short numberOfBytes);
	void RNGCheck();
	void executePlaybackInput(rogueEvent *recordingInput);
	void saveGame();
	void saveRecording();
	void parseFile();
	void RNGLog(char *message);
	
	void checkForDungeonErrors();
	
	void dijkstraScan(short **distanceMap, short **costMap, char passMap[DCOLS][DROWS], boolean useDiagonals);
	void pdsClear(pdsMap *map, short maxDistance, boolean eightWays);
	void pdsSetDistance(pdsMap *map, short x, short y, short distance);
	void pdsBatchOutput(pdsMap *map, short **distanceMap);
	
#if defined __cplusplus
}
#endif
