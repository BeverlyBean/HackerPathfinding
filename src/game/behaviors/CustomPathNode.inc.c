
#include "game/graphpath.h"

#define oPointIndex OBJECT_FIELD_S32(0x1F)
void bhv_CustomPathNode_init(void) {
	gpf_PathInit(o);
	o->oPointIndex = 0;
	o->oTimer = 0;
	// mark_obj_for_deletion(o);
}
void bhv_CustomPathNode_loop(void) {
	// osSyncPrintf("Heartbeat");
	// debug process to highlight neighbors
	GraphPath *p = o->oPathLink;


	if ((o->oTimer > 0) && (o->oTimer % 60 == 0)) {
		o->oPointIndex++;
		if (o->oPointIndex >= NEIGHBORSIZE) {
			o->oPointIndex = 0;
		}
	}


	if (p->neighbors[o->oPointIndex]) {
		cur_obj_unhide();
		obj_turn_toward_pos(o, p->neighbors[o->oPointIndex]->position, O_MOVE_ANGLE_YAW_INDEX, 0x400);
		if (p->neighbors[o->oPointIndex] == TARGET_PATH) {
			osSyncPrintf("I %d AM POINTING AT MARIO", p->_index);
		}
	} else {
		cur_obj_hide();
	}


	// print_text_fmt_int(50, 50, "%d", o->oPointIndex);
}
