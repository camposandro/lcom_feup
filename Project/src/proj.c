#include <minix/syslib.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include "game.h"

int main() {

	sef_startup();
	sys_enable_iop(SELF);

	srand(time(NULL));

	Game* game = (Game *) malloc(sizeof(Game));
	if (game == NULL) {
		printf("Could not allocate memory for game!\n");
		return 1;
	}

	game->current_state = INIT;
	handle_event(game, NO_EVENT);
	return 0;
}
