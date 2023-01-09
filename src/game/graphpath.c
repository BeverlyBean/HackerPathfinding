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
