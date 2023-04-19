
#include "game/graphpath.h"

void bhv_CustomPathNode_init(void) {
	gpf_ObjectInit(o);
}
void bhv_CustomPathNode_loop(void) {
	static s32 switcher = 0;
	GraphPath *p = o->oPathLink;

	if (o->oTimer == 1) {
		gpf_ObjectUpdate(o);
	}
	// if (p->mark) {
	// 	spawn_orange_number(o->oPathLinkNum % 10, 0, 0, 0);
	// 	p->mark = 0;
	// }

	f32 dist = 0.0f;

	if (o->oTimer % 30 == 0) {
		switcher++;
		if (switcher > 2) switcher = 0;
	}

	if (p->objects[switcher]) {
		obj_turn_toward_object(o, p->objects[switcher], O_MOVE_ANGLE_YAW_INDEX, 0x400);
	} else {

	}

	// print_text_fmt_int(50, 50, "P %d", o->oTimer);

	// else {
	// 	obj_turn_toward_object(o, gMarioObject, O_MOVE_ANGLE_YAW_INDEX, 0x400);
	// }
}

