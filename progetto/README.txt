Project by Group 31 (Filippo Garetti, Marco Mezzanotte, Tommaso Calcina)

This is the audio feedback for the pedometer.

It'll work only if 
	>you insert into the Makefile, 
	 in the variable SRC this: prototipoG31/player.cppprototipoG31/slice-and-play.cpp prototipoG31/adpcm.c  .

	>you include "slice-and-play.h" in your main file

	>you use miosix
	
	
It has inside a converter to create new file suitable for the player.

The class to call all the different functionalities is "ring".

This are the functionalities:

> void play_n_of_step(int)

	it gets an int variable and it will play "Hai fatto"-number of step-"passi"

> void looserSong()

	it will play "Hai perso"

>void victorySong()
	
	it will play "Hai vinto"
