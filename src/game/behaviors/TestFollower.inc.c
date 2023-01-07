
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	opObjectInit();
}
void bhv_TestFollower_loop(void) {
	opUpdatePosition();
	opFollow();

	// print_text_fmt_int(50, 50, "A %d", o->oAction);


	for (int i = 0; i < o->oPathWorkLen; i++) {
		char t[50];
		GraphPath *p = o->oPathWork[i];
		sprintf(t, "%x %d %d", o->oPathWork[i], p->mark, p->init);
		print_text(50, 20 + (i*20), t);
	}

}
