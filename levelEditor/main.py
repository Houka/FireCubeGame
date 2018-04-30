import sys
import json
import os

import pygame as pg
from pygame.locals import *

pg.init()


fps = 60
fpsClock = pg.time.Clock()

width, height = 800, 600
screen = pg.display.set_mode((width, height))

class spritesheet(object):
    def __init__(self, filename):
        self.sheet = pg.image.load(filename).convert_alpha()
    # Load a specific image from a specific rectangle

    def image_at(self, rectangle, colorkey=None):
        "Loads image from x,y,x+offset,y+offset"
        rect = pg.Rect(rectangle)
        image = pg.Surface(rect.size).convert_alpha()
        image.fill((0, 0, 0, 0))
        image.blit(self.sheet, (0, 0), rect)
        if colorkey is not None:
            if colorkey is -1:
                colorkey = image.get_at((0, 0))
            image.set_colorkey(colorkey, pg.RLEACCEL)
        return image
    # Load a whole bunch of images and return them as a list

    def images_at(self, rects, colorkey=None):
        "Loads multiple images, supply a list of coordinates"
        return [self.image_at(rect, colorkey) for rect in rects]
    # Load a whole strip of images

    def load_strip(self, rect, image_count, colorkey=None):
        "Loads a strip of images and returns them as a list"
        tups = [(rect[0] + rect[2] * x, rect[1], rect[2], rect[3])
                for x in range(image_count)]
        return self.images_at(tups, colorkey)

# tileset = spritesheet('tileset_forest.png')
# waterset = spritesheet('tileset_water.png')
# represents an object
def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

tileset = spritesheet(resource_path("tileset_forest.png"));
waterset = spritesheet(resource_path("tileset_water.png"));


class Object:

    def __init__(self, otype):
        self.otype = otype
        self.aggroZone = -1
        self.patrolZone = -1
        self.aggroRadius = 50
        self.mass = 10
        self.cooldown = 50
        self.impulse = 10
        self.spawnRate = 10
        self.spawnMass = 5
        self.playerStunDur = 30
        self.selfStunDur = 30

# represents a floor piece with a set of zones, a friction constant, and eventually a texture ID


class Terrain:

    def __init__(self, ttype):
        self.texture = -1
        self.t_water = -1
        self.t_water_decal = -1
        self.t_dirt = -1
        self.t_ice = -1
        self.t_sand = -1
        if ttype == "dirt":
            self.t_dirt = 132 #(21 * row#6) + col#6
            self.texture = 132
        if ttype == "ice":
            self.t_ice = 139 #(21 * row#6) + col#13
            self.t_dirt = 132
            self.texture = 139
        if ttype == "sand":
            self.t_sand = 145 #(21 * row#6) + col#19
            self.t_ice = 139 #(21 * row#6) + col#13
            self.t_dirt = 132
            self.texture = 145
        if ttype == "water":
            self.texture = 4
        self.zone = set()
        self.ttype = ttype

    def __str__(self):
        return "terrain: "+self.ttype+" ("+str(self.texture)+")"

    def __repr__(self):
        return "terrain: "+self.ttype+" ("+str(self.texture)+")"

    def setTexture(self, texture):
        self.texture = texture

    def addZone(self, zoneID):
        if self.ttype != "empty":
            self.zone.add(zoneID)

    def hasZone(self, zoneID):
        return zoneID in self.zone

    def removeZone(self, zoneID):
        self.zone.discard(zoneID)

#sprite sheet map
spriteSheetMap = {"0": (0,0), "1": (0,1), "4": (0,2), "5": (0,3), "7": (0,4), "16": (0,5), "17": (0,6),
                "20": (1,0), "21": (1,1), "23": (1,2), "28": (1,3), "29": (1,4), "31": (1,5), "64": (1,6),
                "65": (2,0), "68": (2,1), "69": (2,2), "71": (2,3), "80": (2,4), "81": (2,5), "84": (2,6),
                "85": (3,0), "87": (3,1), "92": (3,2), "93": (3,3), "95": (3,4), "112": (3,5), "113": (3,6),
                "116": (4,0), "117": (4,1), "119": (4,2), "124": (4,3), "125": (4,4), "127": (4,5), "193": (4,6),
                "197": (5,0), "199": (5,1), "209": (5,2), "221": (5,3), "213": (5,4), "215": (5,5), "223": (5,6),
                "241": (6,0), "245": (6,1), "247": (6,2), "253": (6,3), "255": (6,4)}


# text stuff
pg.font.init()
myfont = pg.font.SysFont('Comic Sans MS', 18)
smallfont = pg.font.SysFont('Comic Sans MS', 12)

# colors
black = Color(0, 0, 0)
white = Color(255, 255, 255)
orange = Color(255, 69, 0)
red = Color(255, 0, 0)
brown = Color(139, 69, 19)
blue = Color(0, 0, 255)
lightBlue = Color(0, 255, 255)
sand = Color(76, 70, 50)
gray = Color(125, 125, 125)
magenta = Color(189, 0, 255)
green = Color(1, 255, 31)
offbrown = Color(89, 50, 60)

# drawing application offsets constants
leftOffset = 150
rightOffset = 150
botOffset = 0
topOffset = 100

# int constants
LEFT = 1
RIGHT = 3

# dimensions of grid
numR = 10
numC = 10
storedR = 10
storedC = 10

# variable with width, numR, etc
boxwidth = 0

# which layer are you editing
editingLayer = "terrain"
backgroundVisible = True
terrainVisible = True
objectsVisible = True

# various states of the application
setRow = False
setCol = False
setZone = False
placingElement = ""
curZone = 1
setAggroZone = False
setPatrolZone = False
setAggroRadius = False
setMass = False
setCooldown = False
setImpulse = False
setSpawnRate = False
setSpawnMass = False
setPlayerStunDur = False
setSelfStunDur = False
setPlayerLoc = False
showTextures = False
setTexture = False
dragPlacing = False

# show the details of the tile at location (r,c) or none if -1,-1
terrainDetails = (-1, -1)
textureHighlight = (-1, -1)
playerLoc = (-1, -1)

def classifyFloor(textureID):
    if textureID == -1:
        return "water"
    c = textureID % 21
    if c <= 6:
        return "dirt"
    if c <= 13:
        return "ice"
    if c <= 20:
        return "sand"

# representation of placed platforms
terrain = {}
background = {}
objects = {}
textures = {}
textures['water'] = {}
textures['dirt'] = {}
textures['ice'] = {}
textures['sand'] = {}
for i in range(100):
    textures['water'][i] = {}
    textures['dirt'][i] = {}
    textures['ice'][i] = {}
    textures['sand'][i] = {}
    terrain[i] = {}
    background[i] = {}
    objects[i] = {}
    for j in range(100):
        terrain[i][j] = Terrain("empty")
        background[i][j] = Terrain("water")
        objects[i][j] = Object("empty")
        textures['water'][i][j] = -1
        textures['dirt'][i][j] = -1
        textures['ice'][i][j] = -1
        textures['sand'][i][j] = -1


# temp string to hold input player types
typedinput = ""

def imageIDToCoord(texID):
    r = int(texID / 21)
    c = texID % 21
    return (r, c)

def imageCoordToID(texCord):
    r = texCord[0]
    c = texCord[1]
    return r * 21 + c

def loadFromJson():
    global terrain
    global background
    global objects
    global numR
    global numC
    data = json.load(open('input.json'))
    numR = data['levelInfo']['rows']
    numC = data['levelInfo']['cols']
    terrain = {}
    background = {}
    objects = {}
    for i in range(100):
        terrain[i] = {}
        background[i] = {}
        objects[i] = {}
        for j in range(100):
            terrain[i][j] = Terrain("empty")
            background[i][j] = Terrain("water")
            objects[i][j] = Object("empty")

    for r in range(numR):
        for c in range(numC):
            tmp = Terrain(data['terrain']['types'][r][c])
            tmp.zone = set(data['terrain']['zones'][r][c])
            terrain[r][c] = tmp

    #player
    p = data['objects']['player']
    if p != {}:
        tmp = Object('player')
        tmp.mass = p['mass']
        tmp.impulse = p['impulse']
        objects[p['row']][p['col']] = tmp

    for r in data['objects']['rocks']:
        tmp = Object('rock')
        objects[r['row']][r['col']] = tmp

    for c in data['objects']['crates']:
        tmp = Object('crate')
        tmp.mass = c['mass']
        objects[c['row']][c['col']] = tmp

    for a in data['objects']['acorns']:
        tmp = Object('acorn')
        tmp.mass = a['mass']
        tmp.aggroZone = a['aggroZone']
        tmp.aggroRadius = a['aggroRadius']
        tmp.patrolZone = a['patrolZone']
        tmp.cooldown = a['dashCooldown']
        tmp.impulse = a['impulse']
        objects[a['row']][a['col']] = tmp

    for o in data['objects']['onions']:
        tmp = Object('onion')
        tmp.mass = o['mass']
        tmp.aggroZone = o['aggroZone']
        tmp.aggroRadius = o['aggroRadius']
        tmp.patrolZone = o['patrolZone']
        tmp.cooldown = o['dashCooldown']
        tmp.impulse = o['impulse']
        tmp.selfStunDur = o['selfStunDur']
        tmp.playerStunDur = o['playerStunDur']
        objects[o['row']][o['col']] = tmp

    for r in data['objects']['rams']:
        tmp = Object('ram')
        tmp.mass = r['mass']
        tmp.aggroZone = r['aggroZone']
        tmp.aggroRadius = r['aggroRadius']
        tmp.patrolZone = r['patrolZone']
        tmp.cooldown = r['dashCooldown']
        tmp.impulse = r['impulse']
        tmp.playerStunDur = r['playerStunDur']
        objects[r['row']][r['col']] = tmp

    for m in data['objects']['mushrooms']:
        tmp = Object('mushroom')
        tmp.mass = m['mass']
        tmp.aggroZone = m['aggroZone']
        tmp.aggroRadius = m['aggroRadius']
        tmp.spawnMass = m['spawnMass']
        tmp.cooldown = m['spawnDashCooldown']
        tmp.impulse = m['spawnImpulse']
        objects[m['row']][m['col']] = tmp

    placingElement = ""
    setRow = False
    setCol = False
    setZone = False
    setAggroZone = False
    setPatrolZone = False
    setAggroRadius = False
    setMass = False
    setCooldown = False
    setImpulse = False
    setSpawnRate = False
    setPlayerStunDur = False
    setSelfStunDur = False
    setSpawnMass = False
    typedinput = ""
    terrainDetails = (-1, -1)

def makeJson():
    global terrain
    global background
    global objects
    global numR
    global numC
    waterTextures = [[0 for x in range(numC)] for y in range(numR)]
    waterDecals = [[0 for x in range(numC)] for y in range(numR)]
    dirtTextures = [[0 for x in range(numC)] for y in range(numR)]
    iceTextures = [[0 for x in range(numC)] for y in range(numR)]
    sandTextures = [[0 for x in range(numC)] for y in range(numR)]

    terrainTypes = [["" for x in range(numC)] for y in range(numR)]
    terrainZones = [[set() for x in range(numC)] for y in range(numR)]
    crates = []
    rocks = []
    acorns = []
    onions = []
    rams = []
    mushrooms = []
    playerInfo = {}

    for r in range(numR):
        for c in range(numC):
            waterTextures[r][c] = background[r][c].t_water
            waterDecals[r][c] = background[r][c].t_water_decal
            dirtTextures[r][c] = terrain[r][c].t_dirt
            iceTextures[r][c] = terrain[r][c].t_ice
            sandTextures[r][c] = terrain[r][c].t_sand

            terrainTypes[r][c] = terrain[r][c].ttype
            terrainZones[r][c] = list(terrain[r][c].zone)
            obj = objects[r][c]
            if obj.otype == "rock":
                rocks.append({
                    "row": r,
                    "col": c
                    })
            if obj.otype == "crate":
                crates.append({
                    "row": r,
                    "col": c,
                    "mass": obj.mass
                    })
            if obj.otype == "acorn":
                acorns.append({
                    "row": r,
                    "col": c,
                    "aggroZone": obj.aggroZone,
                    "patrolZone": obj.patrolZone,
                    "aggroRadius": obj.aggroRadius,
                    "mass": obj.mass,
                    "dashCooldown": obj.cooldown,
                    "impulse": obj.impulse
                    })
            if obj.otype == "onion":
                onions.append({
                    "row": r,
                    "col": c,
                    "aggroZone": obj.aggroZone,
                    "patrolZone": obj.patrolZone,
                    "aggroRadius": obj.aggroRadius,
                    "mass": obj.mass,
                    "dashCooldown": obj.cooldown,
                    "impulse": obj.impulse,
                    "selfStunDur": obj.selfStunDur,
                    "playerStunDur": obj.playerStunDur
                    })
            if obj.otype == "ram":
                rams.append({
                    "row": r,
                    "col": c,
                    "aggroZone": obj.aggroZone,
                    "patrolZone": obj.patrolZone,
                    "aggroRadius": obj.aggroRadius,
                    "mass": obj.mass,
                    "dashCooldown": obj.cooldown,
                    "impulse": obj.impulse,
                    "playerStunDur": obj.playerStunDur
                    })
            if obj.otype == "mushroom":
                mushrooms.append({
                    "row": r,
                    "col": c,
                    "aggroZone": obj.aggroZone,
                    "aggroRadius": obj.aggroRadius,
                    "mass": obj.mass,
                    "spawnMass": obj.spawnMass,
                    "spawnDashCooldown": obj.cooldown,
                    "spawnImpulse": obj.impulse
                    })
            if obj.otype == "player":
                playerInfo["row"] = r
                playerInfo["col"] = c
                playerInfo["mass"] = obj.mass
                playerInfo["impulse"] = obj.impulse
    levelJson = {
        "levelInfo": {
            "rows": numR,
            "cols": numC
        },
        "textures":{
            "waterDecals": waterDecals,
            "water": waterTextures,
            "grass": dirtTextures,
            "ice": iceTextures,
            "sand": sandTextures
        },
        "terrain": {
            "types": terrainTypes,
            "zones": terrainZones
        },
        "objects": {
            "player": playerInfo,
            "rocks": rocks,
            "crates": crates,
            "acorns": acorns,
            "onions": onions,
            "rams": rams,
            "mushrooms": mushrooms
        }
    }

    print(levelJson)
    with open('output.json', 'w') as outfile:
        json.dump(levelJson, outfile, indent=4)




def autoTexture(terrain):
    waterDecals = [[0 for x in range(numC)] for y in range(numR)]
    for r in range(1, 99):
        for c in range(1, 99):
            background[r][c].t_water_decal = -1
            tl = terrain[r-1][c-1].texture
            l = terrain[r][c-1].texture
            bl = terrain[r+1][c-1].texture
            t = terrain[r-1][c].texture
            cn = terrain[r][c].texture
            b = terrain[r+1][c].texture
            tr = terrain[r-1][c+1].texture
            rh = terrain[r][c+1].texture
            br = terrain[r+1][c+1].texture
            h_tl = False
            h_l = False
            h_bl = False
            h_t = False
            h_b = False
            h_tr = False
            h_r = False
            tmp = classifyFloor(cn)
            if tmp == "water" and classifyFloor(t) != "water":
                tmp = classifyFloor(tl)
                h_tl = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(tr)
                h_tr = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                w_combined = h_tl + 2 * h_tr
                if h_tl and classifyFloor(l) != "water":
                    waterDecals[r][c-1] += 2
                if h_tr and classifyFloor(rh) != "water":
                    waterDecals[r][c+1] += 1
                if w_combined == 0:
                    waterDecals[r][c] = 4 #only the middle on
                else:
                    waterDecals[r][c] = w_combined
            tmp = classifyFloor(cn)
            if tmp == "dirt" or tmp == "ice" or tmp == "sand":
                tmp = classifyFloor(tl)
                h_tl = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(l)
                h_l = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(bl)
                h_bl = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(t)
                h_t = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(b)
                h_b = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(tr)
                h_tr = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(rh)
                h_r = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")
                tmp = classifyFloor(br)
                h_br = int(tmp == "dirt" or tmp == "ice" or tmp == "sand")

                combined = (2 ** 0 * h_t + 2 ** 2 * h_r + 2 ** 4 * h_b + 2 ** 6 * h_l)
                if h_t and h_r:
                    combined += (h_tr * 2 ** 1)
                if h_r and h_b:
                    combined += (h_br * 2 ** 3)
                if h_b and h_l:
                    combined += (h_bl * 2 ** 5)
                if h_l and h_t:
                    combined += (h_tl * 2 ** 7)
                textureCoord = spriteSheetMap[str(combined)]

                terrain[r][c].t_dirt = imageCoordToID((textureCoord[0], textureCoord[1]))
                background[r][c].t_water = imageCoordToID((textureCoord[0], textureCoord[1]))

            tmp = classifyFloor(cn)
            if tmp == "sand" or tmp == "ice":
                tmp = classifyFloor(tl)
                h_tl = int(tmp == "sand" or tmp == "ice")
                tmp = classifyFloor(l)
                h_l = int(tmp == "sand" or tmp == "ice")
                tmp = classifyFloor(bl)
                h_bl = int(tmp == "sand" or tmp == "ice")
                tmp = classifyFloor(t)
                h_t = int(tmp == "sand" or tmp == "ice")
                tmp = classifyFloor(b)
                h_b = int(tmp == "sand" or tmp == "ice")
                tmp = classifyFloor(tr)
                h_tr = int(tmp == "sand" or tmp == "ice")
                tmp = classifyFloor(rh)
                h_r = int(tmp == "sand" or tmp == "ice")
                tmp = classifyFloor(br)
                h_br = int(tmp == "sand" or tmp == "ice")

                combined = (2 ** 0 * h_t + 2 ** 2 * h_r + 2 ** 4 * h_b + 2 ** 6 * h_l)
                if h_t and h_r:
                    combined += (h_tr * 2 ** 1)
                if h_r and h_b:
                    combined += (h_br * 2 ** 3)
                if h_b and h_l:
                    combined += (h_bl * 2 ** 5)
                if h_l and h_t:
                    combined += (h_tl * 2 ** 7)
                textureCoord = spriteSheetMap[str(combined)]
                terrain[r][c].t_ice = imageCoordToID((textureCoord[0], textureCoord[1] + 7))

            tmp = classifyFloor(cn)
            if tmp == "sand":
                tmp = classifyFloor(tl)
                h_tl = int(tmp == "sand")
                tmp = classifyFloor(l)
                h_l = int(tmp == "sand")
                tmp = classifyFloor(bl)
                h_bl = int(tmp == "sand")
                tmp = classifyFloor(t)
                h_t = int(tmp == "sand")
                tmp = classifyFloor(b)
                h_b = int(tmp == "sand")
                tmp = classifyFloor(tr)
                h_tr = int(tmp == "sand")
                tmp = classifyFloor(rh)
                h_r = int(tmp == "sand")
                tmp = classifyFloor(br)
                h_br = int(tmp == "sand")

                combined = (2 ** 0 * h_t + 2 ** 2 * h_r + 2 ** 4 * h_b + 2 ** 6 * h_l)
                if h_t and h_r:
                    combined += (h_tr * 2 ** 1)
                if h_r and h_b:
                    combined += (h_br * 2 ** 3)
                if h_b and h_l:
                    combined += (h_bl * 2 ** 5)
                if h_l and h_t:
                    combined += (h_tl * 2 ** 7)
                textureCoord = spriteSheetMap[str(combined)]
                terrain[r][c].t_sand = imageCoordToID((textureCoord[0], textureCoord[1] + 14))
    for r in range(1, numR):
        for c in range(1, numC):
            if waterDecals[r][c] == 4:
                background[r][c].t_water_decal = imageCoordToID((6,5))
            if waterDecals[r][c] == 3:
                background[r][c].t_water_decal = imageCoordToID((7,0))
            if waterDecals[r][c] == 1:
                background[r][c].t_water_decal = imageCoordToID((7,1))
            if waterDecals[r][c] == 2:
                background[r][c].t_water_decal = imageCoordToID((6,6))










    # go through each terrain element and set the texture ID based on the stuff surrounding it

def whichButtonPressed(pos):
    global width
    global height
    global rightOffset
    global botOffset
    global terrainDetails
    global editingLayer
    x = pos[0]
    y = pos[1]
    # setRowBtn
    if x >= width - rightOffset + 120 and x <= width - rightOffset + 120 + 25 and y >= height - botOffset - 300 + 40 and y<= height - botOffset - 300 + 40 +25:
        if terrainDetails != (-1,-1) and editingLayer == "terrain":
            return "setTexture"
    if x >= 10 and x <= 40 and y >= 10 and y <= 40:
        return "setRowBtn"
    if x >= 10 and x <= 40 and y >= 45 and y <= 75:
        return "setColBtn"
    if x >= 10 and x <= 40 and y >= 250 and y <= 290:
        return "setZoneBtn"
    if x >= 10 and x <= 40 and y >= 385 and y <= 415:
        return "setPlayerLoc"
    if x >= 10 and x <= 40 and y >= 100 and y <= 130:
        return "setDirt"
    if x >= 50 and x <= 80 and y >= 100 and y <= 130:
        return "setWater"
    if x >= 10 and x <= 40 and y >= 140 and y <= 170:
        return "setIce"
    if x >= 50 and x <= 80 and y >= 140 and y <= 170:
        return "setSand"
    if x >= 10 and x <= 30 and y >= 440 and y <= 460:
        return "setAcorn"
    if x >= 50 and x <= 70 and y >= 440 and y <= 460:
        return "setRam"
    if x >= 10 and x <= 30 and y >= 480 and y <= 500:
        return "setOnion"
    if x >= 50 and x <= 70 and y >= 480 and y <= 500:
        return "setMushroom"
    if x >= 10 and x <= 30 and y >= 550 and y <= 570:
        return "setCrate"
    if x >= 50 and x <= 70 and y >= 550 and y <= 570:
        return "setRock"
    if x >= width / 2 - 130 and x <= width / 2 - 110 and y >= 10 and y <= 30:
        return "toggleBackground"
    if x >= width / 2 - 110 and x <= width / 2 - 70 and y >= 10 and y <= 30:
        return "selectBackground"
    if x >= width / 2 - 30 and x <= width / 2 - 10 and y >= 10 and y <= 30:
        return "toggleTerrain"
    if x >= width / 2 - 10 and x <= width / 2 + 30 and y >= 10 and y <= 30:
        return "selectTerrain"
    if x >= width / 2 + 70 and x <= width / 2 + 90 and y >= 10 and y <= 30:
        return "toggleObjects"
    if x >= width / 2 + 90 and x <= width / 2 + 130 and y >= 10 and y <= 30:
        return "selectObjects"
    if editingLayer == "objects" and terrainDetails != (-1, -1):
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 40 and y <= height - botOffset - 300 + 40 + 25:
            return "setAggroZone"
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 70 and y <= height - botOffset - 300 + 70 + 25:
            if objects[terrainDetails[0]][terrainDetails[1]].otype == "mushroom":
                return "setSpawnMass"
            else:
                return "setPatrolZone"
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 100 and y <= height - botOffset - 300 + 100 + 25:
                return "setAggroRadius"
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 130 and y <= height - botOffset - 300 + 130 + 25:
            return "setMass"
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 160 and y <= height - botOffset - 300 + 160 + 25:
            return "setCooldown"
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 190 and y <= height - botOffset - 300 + 190 + 25:
            return "setImpulse"
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 220 and y <= height - botOffset - 300 + 220 + 25:
            if objects[terrainDetails[0]][terrainDetails[1]].otype == "mushroom":
                return "setSpawnRate"
            if objects[terrainDetails[0]][terrainDetails[1]].otype == "onion":
                return "setSelfStunDur"
        if x >= width - rightOffset + 122 and x <= width - rightOffset + 122 + 25 and y >= height - botOffset - 300 + 250 and y <= height - botOffset - 300 + 250 + 25:
            if objects[terrainDetails[0]][terrainDetails[1]].otype == "onion" or objects[terrainDetails[0]][terrainDetails[1]].otype == "ram":
                return "setPlayerStunDur"
    return None


def inputFinished():
    global setRow
    global setCol
    global setZone
    global curZone
    global numR
    global numC
    global setAggroZone
    global setPatrolZone
    global setAggroRadius
    global setMass
    global setCooldown
    global setImpulse
    global setSpawnRate
    global setSelfStunDur
    global setPlayerStunDur
    global setSpawnMass
    global typedinput
    if setRow == True:
        setRow = False
        if(typedinput != '' and int(typedinput) > 0 and int(typedinput) < 100):
            numR = int(typedinput)
    if setCol == True:
        setCol = False
        if(typedinput != '' and int(typedinput) > 0 and int(typedinput) < 100):
            numC = int(typedinput)
    if setZone == True:
        if(typedinput != '' and int(typedinput) > 0 and int(typedinput) < 100):
            curZone = int(typedinput)
    if(typedinput != '' and int(typedinput) > 0 and (int(typedinput) < 100 or setAggroRadius)):
        r = terrainDetails[0]
        c = terrainDetails[1]
        if (r == -1 and c == -1) or objects[r][c].otype == "empty":
            return
        if setAggroZone == True:
            setAggroZone = False
            objects[r][c].aggroZone = int(typedinput)
        if setPatrolZone == True:
            setPatrolZone = False
            objects[r][c].patrolZone = int(typedinput)
        if setAggroRadius == True:
            setAggroRadius = False
            objects[r][c].aggroRadius = int(typedinput)
        if setMass == True:
            setMass = False
            objects[r][c].mass = int(typedinput)
        if setCooldown == True:
            setCooldown = False
            objects[r][c].cooldown = int(typedinput)
        if setImpulse == True:
            setImpulse = False
            objects[r][c].impulse = int(typedinput)
        if setSpawnRate == True:
            setSpawnRate = False
            objects[r][c].spawnRate = int(typedinput)
        if setSpawnMass == True:
            setSpawnMass = False
            objects[r][c].spawnMass = int(typedinput)
        if setSelfStunDur == True:
            setSelfStunDur = False
            objects[r][c].selfStunDur = int(typedinput)
        if setPlayerStunDur == True:
            setPlayerStunDur = False
            objects[r][c].playerStunDur = int(typedinput)


# app loop.
while True:
    screen.fill((0, 0, 0))

    for event in pg.event.get():
        if event.type == QUIT:
            pg.quit()
            sys.exit()
        if event.type == MOUSEBUTTONDOWN and placingElement != "" and editingLayer == "terrain":
            dragPlacing = True
        if event.type == MOUSEBUTTONUP:
            dragPlacing = False
        if event.type == MOUSEMOTION:
            pos = pg.mouse.get_pos()
            x = pos[0]
            y = pos[1]
            for r in range(numR):
                for c in range(numC):
                    if x >= leftOffset + boxwidth * c and x <= leftOffset + boxwidth * (c + 1) and y >= topOffset + boxwidth * r and y <= topOffset + boxwidth * (r + 1):
                        if placingElement == "setWater":
                            if event.buttons[2] == 1:
                                background[r][c] = Terrain("empty")
                            elif event.buttons[0] == 1:
                                background[r][c] = Terrain("water")
                        elif event.buttons[2] == 1:
                            terrain[r][c] = Terrain("empty")
                            background[r][c] = Terrain("water")
                        elif event.buttons[0] == 1:
                            if placingElement == "setDirt":
                                terrain[r][c] = Terrain("dirt")
                            elif placingElement == "setIce":
                                terrain[r][c] = Terrain("ice")
                            elif placingElement == "setSand":
                                terrain[r][c] = Terrain("sand")
        if event.type == MOUSEBUTTONUP:
            buttonPressed = whichButtonPressed(pg.mouse.get_pos())
            # if buttonPressed == "setTexture" and not setTexture:

                # setTexture = True
                # storedC = numC
                # storedR = numR
                # numR = 7
                # numC = 21
                # currently this button is disabled since auto texture takes care of it
            if buttonPressed == "setRowBtn":
                setRow = True
                setZone = False
                typedinput = ""
            if buttonPressed == "setColBtn":
                setCol = True
                setZone = False
                typedinput = ""
            if buttonPressed == "setZoneBtn":
                setZone = not setZone
                setRow = False
                setRow = False
                placingElement = ""
                typedinput = ""
            if buttonPressed == "setPlayerLoc" and editingLayer == "objects":
                placingElement = "setPlayerLoc"
            if buttonPressed == "setDirt" and editingLayer == "terrain":
                placingElement = "setDirt"
            if buttonPressed == "setWater" and editingLayer == "background":
                placingElement = "setWater"
            if buttonPressed == "setIce" and editingLayer == "terrain":
                placingElement = "setIce"
            if buttonPressed == "setSand" and editingLayer == "terrain":
                placingElement = "setSand"
            if buttonPressed == "setAcorn" and editingLayer == "objects":
                placingElement = "setAcorn"
            if buttonPressed == "setRam" and editingLayer == "objects":
                placingElement = "setRam"
            if buttonPressed == "setOnion" and editingLayer == "objects":
                placingElement = "setOnion"
            if buttonPressed == "setMushroom" and editingLayer == "objects":
                placingElement = "setMushroom"
            if buttonPressed == "setCrate" and editingLayer == "objects":
                placingElement = "setCrate"
            if buttonPressed == "setRock" and editingLayer == "objects":
                placingElement = "setRock"
            if buttonPressed == "setAggroZone":
                setAggroZone = not setAggroZone
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setPatrolZone":
                setPatrolZone = not setPatrolZone
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setAggroRadius":
                setAggroRadius = not setAggroRadius
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setMass":
                setMass = not setMass
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setCooldown":
                setCooldown = not setCooldown
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setImpulse":
                setImpulse = not setImpulse
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setSpawnRate":
                setSpawnRate = not setSpawnRate
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setSpawnMass":
                setSpawnMass = not setSpawnMass
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setSelfStunDur":
                setSelfStunDur = not setSelfStunDur
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "setPlayerStunDur":
                setPlayerStunDur = not setPlayerStunDur
                setRow = False
                setRow = False
                setZone = False
                typedinput = ""
            if buttonPressed == "toggleBackground":
                backgroundVisible = not backgroundVisible
            if buttonPressed == "selectBackground":
                terrainDetails = (-1,-1)
                editingLayer = "background"
                placingElement = ""
            if buttonPressed == "toggleTerrain":
                terrainVisible = not terrainVisible
            if buttonPressed == "selectTerrain":
                terrainDetails = (-1,-1)
                editingLayer = "terrain"
                placingElement = ""
            if buttonPressed == "toggleObjects":
                objectsVisible = not objectsVisible
            if buttonPressed == "selectObjects":
                terrainDetails = (-1,-1)
                editingLayer = "objects"
                placingElement = ""
            if buttonPressed == None:
                pos = pg.mouse.get_pos()
                x = pos[0]
                y = pos[1]
                placed = False
                for r in range(numR):
                    for c in range(numC):
                        if x >= leftOffset + boxwidth * c and x <= leftOffset + boxwidth * (c + 1) and y >= topOffset + boxwidth * r and y <= topOffset + boxwidth * (r + 1):
                            placed = True
                            if not setZone and event.button == RIGHT:
                                if editingLayer == "terrain":
                                    terrain[r][c] = Terrain("empty")
                                    background[r][c] = Terrain("water")
                                elif editingLayer == "objects":
                                    objects[r][c] = Object("empty")
                            elif placingElement == "setPlayerLoc":
                                objects[r][c] = Object("player")
                                if playerLoc != (-1, -1):
                                    objects[playerLoc[0]][playerLoc[1]
                                                          ] = Object("empty")
                                playerLoc = (r, c)
                            elif placingElement == "setWater":
                                background[r][c] = Terrain("water")
                            elif placingElement == "setDirt":
                                terrain[r][c] = Terrain("dirt")
                            elif placingElement == "setIce":
                                terrain[r][c] = Terrain("ice")
                            elif placingElement == "setSand":
                                terrain[r][c] = Terrain("sand")
                            elif placingElement == "setCrate":
                                objects[r][c] = Object("crate")
                                if playerLoc == (r,c):
                                    playerLoc = (-1,-1)
                            elif placingElement == "setRock":
                                objects[r][c] = Object("rock")
                                if playerLoc == (r,c):
                                    playerLoc = (-1,-1)
                            elif placingElement == "setAcorn":
                                objects[r][c] = Object("acorn")
                                if playerLoc == (r,c):
                                    playerLoc = (-1,-1)
                            elif placingElement == "setRam":
                                objects[r][c] = Object("ram")
                                if playerLoc == (r,c):
                                    playerLoc = (-1,-1)
                            elif placingElement == "setOnion":
                                objects[r][c] = Object("onion")
                                if playerLoc == (r,c):
                                    playerLoc = (-1,-1)
                            elif placingElement == "setMushroom":
                                objects[r][c] = Object("mushroom")
                                if playerLoc == (r,c):
                                    playerLoc = (-1,-1)
                            elif setZone and editingLayer == "terrain":
                                if event.button == LEFT:
                                    terrain[r][c].addZone(curZone)
                                elif event.button == RIGHT:
                                    terrain[r][c].removeZone(curZone)
                            else:
                                if not setTexture:
                                    terrainDetails = (r, c)
                                else:
                                    textureHighlight = (r,c)
                if placed == False and not setTexture:
                    placingElement = ""
                    setRow = False
                    setCol = False
                    setZone = False
                    setAggroZone = False
                    setPatrolZone = False
                    setAggroRadius = False
                    setMass = False
                    setCooldown = False
                    setImpulse = False
                    setSpawnRate = False
                    setPlayerStunDur = False
                    setSelfStunDur = False
                    setSpawnMass = False
                    typedinput = ""
                    terrainDetails = (-1, -1)
        if event.type == pg.KEYDOWN:
            if event.key == pg.K_0:
                typedinput += "0"
            if event.key == pg.K_1:
                typedinput += "1"
            if event.key == pg.K_2:
                typedinput += "2"
            if event.key == pg.K_3:
                typedinput += "3"
            if event.key == pg.K_4:
                typedinput += "4"
            if event.key == pg.K_5:
                typedinput += "5"
            if event.key == pg.K_6:
                typedinput += "6"
            if event.key == pg.K_7:
                typedinput += "7"
            if event.key == pg.K_8:
                typedinput += "8"
            if event.key == pg.K_9:
                typedinput += "9"
            if event.key == pg.K_RETURN:
                if setTexture and textureHighlight != (-1,-1):
                    terrain[terrainDetails[0]][terrainDetails[1]].texture = imageCoordToID(textureHighlight)
                    setTexture = False
                    textureHighlight = (-1,-1)
                    numR = storedR
                    numC = storedC
                inputFinished()
            if event.key == pg.K_p:
                makeJson()
            if event.key == pg.K_l:
                loadFromJson()
            if event.key == pg.K_t:
                autoTexture(terrain)
                showTextures = not showTextures

            # Update.
    boxwidth = int(min((width - (leftOffset + rightOffset)) / numC,
                   (height - (botOffset + topOffset)) / numR))

    # print(str(boxwidth) + "  " + str(boxheight))
    # Draw.
    pg.draw.rect(screen, Color(200, 200, 200), [0, 0, width, height], 0)

    # buttons

    # layers
    pg.draw.rect(screen, gray, [int(width / 2) - 130, 10, 60, 30])
    pg.draw.rect(screen, gray, [int(width / 2) - 30, 10, 60, 30])
    pg.draw.rect(screen, gray, [int(width / 2) + 70, 10, 60, 30])

    backgroundLayerLabel = myfont.render("Bck", False, (0, 0, 0))
    screen.blit(backgroundLayerLabel, (int(width / 2) - 105, 10))
    terrainLayerLabel = myfont.render("Ter", False, (0, 0, 0))
    screen.blit(terrainLayerLabel, (int(width / 2) - 6, 10))
    objectLayerLabel = myfont.render("Obj", False, (0, 0, 0))
    screen.blit(objectLayerLabel, (int(width / 2) + 95, 10))

    if backgroundVisible:
        pg.draw.rect(screen, white, [int(width / 2) - 130, 10, 20, 30])
    else:
        pg.draw.rect(screen, black, [int(width / 2) - 130, 10, 20, 30])
    if terrainVisible:
        pg.draw.rect(screen, white, [int(width / 2) - 30, 10, 20, 30])
    else:
        pg.draw.rect(screen, black, [int(width / 2) - 30, 10, 20, 30])
    if objectsVisible:
        pg.draw.rect(screen, white, [int(width / 2) + 70, 10, 20, 30])
    else:
        pg.draw.rect(screen, black, [int(width / 2) + 70, 10, 20, 30])
    if editingLayer == "background":
        pg.draw.rect(screen, orange, [int(width / 2) - 130, 10, 60, 30], 3)
    elif editingLayer == "terrain":
        pg.draw.rect(screen, orange, [int(width / 2) - 30, 10, 60, 30], 3)
    elif editingLayer == "objects":
        pg.draw.rect(screen, orange, [int(width / 2) + 70, 10, 60, 30], 3)

    # grid dimensions
    if setRow == False:
        pg.draw.rect(screen, black, [10, 10, 30, 30], 0)
    else:
        pg.draw.rect(screen, orange, [10, 10, 30, 30], 0)
    if setCol == False:
        pg.draw.rect(screen, black, [10, 45, 30, 30], 0)
    else:
        pg.draw.rect(screen, orange, [10, 45, 30, 30], 0)

    pg.draw.rect(screen, black, [10, 250, 30, 30], 0)
    if setZone:
        pg.draw.rect(screen, red, [10, 250, 30, 30], 2)

    rowLabel = myfont.render("# rows", False, (0, 0, 0))
    screen.blit(rowLabel, (45, 10))
    rowLabel = myfont.render("# cols", False, (0, 0, 0))
    screen.blit(rowLabel, (45, 45))
    zoneLabel = myfont.render("zone ID", False, (0, 0, 0))
    screen.blit(zoneLabel, (45, 250))

    rowDisplay = myfont.render(str(numR), False, (255, 255, 255))
    screen.blit(rowDisplay, (12, 10))
    colDisplay = myfont.render(str(numC), False, (255, 255, 255))
    screen.blit(colDisplay, (12, 45))
    zoneDisplay = myfont.render(str(curZone), False, (255, 255, 255))
    screen.blit(zoneDisplay, (14, 250))

    pg.draw.rect(screen, brown, [10, 100, 30, 30], 0)
    pg.draw.rect(screen, blue, [50, 100, 30, 30], 0)
    pg.draw.rect(screen, lightBlue, [10, 140, 30, 30], 0)
    pg.draw.rect(screen, sand, [50, 140, 30, 30], 0)

    if placingElement == "setDirt":
        pg.draw.rect(screen, orange, [10, 100, 30, 30], 2)
    if placingElement == "setWater":
        pg.draw.rect(screen, orange, [50, 100, 30, 30], 2)
    if placingElement == "setIce":
        pg.draw.rect(screen, orange, [10, 140, 30, 30], 2)
    if placingElement == "setSand":
        pg.draw.rect(screen, orange, [50, 140, 30, 30], 2)

    pg.draw.circle(screen, black, [25, 400], 15)
    pg.draw.rect(screen, green, [10, 440, 20, 20], 0)
    pg.draw.rect(screen, offbrown, [50, 440, 20, 20], 0)
    pg.draw.rect(screen, magenta, [10, 480, 20, 20], 0)
    pg.draw.rect(screen, white, [50, 480, 20, 20], 0)
    pg.draw.rect(screen, orange, [10, 550, 20, 20], 0)
    pg.draw.rect(screen, gray, [50, 550, 20, 20], 0)


    if placingElement == "setAcorn":
        pg.draw.rect(screen, orange, [10, 440, 20, 20], 2)
    if placingElement == "setRam":
        pg.draw.rect(screen, orange, [50, 440, 20, 20], 2)
    if placingElement == "setOnion":
        pg.draw.rect(screen, orange, [10, 480, 20, 20], 2)
    if placingElement == "setMushroom":
        pg.draw.rect(screen, orange, [50, 480, 20, 20], 2)
    if placingElement == "setPlayerLoc":
        pg.draw.circle(screen, orange, [25, 400], 16, 2)
    if placingElement == "setCrate":
        pg.draw.rect(screen, red, [10, 550, 20, 20], 2)
    if placingElement == "setRock":
        pg.draw.rect(screen, orange, [50, 550, 20, 20], 2)

    for r in range(numR):
            for c in range(numC):
                if setTexture:
                    img = tileset.image_at((c*64, r*64, 64, 64))
                    screen.blit(pg.transform.scale(img, (boxwidth, boxwidth)), (leftOffset + boxwidth * c, topOffset + boxwidth * r))
                else:
                    if backgroundVisible:
                        if background[r][c].ttype == "water":
                            pg.draw.rect(screen, blue,
                                         [leftOffset + boxwidth * c, topOffset + boxwidth * r, (boxwidth - 1), (boxwidth - 1)], 0)
                            if showTextures:
                                if background[r][c].t_water != -1:
                                    texCord = imageIDToCoord(background[r][c].t_water)
                                    tex = waterset.image_at((64 * texCord[1], 64 *texCord[0], 64, 64))
                                    screen.blit(pg.transform.scale(tex, (boxwidth, boxwidth)), (leftOffset + boxwidth * c, topOffset + boxwidth * r))
                                if background[r][c].t_water_decal != -1:
                                    texCord = imageIDToCoord(background[r][c].t_water_decal)
                                    tex = waterset.image_at((64 * texCord[1], 64 *texCord[0], 64, 64))
                                    screen.blit(pg.transform.scale(tex, (boxwidth, boxwidth)), (leftOffset + boxwidth * c, topOffset + boxwidth * r))
                    if terrainVisible:
                        if showTextures:
                            if terrain[r][c].t_dirt != -1:
                                texCord = imageIDToCoord(terrain[r][c].t_dirt)
                                tex = tileset.image_at((64 * texCord[1], 64 *texCord[0], 64, 64))
                                screen.blit(pg.transform.scale(tex, (boxwidth, boxwidth)), (leftOffset + boxwidth * c, topOffset + boxwidth * r))

                            if terrain[r][c].t_ice != -1:
                                texCord = imageIDToCoord(terrain[r][c].t_ice)
                                tex = tileset.image_at((64 * texCord[1], 64 *texCord[0], 64, 64))
                                screen.blit(pg.transform.scale(tex, (boxwidth, boxwidth)), (leftOffset + boxwidth * c, topOffset + boxwidth * r))

                            if terrain[r][c].t_sand != -1:
                                texCord = imageIDToCoord(terrain[r][c].t_sand)
                                tex = tileset.image_at((64 * texCord[1], 64 *texCord[0], 64, 64))
                                screen.blit(pg.transform.scale(tex, (boxwidth, boxwidth)), (leftOffset + boxwidth * c, topOffset + boxwidth * r))
                        else:
                            if terrain[r][c].ttype == "dirt":
                                pg.draw.rect(screen, brown,
                                             [leftOffset + boxwidth * c, topOffset + boxwidth * r, (boxwidth - 1), (boxwidth - 1)], 0)
                            if terrain[r][c].ttype == "ice":
                                pg.draw.rect(screen, lightBlue,
                                             [leftOffset + boxwidth * c, topOffset + boxwidth * r, (boxwidth - 1), (boxwidth - 1)], 0)
                            if terrain[r][c].ttype == "sand":
                                pg.draw.rect(screen, sand,
                                             [leftOffset + boxwidth * c, topOffset + boxwidth * r, (boxwidth - 1), (boxwidth - 1)], 0)
                        if setZone and terrain[r][c].hasZone(curZone) or (setAggroZone and terrain[r][c].hasZone(objects[terrainDetails[0]][terrainDetails[1]].aggroZone)) or (setPatrolZone and terrain[r][c].hasZone(objects[terrainDetails[0]][terrainDetails[1]].patrolZone)):
                            pg.draw.rect(screen, red,
                                         [leftOffset + boxwidth * c, topOffset + boxwidth * r, (boxwidth - 1), (boxwidth - 1)], 3)
                    if objectsVisible:
                        if objects[r][c].otype == "crate":
                            pg.draw.rect(screen, orange,
                                         [leftOffset + boxwidth * c + 5, topOffset + boxwidth * r + 5, (boxwidth - 10), (boxwidth - 10)], 0)
                        if objects[r][c].otype == "rock":
                            pg.draw.rect(screen, gray,
                                         [leftOffset + boxwidth * c + 5, topOffset + boxwidth * r + 5, (boxwidth - 10), (boxwidth - 10)], 0)
                        if objects[r][c].otype == "acorn":
                            pg.draw.rect(screen, green,
                                         [leftOffset + boxwidth * c + 5, topOffset + boxwidth * r + 5, (boxwidth - 10), (boxwidth - 10)], 0)
                        if objects[r][c].otype == "ram":
                            pg.draw.rect(screen, offbrown,
                                         [leftOffset + boxwidth * c + 5, topOffset + boxwidth * r + 5, (boxwidth - 10), (boxwidth - 10)], 0)
                        if objects[r][c].otype == "onion":
                            pg.draw.rect(screen, magenta,
                                         [leftOffset + boxwidth * c + 5, topOffset + boxwidth * r + 5, (boxwidth - 10), (boxwidth - 10)], 0)
                        if objects[r][c].otype == "mushroom":
                            pg.draw.rect(screen, white,
                                         [leftOffset + boxwidth * c + 5, topOffset + boxwidth * r + 5, (boxwidth - 10), (boxwidth - 10)], 0)
                        if objects[r][c].otype == "player":
                            pg.draw.circle(screen, black,
                                         [int(leftOffset + boxwidth * c + (boxwidth/2)), int(topOffset + boxwidth * r + (boxwidth/2))], int((boxwidth-5)/2), 0)

    if terrainDetails != (-1, -1):
            tr = terrainDetails[0]
            tc = terrainDetails[1]
            if not setTexture:
                pg.draw.rect(screen, orange,
                             [leftOffset + boxwidth * tc, topOffset + boxwidth * tr, (boxwidth - 1), (boxwidth - 1)], 3)
            else:
                coord = imageIDToCoord(terrain[tr][tc].texture)
                pg.draw.rect(screen, green,
                             [leftOffset + boxwidth * coord[1], topOffset + boxwidth * coord[0], (boxwidth - 1), (boxwidth - 1)], 3)
            pg.draw.rect(screen, white, [
                         width - rightOffset, height - botOffset - 300, rightOffset, 300 + botOffset], 0)
            pg.draw.rect(screen, black, [
                         width - rightOffset, height - botOffset - 300, rightOffset, 300 + botOffset], 4)
            if editingLayer == "objects":
                objectLabel = myfont.render(
                        objects[tr][tc].otype + " (" + str(tr) + ", " + str(tc) + ")", False, (0, 0, 0))
                screen.blit(objectLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 10))
                if objects[tr][tc].otype != "empty" and objects[tr][tc].otype != "rock":
                    if objects[tr][tc].otype != "player" and objects[tr][tc].otype != "crate":
                        # aggro zone
                        azLabel = myfont.render(
                            "aggro zone: ", False, (0, 0, 0))
                        screen.blit(azLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 40))

                        if setAggroZone:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 40, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 40, 25, 25])

                        azDisplay = myfont.render(
                            str(objects[tr][tc].aggroZone), False, (255, 255, 255))
                        screen.blit(azDisplay, (width - rightOffset +
                                                122, height - botOffset - 300 + 40))

                    if objects[tr][tc].otype != "mushroom" and objects[tr][tc].otype != "player" and objects[tr][tc].otype != "crate":
                        # patrol zone
                        pzLabel = myfont.render(
                            "patrol zone: ", False, (0, 0, 0))
                        screen.blit(pzLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 70))

                        if setPatrolZone:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 70, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 70, 25, 25])

                        pzDisplay = myfont.render(
                            str(objects[tr][tc].patrolZone), False, (255, 255, 255))
                        screen.blit(pzDisplay, (width - rightOffset +
                                                122, height - botOffset - 300 + 70))
                    elif objects[tr][tc].otype != "player" and objects[tr][tc].otype != "crate":
                        smLabel = myfont.render(
                            "spawn mass: ", False, (0, 0, 0))
                        screen.blit(smLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 70))

                        if setSpawnMass:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 70, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 70, 25, 25])

                        smDisplay = myfont.render(
                            str(objects[tr][tc].spawnMass), False, (255, 255, 255))
                        screen.blit(smDisplay, (width - rightOffset +
                                                122, height - botOffset - 300 + 70))

                    if objects[tr][tc].otype != "player" and objects[tr][tc].otype != "crate":
                        # aggro radius
                        arLabel = myfont.render(
                            "aggro radius: ", False, (0, 0, 0))
                        screen.blit(arLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 100))

                        if setAggroRadius:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 100, 25, 25])
                            pg.draw.circle(screen, red, [int(leftOffset + boxwidth * tc + (
                                int(boxwidth / 2))), int(topOffset + boxwidth * tr + (int(boxwidth / 2)))], int(boxwidth * objects[tr][tc].aggroRadius / 10), 3)
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 100, 25, 25])
                        ar = objects[tr][tc].aggroRadius
                        arDisplay = myfont.render(
                            str(ar), False, (255, 255, 255))
                        if ar >= 100:
                            arDisplay = smallfont.render(
                                str(ar), False, (255, 255, 255))
                        screen.blit(arDisplay, (width - rightOffset +
                                                122, height - botOffset - 300 + 100))

                    # mass
                    mLabel = myfont.render(
                        "mass: ", False, (0, 0, 0))
                    screen.blit(mLabel, (width - rightOffset +
                                         5, height - botOffset - 300 + 130))

                    if setMass:
                        pg.draw.rect(screen, orange, [width - rightOffset +
                                                      120, height - botOffset - 300 + 130, 25, 25])
                    else:
                        pg.draw.rect(screen, black, [width - rightOffset +
                                                     120, height - botOffset - 300 + 130, 25, 25])

                    mDisplay = myfont.render(
                        str(objects[tr][tc].mass), False, (255, 255, 255))
                    screen.blit(mDisplay, (width - rightOffset +
                                           122, height - botOffset - 300 + 130))

                    if objects[tr][tc].otype != "player" and objects[tr][tc].otype != "crate":
                        # cooldown between shots
                        cdLabel = myfont.render(
                            "dash cooldown: ", False, (0, 0, 0))
                        screen.blit(cdLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 160))

                        if setCooldown:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 160, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 160, 25, 25])

                        cdDisplay = myfont.render(
                            str(objects[tr][tc].cooldown), False, (255, 255, 255))
                        screen.blit(cdDisplay, (width - rightOffset +
                                                122, height - botOffset - 300 + 160))

                    # impulse
                    if objects[tr][tc].otype != "crate":
                        impLabel = myfont.render(
                            "impulse: ", False, (0, 0, 0))
                        screen.blit(impLabel, (width - rightOffset +
                                               5, height - botOffset - 300 + 190))

                        if setImpulse:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 190, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 190, 25, 25])

                        impDisplay = myfont.render(
                            str(objects[tr][tc].impulse), False, (255, 255, 255))
                        screen.blit(impDisplay, (width - rightOffset +
                                                 122, height - botOffset - 300 + 190))

                    # mushroom spawn rate
                    if objects[tr][tc].otype == "mushroom":
                        srLabel = myfont.render(
                            "spawn rate: ", False, (0, 0, 0))
                        screen.blit(srLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 220))

                        if setSpawnRate:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 220, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 220, 25, 25])

                        srDisplay = myfont.render(
                            str(objects[tr][tc].spawnRate), False, (255, 255, 255))
                        screen.blit(srDisplay, (width - rightOffset +
                                                122, height - botOffset - 300 + 220))
                    # onion stun info
                    if objects[tr][tc].otype == "onion":
                        # self stun
                        stLabel = myfont.render(
                            "self stun dur: ", False, (0, 0, 0))
                        screen.blit(stLabel, (width - rightOffset +
                                              5, height - botOffset - 300 + 220))

                        if setSelfStunDur:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 220, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 220, 25, 25])

                        stDisplay = myfont.render(
                            str(objects[tr][tc].selfStunDur), False, (255, 255, 255))
                        screen.blit(stDisplay, (width - rightOffset +
                                                122, height - botOffset - 300 + 220))
                    if objects[tr][tc].otype == "onion" or objects[tr][tc].otype == "ram":
                        # player stun
                        pstLabel = myfont.render(
                            "plyr stun dur: ", False, (0, 0, 0))
                        screen.blit(pstLabel, (width - rightOffset +
                                               5, height - botOffset - 300 + 250))

                        if setPlayerStunDur:
                            pg.draw.rect(screen, orange, [width - rightOffset +
                                                          120, height - botOffset - 300 + 250, 25, 25])
                        else:
                            pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 250, 25, 25])

                        pstDisplay = myfont.render(
                            str(objects[tr][tc].playerStunDur), False, (255, 255, 255))
                        screen.blit(pstDisplay, (width - rightOffset +
                                                 122, height - botOffset - 300 + 250))
            else:
                square = None
                if editingLayer == "background":
                    square = background[tr][tc]
                elif editingLayer == "terrain":
                    square = terrain[tr][tc]
                nameLabel = myfont.render(
                    square.ttype + " (" + str(tr) + ", " + str(tc) + ")", False, (0, 0, 0))
                screen.blit(nameLabel, (width - rightOffset +
                                        5, height - botOffset - 300 + 10))

                # display texture info
                if setTexture:
                    pg.draw.rect(screen, orange, [width - rightOffset + 120, height - botOffset - 300 + 40, 25, 25])
                else:
                    pg.draw.rect(screen, black, [width - rightOffset +
                                                         120, height - botOffset - 300 + 40, 25, 25])
                textureLabel = myfont.render(
                    "texture: ", False, (0, 0, 0))
                screen.blit(textureLabel, (width - rightOffset +
                                           5, height - botOffset - 300 + 40))

                if editingLayer == "terrain":
                    textureDisplay = myfont.render(str(terrain[tr][tc].texture), False, (255, 255, 255))
                    screen.blit(textureDisplay, (width - rightOffset + 122, height - botOffset - 300 + 40))
                else:
                    textureDisplay = myfont.render(str(background[tr][tc].t_water), False, (255, 255, 255))
                    screen.blit(textureDisplay, (width - rightOffset + 122, height - botOffset - 300 + 40))

                if editingLayer == "terrain":
                    # display zone info
                    zonesStr = ','.join(str(s) for s in terrain[tr][tc].zone)
                    if zonesStr == "":
                        zonesStr = "none"
                    zonesLabel = myfont.render("zones:", False, (0, 0, 0))
                    screen.blit(zonesLabel, (width - rightOffset +
                                             5, height - botOffset - 300 + 70))
                    zonesDisplay = smallfont.render(zonesStr, False, (0, 0, 0))
                    screen.blit(zonesDisplay, (width - rightOffset +
                                               5, height - botOffset - 300 + 90))
    if textureHighlight != (-1, -1):
        pg.draw.rect(screen, orange,
                         [leftOffset + boxwidth * textureHighlight[1], topOffset + boxwidth * textureHighlight[0], (boxwidth - 1), (boxwidth - 1)], 3)

    pg.display.flip()
    fpsClock.tick(fps)
