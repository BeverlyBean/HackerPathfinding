
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	o->oFollower = 1;
	gpf_ObjectInit(o);

	gpf_InitPath(o);
	gpf_MakePath(o, gMarioObject);
}
void bhv_TestFollower_loop(void) {
	static s32 switcher = 0;

	GraphPath *p = o->oPathLink;

	gpf_ObjectUpdate(o);

	gpf_FollowPath(o);

	if (o->oTimer % 30 == 0) {
		switcher++;
		if (switcher > 2) switcher = 0;
	}

	// if (p->objects[switcher]) {
	// 	obj_turn_toward_object(o, p->objects[switcher], O_MOVE_ANGLE_YAW_INDEX, 0x400);

	// 	char t[50];
	// 	sprintf(t, "%08X %f", p->objects[switcher], p->distances[switcher]);
	// 	print_text(50, 50, t);

	// } else {

	// }

	// char t[10][50];
	// GraphNode **pl = o->oPathWork;
	// for (int i = 0; i < 10; i++) {
	// 	sprintf(t[i], "%08X",
	// 		pl[i]
	// 	);
	// 	print_text(20, 20 + (10 * i), t[i]);
	// }
	char t2[50];
	sprintf(t2, "%d", o->oPathWorkIdx);
	print_text(20, 40, t2);
}
