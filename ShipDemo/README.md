# Ship Demo

This is a simple application to show off device-independent control and layered parallax.
To control the ship, you use the arrow keys on the desktop and touch the screen on mobile.
When you touch the screen, the ship will go towards where you touch.

The parallax consists of three layers: the space background, the planets, and the ship.
This scene is entirely defined in the file assets.json.  There is no layout code in
the game.

When the ship "moves", we actually move the background in the opposite direction.  This
allows us to keep the ship in the center of the screen where the player can see it.
This library is a C++ cross platform game library for OS X, iOS, Windows, and Android. 
Its primary focus are the mobile platforms iOS and Android.