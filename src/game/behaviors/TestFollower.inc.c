
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	opObjectInit();
}
void bhv_TestFollower_loop(void) {
	gpf_3neighbors(o->oPathLink);
	opUpdatePosition();
	opFollow();

	// print_text_fmt_int(50, 50, "A %d", o->oAction);


	// for (int i = 0; i < o->oPathWorkLen; i++) {
		char t[50];
		char t2[50];
		// GraphPath *p = o->oPathWork[i];
		GraphPath *p = o->oPathLink;
		// sprintf(t, "%x %d", p, p->p);
		// if (p == NULL) return;
		sprintf(t, "%x %d %d",
			p,
			p->mark,
			p->init
		);
		sprintf(t2, "%.2f %.2f %.2f",
			p->distances[0],
			p->distances[1],
			p->distances[2]
		);
		print_text(20, 36, t);
		print_text(20, 52, t2);
		for (int i = 1; i <= 3; i++) {
			char r[50];
			sprintf(r, "%x", p->neighbors[i-1]);
			print_text(20, 56+(i*16), r);
		}
		// print_text(20, 36 + (i*16), t);
	// }

	print_text_fmt_int(20, 20, "A %d", o->oPathWorkLen);

}
