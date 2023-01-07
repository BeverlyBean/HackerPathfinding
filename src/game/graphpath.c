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
int graphPathInit = 1;
void gpf_init() {
    bzero(gGraphPool, sizeof(gGraphPool));
    bzero(gPathWork, sizeof(gPathWork));
    graphPathInit = 1;

    // reserve [0] for mario
    gGraphPool[0].init = 1;
    gGraphPool[0].objLink = gMarioObject;
    gMarioState->pathLink = &gGraphPool[0];
    vec3f_copy(gGraphPool[0].position, gMarioState->pos);
}

void mario_graphpath_update() {
    vec3f_copy(gGraphPool[0].position, gMarioState->pos);
    gpf_3neighbors(&gGraphPool[0]);
}

int gpf_pop() {
    for (int i = 0; i < GPF_SIZE; i++) {
        if (gGraphPool[i].init == 0) {
            gGraphPool[i].init = 1;
            return i;
        }
    }
    return -1;
}

void gpf_closest_node(GraphPath **a, Vec3f pos) {
    float max = 99999999.9f;
    int max_i = -1;
    for (int i = 0; i < GPF_SIZE; i++) {
        if (gGraphPool[i].init == 0) {
            continue;
        }
        float vd = vec3f_dist(gGraphPool[i].position, pos);
        if (vd < max) {
            max = vd;
            max_i = i;
        }
    }

    *a = &gGraphPool[max_i];
}

void gpf_write(GraphPath *p, Vec3f pos) {

    p->position[0] = pos[0];
    p->position[1] = pos[1];
    p->position[2] = pos[2];
}

void gpf_3neighbors(GraphPath *p) {
    // quick+dirty triple minimum algorithm
    for (int i = 0; i < GPF_SIZE; i++) {
        // quick + dirty optimization
        if (gGraphPool[i].init == 0) continue;

        GraphPath *currNode = &gGraphPool[i];

        float dist_to_node = vec3f_dist(p->position, currNode->position);
        if (dist_to_node == 0) continue;

        if (dist_to_node < p->distances[0]) {
            p->distances[2] = p->distances[1];
            p->distances[1] = p->distances[0];
            p->distances[0] = dist_to_node;


            p->neighbors[2] = p->neighbors[1];
            p->neighbors[1] = p->neighbors[0];
            p->neighbors[0] = currNode;
        } else if (dist_to_node < p->distances[1]) {
            p->distances[2] = p->distances[1];
            p->distances[1] = dist_to_node;

            p->neighbors[2] = p->neighbors[1];
            p->neighbors[1] = currNode;
        } else if (dist_to_node < p->distances[2]) {
           p->distances[2] = dist_to_node;

           p->neighbors[2] = currNode;
        }
    }

    // check if a neighbor has a wall
    for (int i = 0; i < 3; i++) {
        Vec3f dir, throw;
        struct Surface *s = NULL;

        find_surface_on_ray(p->position, dir, &s, throw, RAYCAST_FIND_FLOOR | RAYCAST_FIND_CEIL | RAYCAST_FIND_WALL);

        if (s != NULL) {
            p->neighbors[i] = NULL;
            p->distances[i] = 99999.0f;
        }
    }
}

void gpf_register(struct Object *ob) {
    int r = gpf_pop();


    // epic double link
    ob->oPathLink = &gGraphPool[r];
    gGraphPool[r].objLink = ob;    
    ob->oPathLinkNum = r;


    gGraphPool[r].mark = r;

    gGraphPool[r].position[0] = o->oPosX;
    gGraphPool[r].position[1] = o->oPosY;
    gGraphPool[r].position[2] = o->oPosZ;
}

int gPathsFound = 0;


int isaddr(void *v) {
    u32 a = (u32)v;


    return ((a & 0xFF000000) == 0x80000000);
}


static int min3v(Vec3f s) {
    float a = s[0], b = s[1], c = s[2];

    int ret = 0;

    if (b < a) {
        a = b;
        ret = 1;
    }
    if (c < a) {
        a = c;
        ret = 2;
    }
    return ret;
}

int gpf_pathfind(struct Object *oFrom, struct Object *oTo) {
    GraphPath **path = oFrom->oPathWork;

    if (gPathsFound == 0) {
        for (int i = 0; i < GPF_SIZE; i++) {
            if (gGraphPool[i].init == 0) continue;
            gpf_3neighbors(&gGraphPool[i]);
        }
        gPathsFound = 1;
    }

    int pathIdx = 0;


    GraphPath *p = oFrom->oPathLink;


    // start with a super naive approach instead of a real algorithm
    while (isaddr(p) && p != oTo->oPathLink) {
        path[pathIdx] = p;
        p = p->neighbors[min3v(p->distances)];
        pathIdx++;
    }

    o->oPathWorkLen = pathIdx;

    if (pathIdx == 0) {
        // we are already at the end
        // (does this condition ever happen?)
        return 1;
    }
    return 0;
}



// op = ObjectPath

#define o gCurrentObject

void opObjectInit() {
    o->oPathWork = gPathWork[gPathIdx++];
    for (int i = 0; i < GPF_SIZE; i++) {
        o->oPathWork[i] = 0;
    }
    o->oAction = GPFS_FINDNODE;
    o->oPathWorkLen = 0;
}


void opGetPath(struct Object *to) {
    if (o->oPathWork[0] == 0) {
        gpf_pathfind(o, to);
        o->oPathWorkIdx = 0;
    }
}


void goto_vec(struct Object *obj, GraphPath *p) {
    if (isaddr(obj) == 0) return;
    if (isaddr(p) == 0) return;
    f32 dx = p->position[0] - obj->oPosX;
    f32 dy = p->position[1] - obj->oPosY + 120.0f;
    f32 dz = p->position[2] - obj->oPosZ;
    s16 targetPitch = atan2s(sqrtf(sqr(dx) + sqr(dz)), dy);

    obj_turn_toward_object(o, p->objLink, O_MOVE_ANGLE_YAW_INDEX, 0x1000);

    obj->oMoveAnglePitch = approach_s16_symmetric(obj->oMoveAnglePitch, targetPitch, 0x1000);
    obj->oVelY = sins(obj->oMoveAnglePitch) * 10.0f;
    obj->oForwardVel = coss(obj->oMoveAnglePitch) * 10.0f;
}

void opGo(GraphPath *p) {
    if (p) {
        goto_vec(o, p);
    }
}

void opUpdatePosition() {
    GraphPath *p = o->oPathLink;

    vec3f_copy(p->position, &o->oPosX);
    gpf_3neighbors(p);
}

void opFollow() {
    switch (o->oAction) {
        case GPFS_FINDNODE:
            opGetPath(gMarioObject);
            o->oAction = GPFS_GOTO_NODE;
            break;
        case GPFS_GOTO_NODE:
            // opGo(o->oPathWork[o->oPathWorkIdx]);
            // break;
    }
}



