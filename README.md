# TA3D version 0.5.4

this software is under GPL (see COPYING)

TA3D is a "clone" of Total Annihilation but in real 3D using OpenGl. It is not finished yet, but it has some features from the original game:
  * support for "weapons"
  * allow moving of units
  * allow building
	* units are animated with scripts from the original game(stable but not finished)
	* full loading support of maps from original game, with perspective correction system
	* shadows of units and buildings (using shadow volume)
	* automatic loading of detected units (in TA3D's directory)
	* particle engine
	* user interface on the model of TA's one (not finished)
	* mouse commands on the rendered terrain
	* run under Linux (developped under Linux)
	* run under windows
	* enhanced explosion effects
	* campaign mode

and additionnal features:
	* water effect for oceans, lakes and rivers, and lava effect
	* a console that can affect the game (killing units, ...)

work in progress:
	* A.I. (it should be able to learn when finished)

planed but not sure:
	* a random map generator
	* a new map format, with more details
	* a system to make the terrain destructible

and probably more!

NB: TA3D requires Allegro v 4.2.2 or better, AllegroGL >= 0.4.3, FMODex and zlib

## INSTALLATION

For details about compiling the code see INSTALL

TA3D needs TA ressource files to work, if you have the first TA CD you can extract totala1.hpi and totala2.hpi with this command: `./install_sh` which in fact will run `./ta3d install`. Then follow on screen instructions.

If you don't have the first TA CD, you can get *.hpi/*.ufo files which contain maps/units/weapons/... from a mod or the demo.

## Running

To launch TA3D just type ./ta3d , it will launch the menu allowing you to launch a game and select a map console can be enabled with ")".("²" under windows)
