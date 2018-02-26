//
//  JSLevelConstants.h
//  JSONDemo
//
//  This file contains all of the details that define the JSON structure of a LevelModel.
//  Created by Walker White on 1/20/16.
//
//  Author: Walker White
//  Version: 1/20/16
//
#ifndef __LEVEL_CONSTANTS_H__
#define __LEVEL_CONSTANTS_H__

/** The scene graph priorities of the various objects */
#define TILE_PRIORITY       0
#define ENTITY_PRIORITY     1

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
#pragma once
