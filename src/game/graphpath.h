#pragma once

#include "object_fields.h"

#define NEIGHBORSIZE 1

typedef struct _Graph {
    u8 init;
    u8 mark;
    Vec3f position;
    struct Object *objLink;
    struct _Graph *neighbors[NEIGHBORSIZE];
    float distances[NEIGHBORSIZE];

    // struct Object objects[3];
} GraphPath;

#define GPF_SIZE 500

void find_surface_on_ray(Vec3f orig, Vec3f dir, struct Surface **hit_surface, Vec3f hit_pos, s32 flags);

typedef struct _gpl {
    u8 init;
    GraphPath *p;
    GraphPath *next;
} GraphList;

#define oPathWork OBJECT_FIELD_S32P(0x1B)
#define oPathWorkIdx OBJECT_FIELD_S32(0x1C)
#define oPathWorkLen OBJECT_FIELD_S32(0x1D)

#define oPathLink OBJECT_FIELD_S32P(0x1B)
#define oPathLinkNum OBJECT_FIELD_S32(0x1C)

#define oMaestroInit OBJECT_FIELD_S32(0x1B)

extern int graphPathInit;
