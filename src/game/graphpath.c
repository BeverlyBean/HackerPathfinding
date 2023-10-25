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
#define INFINITY 99999.9f

// #define static

GraphPath *graphStart;
GraphPath *graphEnd;

extern const BehaviorScript bhvCustomPathNode[], bhvTestFollower[], bhvMario[];


int dbgPathCount = 0;
#define isaddr(a) (((u32)(a) & 0xFF000000) == 0x80000000)

GraphPath gGraphPool[GPF_SIZE];
int gPoolIdx = 1; // 0 is mario

// worst case is that i need all this space
struct GraphPath *gPathWork[FOLLOWER_COUNT][GPF_SIZE];
int gPathIdx = 1; // 0 is mario

f32 __gpf_distance(GraphPath *g1, GraphPath *g2) {
    Vec3f d;
    vec3_diff(d, g2->position, g1->position);
    return vec3_mag(d);
}

s32 obj_turn_toward_pos(struct Object *obj, Vec3f target, s16 angleIndex, s16 turnAmount) {
    Vec3f d;
    s16 targetAngle = 0x0;
    s16 startAngle;

    switch (angleIndex) {
        case O_MOVE_ANGLE_PITCH_INDEX:
        case O_FACE_ANGLE_PITCH_INDEX:
            d[0] = target[0] - obj->oPosX;
            d[1] = -target[1] + obj->oPosY;
            d[2] = target[2] - obj->oPosZ;

            targetAngle = atan2s(sqrtf(sqr(d[0]) + sqr(d[2])), d[1]);
            break;

        case O_MOVE_ANGLE_YAW_INDEX:
        case O_FACE_ANGLE_YAW_INDEX:
            d[0] = target[0] - obj->oPosX;
            d[2] = target[2] - obj->oPosZ;

            targetAngle = atan2s(d[2], d[0]);
            break;
    }

    startAngle = o->rawData.asU32[angleIndex];
    o->rawData.asU32[angleIndex] = approach_s16_symmetric(startAngle, targetAngle, turnAmount);
    return targetAngle;
}

// static f32 __gpf_distance(GraphPath *s, GraphPath *d) {
//     Vec3f sv; vec3f_copy(sv, s->position);
//     Vec3f dv; vec3f_copy(dv, d->position);

//     f32 di = INFINITY;
//     vec3f_get_dist(sv, dv, &di);

//     return di;
// }

u32 gpf_count() {
    int count = 0;

    for (int i = 0; i < GPF_SIZE; i++) {
        if (gGraphPool[i].init == TRUE) {
            count++;
        }
    }

    return count;
}

static void __gpf_ctor(GraphPath *p) {
    p->init = 1;
    p->neighbors[0] = 
    p->neighbors[1] = 
    p->neighbors[2] = NULL;

    p->distances[0] = 
    p->distances[1] = 
    p->distances[2] = 0.0f;
}

u8 __gpf_CheckWall(GraphPath *g1, GraphPath *g2) {
    struct Surface *hit = NULL;
    Vec3f hit_pos;
    Vec3f dir;
    Vec3f srcPos, destPos;
    vec3f_copy(srcPos, g1->position);
    vec3f_copy(destPos, &g2->position);

    srcPos[1] += 15;
    destPos[1] += 15;

    vec3f_diff(dir, destPos, srcPos);
    find_surface_on_ray(
        srcPos,
        dir,
        &hit,
        hit_pos,
        RAYCAST_FIND_FLOOR | RAYCAST_FIND_WALL
    );

    return (hit != NULL);
}

static GraphPath *__gpf_NearestPath(int i, GraphPath *p) {
    GraphPath *closestPath = NULL;
    f32 minDist = 0x20000;

    f32 maxDist = 0;
    switch (i) {
        case 0: break;
        case 1: maxDist = p->distances[0]; break;
        #define __max(a, b) (((a) <= (b)) ? (b) : (a))
        case 2: maxDist = __max(p->distances[1], p->distances[2]); break;
    }

    int counting = 0;
    for (int __i = 0; __i < GPF_SIZE; __i++) {

        if (gGraphPool[__i].init == FALSE) {
            continue;
        }


        GraphPath *check = &gGraphPool[__i];

        f32 objDist = __gpf_distance(p, check);
        if ((objDist <= minDist)
         && (objDist > maxDist)
        ) {
            if (__gpf_CheckWall(p, check) == FALSE) {
                closestPath = check;
                minDist = objDist;
            }
        }
        counting++;
    }

    p->distances[i] = minDist;

    return closestPath;
}

static GraphPath *__gpf_Pop() {
    u32 idx = gPoolIdx++;

    GraphPath *ret = &gGraphPool[idx];
    __gpf_ctor(ret);

    ret->_index = idx;

    return ret;
}

static void __gpf_NeighborFunc(GraphPath *p) {
    // todo: replace with 3-minimum function that i couldnt get working before
    for (int i = 0; i < NEIGHBORSIZE; i++) {
        if (p->neighbors[i] != NULL) {
            if (p->neighbors[i]->init == 1) continue;
            else {
                assert(0, "how did we get here");
            }
        }

        GraphPath *np = __gpf_NearestPath(i, p);

        if (np == NULL) continue;
        p->neighbors[i] = np;
    }
}

static void __gpf_UpdateNeighbors(GraphPath *p) {
    assert(p != NULL, "No Path");
    // todo: replace with 3-minimum function that i couldnt get working before
    for (int i = 0; i < NEIGHBORSIZE; i++) {
        p->neighbors[i] = __gpf_NearestPath(i, p);
    }
}

void gpf_setup_neighbors() {
    for (int i = 0; i < GPF_SIZE; i++) {
        __gpf_UpdateNeighbors(&gGraphPool[i]);
    }
}

typedef enum LM {
    SINGLE_LINK,
    DOUBLE_LINK,
} _GPFLinkMode;
static void __gpf_Link(_GPFLinkMode mode, Obj *oo, GraphPath *p) {
    oo->oPathLink = (s32 *)p;
    if (mode == DOUBLE_LINK) {
        p->objLink = oo;
    } else {
        p->objLink = NULL;
    }

    vec3f_copy(p->position, &o->oPosX);

    // viral init?
    // __gpf_NeighborFunc(oo);
}

void gpf_ObjectInit(Obj *oo) {
    GraphPath *p = __gpf_Pop();

    __gpf_Link(DOUBLE_LINK, oo, p);
}

void gpf_PathInit(Obj *oo) {
    GraphPath *p = __gpf_Pop();

    __gpf_Link(SINGLE_LINK, oo, p);
}

void gpf_ObjectUpdate(Obj *oo) {
    GraphPath *p = OBJ_PATH(oo);

    vec3f_copy(p->position, &oo->oPosX);

    __gpf_UpdateNeighbors(p);
}

void mario_graphpath_init() {
    __gpf_Link(DOUBLE_LINK, gMarioObject, &gGraphPool[0]);
}

void mario_graphpath_update() {
    gpf_ObjectUpdate(gMarioObject);
}

void gpf_InitPath(struct Object *oo) {
    oo->oPathWork = &gPathWork[gPathIdx++];
    oo->oPathWorkIdx = 0;
    // oo->oPathWorkLen = 0;
}

static GraphPath *__gpf_searchMinDist(GraphPath **Q, f32 *distances, u32 front, u32 back) {
    // swap min dist and front of queue; return front
    assert(front <= back, "broken queue");

    u32 idx = front;
    f32 min_dist = INFINITY;

    for (u32 i = front; i < back; i++) {
        if (distances[i] < min_dist) {
            min_dist = distances[i];
            idx = i;
        }
    }

    GraphPath *tmp = Q[front];
    Q[front] = Q[idx];
    Q[idx] = tmp;
    Q[front]->_inQueue = 0;

    return Q[front];
}

void gpf_find_shortest_path(GraphPath *source, GraphPath *dest, GraphPath **path) {
    // Initialize data structures

    GraphPath *prev[GPF_SIZE];
    float distancesTo[GPF_SIZE];
    GraphPath *Q[GPF_SIZE];
    u32 Q_front = 0;
    u32 Q_back = 0;

    for (int i = 0; i < GPF_SIZE; i++) {
        distancesTo[i] = 0;
        prev[i] = NULL;
        Q[i] = NULL;
        if (gGraphPool[i].init == 1) {
            Q[Q_back++] = &gGraphPool[i];
            gGraphPool[i]._inQueue = 1;
            distancesTo[i] = __gpf_distance(source, &gGraphPool[i]);
        }
    }
    distancesTo[source->_index] = 0;

    while (Q_front != !Q_back) {
        GraphPath *candidate = __gpf_searchMinDist(Q, distancesTo, Q_front, Q_back);
        Q_front++; // effectively popped queue

        // short circuit check
        if (candidate == dest) {
            GraphPath *end = dest;

            if (prev[end->_index] || end == source) {
                u32 idx = 0;

                while (end != NULL) {
                    path[idx] = end;
                    end = prev[end->_index];
                }

                Obj *so = source->objLink;

                // TODO: - 1?
                so->oPathWorkIdx = idx - 1;
                return;
            }
        }


        for (int i = 0; i < NEIGHBORSIZE; i++) {
            if (candidate->neighbors[i] == NULL) continue;

            if (candidate->neighbors[i]->_inQueue == 1) {
                f32 alt = candidate->distances[i] + distancesTo[candidate->_index];

                if (alt < distancesTo[candidate->neighbors[i]->_index]) {
                    distancesTo[candidate->neighbors[i]->_index] = alt;
                    prev[candidate->neighbors[i]->_index] = candidate;
                }
            }
        }

    }


    // found nothing
    Obj *so = source->objLink;
    so->oPathWorkIdx = -1;
}

void gpf_MakePath(struct Object *oo, GraphPath *dest) {
    gpf_find_shortest_path(OBJ_PATH(oo), dest, oo->oPathWork);
}

void gpf_FollowPath(struct Object *oo) {
    GraphPath **pl = oo->oPathWork;

    if (oo->oPathWorkIdx == -1) {
        return;
    }

    GraphPath *toFollow = pl[oo->oPathWorkIdx];


    obj_turn_toward_pos(oo, toFollow->position, O_MOVE_ANGLE_YAW_INDEX, 0x400);

    if (__gpf_distance(oo->oPathLink, toFollow) < 10.0f) {
        oo->oPathWorkIdx--;
    }

    oo->oForwardVel = 5.0f;
}

