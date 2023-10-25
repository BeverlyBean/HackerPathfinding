#pragma once

#include "object_fields.h"

#define NEIGHBORSIZE 1
#define GPF_SIZE 25

typedef struct _Graph {
    u8 _inQueue;
    u8 init;
    u16 _index;
    u32 magic;
    Vec3f position;
    struct _Graph *neighbors[NEIGHBORSIZE];
    float distances[NEIGHBORSIZE];
    // if follower
    struct Object *objLink;
} GraphPath;
extern GraphPath gGraphPool[GPF_SIZE];

#define GPF_MAGIC 0x50415448
#define FOLLOWER_COUNT 10
#define MINIMUM_PATH_DIST 10.0f
#define TARGET_PATH (&gGraphPool[0])

void find_surface_on_ray(Vec3f orig, Vec3f dir, struct Surface **hit_surface, Vec3f hit_pos, s32 flags);


#define oPathWork OBJECT_FIELD_S32P(0x1B)
#define oPathWorkIdx OBJECT_FIELD_S32(0x1C)
#define oPathLink OBJECT_FIELD_S32P(0x1D)
#define oFollower OBJECT_FIELD_S32(0x1E)
#define OBJ_PATH(_ob) ((GraphPath*)((_ob)->oPathLink))
// #define oPathWorkLen OBJECT_FIELD_S32(0x1F)

#define d__gpf_SourceBhv ((uintptr_t *)bhvTestFollower)
#define d__gpf_DestBhv ((uintptr_t *)bhvMario)

extern int graphPathInit;
