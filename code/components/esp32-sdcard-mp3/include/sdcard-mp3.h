#ifndef SDCARD_MP3_H
#define SDCARD_MP3_H

/*
Starts the task in the mp3 player so the mp3 can work with queue's
(Call this function if you want to work with queue's)

-Parameter: NULL
*/
void mp3_task(void*);

/*
Stops the task in the mp3 player
(After calling this function, the mp3 cannot work with queue's anymore)
*/
void mp3_stopTask();

/*
Adds an item to the queue of the mp3 player and plays this mp3 file when its the first in the queue

-Parameters: A string (char*) with the filepath to the .mp3 file
*/
void mp3_addToQueue(char*);

/*
Plays an .mp3 file directly. When another file is playing, it will be interrupted.

-Parameters: A string (char*) with the filepath to the .mp3 file
*/
void mp3_play(char*);

/*
Stops the mp3 player from being able to play .mp3 files.
(Call this function when exiting the application)
*/
void mp3_stop();

#endif