
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	opObjectInit();
}
void bhv_TestFollower_loop(void) {
	GraphPath *p = o->oPathLink;

	opGetNeighbors();
	opFollow();

	char t[50];
	char t2[50];
	sprintf(t, "%x %x %x",
		p->neighbors[0],
		p->neighbors[1],
		p->neighbors[2]
	);


	sprintf(t2, "%f %f %f",
		p->distances[0],
		p->distances[1],
		p->distances[2]
	);
	print_text(20, 20, t);
	print_text(20, 40, t2);
}
