#include <PR/ultratypes.h>

#include "config.h"
#include "area.h"
#include "sm64.h"
#include "object_list_processor.h"
#include "object_fields.h"
#include "level_update.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "graphpath.h"
#include "debug.h"

typedef struct Object Obj;

GraphPath *graphStart;
GraphPath *graphEnd;


int dbgPathCount = 0;
#define isaddr(a) (((u32)(a) & 0xFF000000) == 0x80000000)

GraphPath gGraphPool[GPF_SIZE];
int gPoolIdx = 1; // 0 is mario

// worst case is that i need all this space
int gPathIdx = 1; // 0 is mario
struct GraphPath *gPathWork[OBJECT_POOL_CAPACITY][GPF_SIZE];
enum GPFState {
    GPFS_FINDNODE,
    GPFS_GOTO_NODE,
    GPFS_INC_PATH,
};

static void __gpf_ctor(GraphPath *p) {
    p->init = 1;
    p->neighbors[0] = 
    p->neighbors[1] = 
    p->neighbors[2] = NULL;

    p->distances[0] = 
    p->distances[1] = 
    p->distances[2] = 0.0f;
}

Obj *__gpf_NearestObj(int i, GraphPath *p) {
    Obj *_o = p->objLink;

    extern const BehaviorScript bhvCustomPathNode[];
    uintptr_t *behaviorAddr = segmented_to_virtual(bhvCustomPathNode);
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    Obj *obj = (Obj *) listHead->next;
    Obj *closestObj = NULL;
    f32 minDist = 0x20000;

    f32 maxDist = 0;
    switch (i) {
        case 0: break;
        case 1: maxDist = p->distances[0]; break;
        #define __max(a, b) (((a) <= (b)) ? (b) : (a))
        case 2: maxDist = __max(p->distances[1], p->distances[2]); break;
    }

    int __i = 0;
    while (obj != (Obj *) listHead) {
        if (obj->behavior == behaviorAddr
            && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && obj != _o
        ) {
            f32 objDist = dist_between_objects(_o, obj);
            if ((objDist <= minDist)
             && (objDist > maxDist)
            ) {
                // skip that if theres a wall
                struct Surface *hit = NULL;
                Vec3f hit_pos;
                Vec3f dir;
                Vec3f oPosSource, oPosDest;
                vec3f_copy(oPosSource, &_o->oPosX);
                vec3f_copy(oPosDest, &obj->oPosX);

                oPosSource[1] += 15;
                oPosDest[1] += 15;

                vec3f_diff(dir, oPosDest, oPosSource);
                find_surface_on_ray(
                    oPosSource,
                    dir,
                    &hit,
                    hit_pos,
                    RAYCAST_FIND_FLOOR | RAYCAST_FIND_WALL
                );

                if (hit == NULL) {
                    closestObj = obj;
                    minDist = objDist;
                }
            }
        }

        obj = (Obj *) obj->header.next;
        __i++;
    }

    p->distances[i] = minDist;

    // char buf[50];
    // sprintf(buf, "went through %d entries idx %d bhv %08X", __i, i, behaviorAddr);
    // assert(closestObj != NULL, buf);
    return closestObj;
}


static GraphPath *__gpf_Pop() {
    GraphPath *ret = &gGraphPool[gPoolIdx++];
    __gpf_ctor(ret);
    return ret;
}

static void __gpf_NeighborFunc(Obj *oo) {
    GraphPath *p = oo->oPathLink;

    // todo: replace with 3-minimum function that i couldnt get working before
    for (int i = 0; i < NEIGHBORSIZE; i++) {
        if (p->neighbors[i] != NULL) {
            if (p->neighbors[i]->init == 1) continue;
            else {
                assert(0, "how did we get here");
            }
        }

        p->objects[i] = __gpf_NearestObj(i, p);


        Obj *oi = p->objects[i];
        if (oi == NULL) continue;

        if (oi->oPathLink == NULL) {
            gpf_ObjectInit(oi);

        }
        p->neighbors[i] = oi->oPathLink;
    }
}

static void __gpf_Link(Obj *oo, GraphPath *p) {
    oo->oPathLink = p;
    p->objLink = oo;
    vec3f_copy(p->position, &o->oPosX);

    // viral init?
    __gpf_NeighborFunc(oo);
}

// this will implement sorting
void gpf_setup_neighbors() {

}

void gpf_ObjectInit(Obj *oo) {
    GraphPath *p = __gpf_Pop();

    __gpf_Link(oo, p);
}

void gpf_ObjectUpdate(Obj *oo) {
    GraphPath *p = oo->oPathLink;

    vec3f_copy(p->position, &o->oPosX);

    __gpf_NeighborFunc(oo);
}

void mario_graphpath_init() {
    __gpf_Link(gMarioObject, &gGraphPool[0]);
}

void mario_graphpath_update() {
    __gpf_NeighborFunc(gMarioObject);
}
