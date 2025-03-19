#pragma once

#include <math/vec3.h>
#include <math/quat.h>

#include <stdint.h>
#include <stdbool.h>

#define MAX_PHYSICS_OBJECTS 65535

#define INVALID_BODYID UINT32_MAX

typedef enum {
	PHYS_LAYER_NONMOVING,
	PHYS_LAYER_MOVING
} phys_layer_t;

typedef enum {
	PHYS_TYPE_STATIC,
	PHYS_TYPE_KINEMATIC,
	PHYS_TYPE_DYNAMIC
} phys_type_t;

typedef enum {
	PHYS_SHAPE_CUBE,
	PHYS_SHAPE_SPHERE,
	PHYS_SHAPE_CYLINDER,
	PHYS_SHAPE_CAPSULE,
	PHYS_SHAPE_CONVEX
} phys_shape_t;

typedef struct {
	uint32_t bodyId;
	uint16_t index;
	uint8_t shape;
	uint8_t layer;
	uint32_t flags;

	vec3_t position;
	quat_t rotation;
	vec3_t velocity;
	vec3_t angularVelocity;
	float friction;
	float restitution;
} physobj_t;

bool Phys_Init();
void Phys_Destroy();

void Phys_Update(float tickRate);

physobj_t* Phys_AddCube(vec3_t position, quat_t rotation, vec3_t size, phys_type_t type, phys_layer_t layer);
physobj_t* Phys_AddSphere(vec3_t position, quat_t rotation, float radius, phys_type_t type, phys_layer_t layer);
physobj_t* Phys_AddCylinder(vec3_t position, quat_t rotation, float radius, float height, phys_type_t type, phys_layer_t layer);
physobj_t* Phys_AddCapsule(vec3_t position, quat_t rotation, float radius, float height, phys_type_t type, phys_layer_t layer);
physobj_t* Phys_AddConvex(vec3_t position, quat_t rotation, const vec3_t* points, uint32_t count, phys_type_t type, phys_layer_t layer);
void Phys_RemoveObject(physobj_t* physobj);
