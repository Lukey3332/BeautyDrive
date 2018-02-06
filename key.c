#include "util.h"
#include "sys.h"
#include "in.h"
#include "game.h"
#include "SDL.h"
#include "uthash.h"

typedef struct {
	uint keycode;
	uint command;
	UT_hash_handle hh;
} keysym;

keysym * keymap = NULL;

void map_key (uint keycode, uint command) {
	keysym * tmp;
	HASH_FIND_INT( keymap, &keycode, tmp);
	if(!tmp) {
		tmp = malloc(sizeof(keysym));
		tmp->keycode = keycode;
		tmp->command = command;
		HASH_ADD_INT( keymap, keycode, tmp);
	} else {
		tmp->command = command;
	}
}

void HandleKey (uint keycode, char down)
{
	keysym * tmp;
	HASH_FIND_INT( keymap, &keycode, tmp);
	if(!tmp) return;
	switch(tmp->command) {
		case CMD_QUIT:
			Sys_Quit();
	}
}
