#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

void radio_init();
void radio_stop();

/*
Switches the radio to another channel

-Parameters: The name of the channel to switch to
*/
void radio_switch(char[]);

/*
Starts the task of playing the radio (+ starts all the pipelines, streams, etc.)
*/
void radio_task(void*);

/*
Stops the radio from playing, call radio_switch() to play a radio station again
*/
void radio_reset();

/*
Stops the task of the radio. After calling this function
the radio_switch() does not work anymore
*/
void radio_quit();

#endif
