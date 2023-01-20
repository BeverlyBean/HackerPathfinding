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

GraphPath *graphStart;
GraphPath *graphEnd;

f32 vec3f_dist(Vec3f v0, Vec3f v1) {
    return sqrtf(
          (v0[0] * v1[0])
        + (v0[1] * v1[1])
        + (v0[2] * v1[2])
    );
}

GraphPath gGraphPool[GPF_SIZE];

// worst case is that i need all this space
int gPathIdx = 0;
struct GraphPath *gPathWork[OBJECT_POOL_CAPACITY][GPF_SIZE];
enum GPFState {
    GPFS_FINDNODE,
    GPFS_GOTO_NODE,
    GPFS_INC_PATH,
};


/*
    A Graph traversal node system that objects can use to get fron point A to point B

    A custom object will act as a "node":
    - init() registers its position and its 3 closest neighbors if not obscured by something like a wall (check with raycasts)
    - loop() will do nothing

    A struct for a Graph {
      Vec3f position;
      Graph *neighbors[3];
      f32 distances[3];
    };

    An API will act as a pathfinder
    -init() will initialize a Graph struct and pool
    -find(Graph *path, Vec3f from, Vec3f to); will traverse the Graph and provide a list of nodes to follow
    --registers "shadow" graphnodes for at least the `to` position to simplify code
    --returns 1 if path has no neighbors and (path->position == o->pos)
    --maybe provide the face angle to determine where the path starts?
    -follow(Vec3f position, Graph *dest) to move towards individual graph nodes


    Objects will use the API
    -an object will be expected to travel from its current position to the first graph node, and from the last graph node to the destination
*/

int gpf_getPath() {
    for (int i = 1; i < GPF_SIZE; i++) {
        if (gGraphPool[i].init == 0) {
            return i;
        }
    }
    return -1;
}

void gpf_register(struct Object *obj) {
    int r = gpf_getPath();
    assert(r != -1, "BAD POP");

    GraphPath *p = &gGraphPool[r];
    p->init = 1;
    p->mark = r;
    p->objLink = obj;

    vec3f_copy(p->position, &o->oPosX);

    obj->oPathLink = p;
    obj->oPathLinkNum = r;
}

void mario_graphpath_init() {
    gGraphPool[0].init = 1;
    gGraphPool[0].mark = 0;
    gGraphPool[0].objLink = o;

    o->oPathLink = gMarioState->pathLink = &gGraphPool[0];
    vec3f_copy(gGraphPool[0].position, gMarioState->pos);
}

void mario_graphpath_update() {
    vec3f_copy(gGraphPool[0].position, gMarioState->pos);
}

void opGotoPath(GraphPath *p) {
    o->oForwardVel = 8.0f;
    o->oMoveAngleYaw = obj_angle_to_object(o, p->objLink);
}

void opObjectInit() {
    gpf_register(o);
    o->oPathWork = gPathWork[gPathIdx++];
    o->oPathWorkIdx = 0;
}

void opFollow() {
    GraphPath *p  = o->oPathLink;
    GraphPath *m  = gMarioState->pathLink;
    // opGotoPath(o->oPathLink);
    opGotoPath(m);
}

