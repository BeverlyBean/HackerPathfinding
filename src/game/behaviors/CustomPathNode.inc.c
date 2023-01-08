
#include "game/graphpath.h"

void bhv_CustomPathNode_init(void) {
	if (graphPathInit == 0) {
		gpf_init();
		graphPathInit = 1;
	}

	gpf_register(o);
}
void bhv_CustomPathNode_loop(void) {
	GraphPath *p = o->oPathLink;
	gpf_3neighbors(p);
	if (p->mark) {
		spawn_orange_number(o->oPathLinkNum % 10, 0, 0, 0);
		p->mark = 0;
	}
}
