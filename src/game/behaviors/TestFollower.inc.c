
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	gpf_ObjectInit(o);
}
void bhv_TestFollower_loop(void) {
	GraphPath *p = o->oPathLink;

	gpf_ObjectUpdate(o);

	// char t[50];
	// // char t2[50];
	// sprintf(t, "%f",
	// 	p->distances[0]
	// );
	// print_text(20, 20, t);


	// sprintf(t2, "%f %f %f",
	// 	p->distances[0],
	// 	p->distances[1],
	// 	p->distances[2]
	// );
	// print_text(20, 40, t2);
	// print_text_fmt_int(20, 60, "M %d", o->oPathLinkNum);

	// if (o->oPathLinkNum != p->mark) {
		// *(vs32*)o->oPathLinkNum=0;
		// *(vs32*)p->mark=0;
	// }
}
