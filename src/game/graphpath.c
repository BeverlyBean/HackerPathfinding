#include <PR/ultratypes.h>

#include "config.h"
#include "area.h"
#include "sm64.h"

typedef struct _Graph {
    u8 init;
    Vec3f position;
    struct _Graph *neighbors[3];
    float distances[3];

    // struct Object objects[3];
} GraphPath;

GraphPath *graphStart;
GraphPath *graphEnd;


typedef struct _GraphWork {
    GraphPath from;
    GraphPath to;
} GraphWork;

#define GPF_SIZE 500
GraphPath gGraphPool[GPF_SIZE];

void find_surface_on_ray(Vec3f orig, Vec3f dir, struct Surface **hit_surface, Vec3f hit_pos, s32 flags);

typedef struct _gpl {
    u8 init;
    GraphPath *p;
    GraphPath *next;
} GraphList;
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

void gpf_init() {
    bzero(gGraphPool, sizeof(gGraphPool));
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
}

void gpf_register(Vec3f pos) {
    int r = gpf_pop();

    gGraphPool[r].position[0] = pos[0];
    gGraphPool[r].position[1] = pos[1];
    gGraphPool[r].position[2] = pos[2];
}

int gPathsFound = 0;

int gpf_pathfind(GraphPath **path, Vec3f from, Vec3f to) {
    if (gPathsFound == 0) {
        gpf_3neighbors();
    }
}