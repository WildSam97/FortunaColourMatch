# FortunaColourMatch
A small colour matching game for the LaFortuna Board.

# Game instructions
On the menu, choose your difficulty. As the on screen instructions say, press left for easy and right for hard.
In easy mode, the goal blocks do not move.
In hard mode, there is a random chance that the goals will move around screen.

In game, you control the small block.
To score a point, use the compass direction buttons to move the block inside the larger goal block and use the rotary wheel to change the colour of your block to match the goal colour.
Scoring a point adds time to the game timer, extending the length of the game.
Throughout the game, the timer will tick down.
Once the timer reaches 0 the game is over.
Pressing the centre button when the game is over will return you to the main menu.

# Flashing Instructions
(Assuming you are using the AVR Tool Chain from the COMP2215 wiki)
On windows: Simply open a command prompt in the main folder and use "make upload" 
(note: the included makefile requires all files to be in the same folder, subfolders did not work for me when using the makefile from the wiki)
Other Operating Systems: I have not tested compilation for other operating systems as I use windows, however I imagine the process you used for the lab tasks will also work for this.

Once flashed, remember to hard restart the LaFortuna using the "BAT/USB" switch before attempting to play. The software reset from the flashing process did not work properly for me when testing, however a hard restart fixed any issues.

# Included Code
+ Steve Gunn's led, lcd and rotary libraries.
+ A modified version of Switches.c from Phill Raynsford's FortunaTetris
+ rand_init and rand functions from  Giacomo Meanti's Space Invaders game
