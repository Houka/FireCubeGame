//
//	Constants.h
//	Coalide
//
#ifndef __LEVEL_CONSTANTS_H__
#define __LEVEL_CONSTANTS_H__

#pragma mark -
#pragma mark Game Constants

#define PRIMARY_FONT "charlemagne"
#define FONT_COLOR Color4::BLUE

/** This is adjusted by screen aspect ratio to get the height */
#define GAME_WIDTH 1024

#define CATEGORY_TERRAIN 0x0001

#define STUN_TIME 15

#pragma mark -
#pragma mark Model Constants

#define PLAYER_NAME		"player"
#define PLAYER_TEXTURE	"nicoal"

#define ENEMY_NAME		"enemy"
#define ACORN			"acorn"
#define MUSHROOM		"mushroom"
#define ONION			"onion"

#define TILE_NAME		"tile"

#define INANIMATE_NAME	"inanimate"
#define BREAKABLE_NAME	"breakable"
#define MOVABLE_NAME	"movable"
#define IMMOBILE_NAME	"immobile"

#pragma mark -
#pragma mark Physics Constants
/** Game world dimensions in Box2d units */
#define DEFAULT_WIDTH   32.0f
#define DEFAULT_HEIGHT  18.0f

#define DEFAULT_GRAVITY 0.0f
#define GRAVITY	9.8f

#define MIN_SPEED_FOR_CHARGING 5.0f
#define GLOBAL_AIR_DRAG 1.5f

#define UNIT_DIM Vec2(1,1)

#pragma	mark -
#pragma mark Level Constants
/** The scene graph priorities of the various objects */
#define TILE_PRIORITY       0
#define UNIT_PRIORITY		1

/** The global fields of the level model */
#define LAYERS_FIELD        "layers"
#define TILESETS_FIELD      "tilesets"
#define CANVAS_FIELD        "canvas"

/** The fields for each layer */
#define TILESET_FIELD       "tileset"
#define DATA_FIELD          "data"

/** The fields for each tileset */
#define NAME_FIELD          "name"
#define IMAGE_FIELD         "image"
#define IMAGE_WIDTH_FIELD   "imagewidth"
#define IMAGE_HEIGHT_FIELD  "imageheight"
#define TILE_WIDTH_FIELD    "tilewidth"
#define TILE_HEIGHT_FIELD   "tileheight"

/** The fields for the canvas */
#define WIDTH_FIELD         "width"
#define HEIGHT_FIELD        "height"

/** The source for our level file */
#define LEVEL_FILE   "json/map4.json"
/** The key for our loaded level */
#define LEVEL_KEY    "level"

#pragma mark -
#pragma mark Object Types
enum class OBJECT_TYPE : int {
	BREAKABLE,
	MOVABLE,
	IMMOBILE
};

#pragma mark -
#pragma mark Tile Types
enum class TILE_TYPE : int {
	WATER,
	GRASS,
	SAND,
	ICE
};

#define WATER_TEXTURE "water"
#define ISLAND_TEXTURE "island"
#define ISLAND_BASE_TEXTURE "islandBase"
#define LAND_TEXTURE "land"
#define NW_LAND_TEXTURE "nwLand"
#define N_LAND_TEXTURE "nLand"
#define NE_LAND_TEXTURE "neLand"
#define E_LAND_TEXTURE "eLand"
#define SE_LAND_TEXTURE "seLand"
#define S_LAND_TEXTURE "sLand"
#define SW_LAND_TEXTURE "swLand"
#define W_LAND_TEXTURE "wLand"
#define L_LAND_BASE_TEXTURE "lLandBase"
#define C_LAND_BASE_TEXTURE "cLandBase"
#define R_LAND_BASE_TEXTURE "rLandBase"
#define SAND_TEXTURE "sand"
#define ICE_TEXTURE "ice"

#endif /* defined(__LEVEL_CONSTANTS_H__) */
