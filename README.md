# IT-276-Pep-s-Spicy-Adventure-

2D side scrolling shoot-em-up, created using SDL 2, written in C

You play as pep, the pepper. You fire spice at all the not spicy people.

WASD to move around on the screen.
Click the mouse to fire pepper shots. Hold the click down for a second to charge your shot and fire a big spice shot.
Press space to use a bomb that will clear the screen of all enemies.

Pep's enemies will try and fire projectiles at him or move to touch him. Either of which will result in the loss of a life for pep.
Melt - ice cream cone that moves towards pep firing bullets staright ahead.
Milk Tank - milk carton that moves towards pep trying to touch him, takes 5 spice shots to kill or one charge shot/bomb.
Celery stalker - celery stalk that sees pep, waits a moment, then charges at him trying to  touch him.
Professor Slice - slice of bread that runs away from pep, covering the screen with bread crumb bombs.
Clarence - slice of cheese that waits at the top of the screen for pep to pass under him and falls towards him, can only be killed in falling state or with a bomb.

Power ups will stay still on the screen while the camera moves towards them, they each take effect after touching them.
D - double tap, will let pep fire faster than previously.
S - spread shot, will make pep fire extra bullets upwards and downwards, effect stacks on successive pickups
H - heat shield, will give pep a shield that protects him from one hit
G - Goo shot, makes the enemies stick to a spot on screen, if pep touches them in this state he still dies.

Make it to the end of each stage and you will progress to the next.
After completing the final stage the program will exit.

Pep starts with 3 lives and 2 bombs.
There are bomb pick ups and extra lifes found in the stages.

////////////////////////////////ADDITIONS SINCE MIDTERM///////////////////////////////////////////////

There are several new game modes including: Editor Mode, Challenge Mode, Arcade Mode, and Endless Mode.
Arcade Mode - play through the three pre made levels of the game. Saving scrore after completeing levels and at exit updating the                    highscore list with the new score
Editor Mode - Create a level clicking the prev and next buttons to cycle through which entity to place in the game world (by clciking)               next, at exit will save the entities placed on the screen to a level file, which is loaded by challenge mode
Challenge Mode - play through created level, updating highscores on victory
Endless Mode - Play through endless waves of enemies, that will spawn more and more frequently as time progresses.

New systems include buttons, audio, parallax scrolling backgrounds, HUD, and bloom effects as well as additions to enemy behavior (Sinodal Movement) and particle effects (Particle Explosion, Particle Trail, and Particle Blast).


"music by audionautix.com"


