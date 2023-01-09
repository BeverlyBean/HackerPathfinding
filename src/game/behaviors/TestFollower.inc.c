
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	opObjectInit();
}
void bhv_TestFollower_loop(void) {
	// gpf_3neighbors(o->oPathLink);
	opUpdatePosition();
	opFollow();
}
