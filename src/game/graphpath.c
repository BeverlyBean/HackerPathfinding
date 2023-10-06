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
struct GraphPath *gPathWork[OBJECT_POOL_CAPACITY][GPF_SIZE];
int gPathIdx = 1; // 0 is mario

static void __gpf_ctor(GraphPath *p) {
    p->init = 1;
    p->neighbors[0] = 
    p->neighbors[1] = 
    p->neighbors[2] = NULL;

    p->distances[0] = 
    p->distances[1] = 
    p->distances[2] = 0.0f;
}

u8 __gpf_CheckWall(Obj *o1, Obj *o2) {
    struct Surface *hit = NULL;
    Vec3f hit_pos;
    Vec3f dir;
    Vec3f oPosSource, oPosDest;
    vec3f_copy(oPosSource, &o1->oPosX);
    vec3f_copy(oPosDest, &o2->oPosX);

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

    return (hit != NULL);
}

static Obj *__gpf_NearestObj(int i, GraphPath *p) {
    Obj *_o = p->objLink;

    Obj *closestObj = NULL;
    f32 minDist = 0x20000;

    f32 maxDist = 0;
    switch (i) {
        case 0: break;
        case 1: maxDist = p->distances[0]; break;
        #define __max(a, b) (((a) <= (b)) ? (b) : (a))
        case 2: maxDist = __max(p->distances[1], p->distances[2]); break;
    }

    int counting = 0;
    for (int __i = 0; __i < OBJECT_POOL_CAPACITY; __i++) {

        if (gObjectPool[__i].activeFlags == ACTIVE_FLAG_DEACTIVATED) {
            continue;
        }


        Obj *obj = &gObjectPool[__i];

        GraphPath *p = OBJ_PATH(obj);
        u32 foundBehav = 0;


        if (((u32)p & 0xFF000000) == 0x80000000) {
            if (p->magic == GPF_MAGIC) {
                foundBehav = 1;
            }
        }

        if (foundBehav 
            && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && obj != _o
        ) {
            f32 objDist = dist_between_objects(_o, obj);
            if ((objDist <= minDist)
             && (objDist > maxDist)
            ) {
                if (__gpf_CheckWall(_o, obj) == FALSE) {
                    closestObj = obj;
                    minDist = objDist;
                }
            }
        }
        counting++;
    }

    p->distances[i] = minDist;

    char t[50];
    sprintf(t, "went through %d", counting);
    assert(closestObj != NULL, t);
    return closestObj;
}

static GraphPath *__gpf_Pop() {
    u32 idx = gPoolIdx++;

    GraphPath *ret = &gGraphPool[idx];
    __gpf_ctor(ret);

    ret->_index = idx;

    return ret;
}

static void __gpf_NeighborFunc(Obj *oo) {
    GraphPath *p = OBJ_PATH(oo);

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

        if (OBJ_PATH(oi) == NULL) {
            gpf_ObjectInit(oi);

        }
        p->neighbors[i] = OBJ_PATH(oi);
    }
}

static void __gpf_UpdateNeighbors(Obj *oo) {
    GraphPath *p = OBJ_PATH(oo);
    assert(p != NULL, "No Path");
    // todo: replace with 3-minimum function that i couldnt get working before
    for (int i = 0; i < NEIGHBORSIZE; i++) {
        p->objects[i] = __gpf_NearestObj(i, p);


        Obj *oi = p->objects[i];
        if (oi == NULL) continue;

        if (OBJ_PATH(oi) == NULL) {
            gpf_ObjectInit(oi);
        }
        p->neighbors[i] = OBJ_PATH(oi);
    }
}

static void __gpf_Link(Obj *oo, GraphPath *p) {
    oo->oPathLink = (s32 *)p;
    p->objLink = oo;

    vec3f_copy(p->position, &o->oPosX);

    // viral init?
    __gpf_NeighborFunc(oo);
}

void gpf_ObjectInit(Obj *oo) {
    GraphPath *p = __gpf_Pop();

    __gpf_Link(oo, p);
}

void gpf_ObjectUpdate(Obj *oo) {
    GraphPath *p = OBJ_PATH(oo);

    vec3f_copy(p->position, &o->oPosX);

    __gpf_UpdateNeighbors(oo);
}

void mario_graphpath_init() {
    __gpf_Link(gMarioObject, &gGraphPool[0]);
}

void mario_graphpath_update() {
    gpf_ObjectUpdate(gMarioObject);

    // TODO: have it regenerate neighbors for each of mario's neighbors
}

void gpf_InitPath(struct Object *oo) {
    oo->oPathWork = &gPathWork[gPathIdx++];
    oo->oPathWorkIdx = 0;
    // oo->oPathWorkLen = 0;
}

static f32 __gpf_distance(GraphPath *s, GraphPath *d) {
    Vec3f sv; vec3f_copy(sv, s->position);
    Vec3f dv; vec3f_copy(dv, d->position);

    f32 di = INFINITY;
    vec3f_get_dist(sv, dv, &di);

    return di;
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

void find_shortest_path(GraphPath *source, GraphPath *dest, GraphPath **path) {
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

void gpf_MakePath(struct Object *oo, struct Object *dest) {
    find_shortest_path(OBJ_PATH(oo), OBJ_PATH(dest), oo->oPathWork);
}

void gpf_FollowPath(struct Object *oo) {
    GraphPath **pl = oo->oPathWork;

    if (oo->oPathWorkIdx == -1) {
        return;
    }

    GraphPath *toFollow = pl[oo->oPathWorkIdx];


    obj_turn_toward_object(oo, toFollow->objLink, O_MOVE_ANGLE_YAW_INDEX, 0x400);

    if (dist_between_objects(oo, toFollow->objLink) < 10.0f) {
        oo->oPathWorkIdx--;
    }

    oo->oForwardVel = 5.0f;
}

