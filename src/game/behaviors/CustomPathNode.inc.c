
#include "game/graphpath.h"

void bhv_CustomPathNode_init(void) {
	gpf_ObjectInit(o);
}
void bhv_CustomPathNode_loop(void) {
	GraphPath *p = o->oPathLink;

	gpf_ObjectUpdate(o);
	// if (p->mark) {
	// 	spawn_orange_number(o->oPathLinkNum % 10, 0, 0, 0);
	// 	p->mark = 0;
	// }



	if (p) {
		for (int i = 0; i < NEIGHBORSIZE; i++) {
			GraphPath *n = p->neighbors[i];
			if (n) {
				if (n->objLink) {
					s16 angle = obj_angle_to_object(o, n->objLink);
				}
			}
		}
	}

	f32 dist = 0.0f;

	if (o->oBehParams == 0) {
		if (p->objects[0]) {
			obj_turn_toward_object(o, p->objects[0], O_MOVE_ANGLE_YAW_INDEX, 0x400);
		}
	}

	if (p->neighbors[0] == 0) {
		o->header.gfx.scale[1] = 0;
	} else {
		o->header.gfx.scale[1] = 2;
	}

	// else {
	// 	obj_turn_toward_object(o, gMarioObject, O_MOVE_ANGLE_YAW_INDEX, 0x400);
	// }
}

