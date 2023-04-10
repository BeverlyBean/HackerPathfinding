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


int dbgPathCount = 0;
#define isaddr(a) (((u32)(a) & 0xFF000000) == 0x80000000)

GraphPath gGraphPool[GPF_SIZE];

// worst case is that i need all this space
int gPathIdx = 0;
struct GraphPath *gPathWork[OBJECT_POOL_CAPACITY][GPF_SIZE];
enum GPFState {
    GPFS_FINDNODE,
    GPFS_GOTO_NODE,
    GPFS_INC_PATH,
};

void opObjectInit() {

}

void opFollow() {

}

void gpf_register() {

}

void gpf_setup_neighbors() {

}

void mario_graphpath_init() {

}

void mario_graphpath_update() {
    
}
