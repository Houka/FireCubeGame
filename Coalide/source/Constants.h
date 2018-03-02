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

#pragma mark -
#pragma mark Physics Constants
/** Game world dimensions in Box2d units */
#define DEFAULT_WIDTH   32.0f
#define DEFAULT_HEIGHT  18.0f

#define DEFAULT_GRAVITY 0.0f

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
#define PROTO_LEVEL_FILE   "json/map.json"
/** The key for our loaded level */
#define PROTO_LEVEL_KEY    "protolevel"

#endif /* defined(__LEVEL_CONSTANTS_H__) */
