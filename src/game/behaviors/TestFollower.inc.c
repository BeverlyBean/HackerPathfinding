
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	o->oFollower = 1;
	gpf_ObjectInit(o);

	gpf_InitPath(o);

	o->oAction = 0;
}
void bhv_TestFollower_loop(void) {
	static s32 switcher = 0;

	GraphPath *p = o->oPathLink;

	gpf_ObjectUpdate(o);

	switch (o->oAction) {
		case 0:
			if (gpf_AllPathsRegistered()) {
				gpf_MakePath(o, TARGET_PATH);
				o->oAction ++;
			}
			break;
		case 1:
			GraphPath **pat = o->oPathWork;
			for (int i = 0; i < GPF_SIZE; i++) {
				if (pat[i]) {
					print_text_fmt_int(50, 10 + (i*10), "%08X", pat[i]);
				}
			}
			gpf_FollowPath(o);
			break;
	}

	// if (o->oTimer % 30 == 0) {
	// 	switcher++;
	// 	if (switcher > 2) switcher = 0;
	// }

	// if (p->neighbors[switcher]) {
		// obj_turn_toward_pos(o, p->neighbors[switcher]->position, O_MOVE_ANGLE_YAW_INDEX, 0x400);
	// }

	// for (int i = 0; i < NEIGHBORSIZE; i++) {
	// 	char t[50];
	// 	sprintf(t, "%08X %f", p->neighbors[switcher], p->distances[switcher]);
	print_text_fmt_int(10, 10, "%d", o->oPathWorkIdx);
	// }

	// } else {

	// }
}
