# Polygon Demo

This library shows off how to use the various polygon scene graph nodes: PolygonNode,
WireNode, and PathNode.  The shapes are all defined in the scene graph specified in
assets.json.  There is no explict layout code.  To see how to work with polygons
programmatically, look at the Rocket Demo.

This demo also shows off panning.  On desktop, you drag the screen about with the mouse.
On mobile, you "pan", which is a two finger drag.  As a word of warning, two finger 
drag is a little finicky on Android.  If your fingers are two close together, they may
register as just one finger.  You may need to spread your fingers a little bit to get
it to work.
