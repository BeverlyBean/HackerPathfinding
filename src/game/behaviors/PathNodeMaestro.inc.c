
#define oMaestroNodeCount OBJECT_FIELD_S32(0x1B)

void bhv_PathNodeMaestro_init(void) {
	o->oMaestroNodeCount = 0;
	o->oAction = 0;
}


void bhv_PathNodeMaestro_loop(void) {
	s32 oc = gpf_count();
	switch (o->oAction) {
		case 0: {
			if (oc > o->oMaestroNodeCount) {
				o->oMaestroNodeCount = oc;
			} else if (oc == o->oMaestroNodeCount) {
				o->oAction++;
			}
		}
		break;
		case 1: {
			gpf_setup_neighbors();
			o->oAction++;
		}
		break;
		case 2: {
			// either:
			// mark_obj_for_deletion(o);
			// or:
			// set up neighbors if e.g. a new path node is spawned
			if (oc != o->oMaestroNodeCount) {
				o->oMaestroNodeCount = oc;
				o->oAction = 1;
			}
		}
	}

	print_text_fmt_int(50,50, "%d", oc);
}
