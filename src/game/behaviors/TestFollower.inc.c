
#include "game/graphpath.h"

void bhv_TestFollower_init(void) {
	o->oFollower = 1;
	gpf_ObjectInit(o);

	gpf_InitPath(o);
}
void bhv_TestFollower_loop(void) {
	static s32 switcher = 0;

	GraphPath *p = o->oPathLink;

	gpf_ObjectUpdate(o);

	gpf_MakePath(o, TARGET_PATH);
	gpf_FollowPath(o);

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
	// 	print_text(50, 50 + (i* 10), t);
	// }

	// } else {

	// }
}
