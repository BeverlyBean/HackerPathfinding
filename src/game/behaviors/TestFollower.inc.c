
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	opObjectInit();
}
void bhv_TestFollower_loop(void) {
	GraphPath *p = o->oPathLink;

	opGetNeighbors();
	opFollow();

	char t[50];
	sprintf(t, "%f %f",
		gMarioState->pos[0],
		gMarioState->pos[2]
	);
	print_text(20, 20, t);
}
