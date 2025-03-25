#include <engine/physics.h>

#include <engine/log.h>

#include <joltc.h>

static void jphMessageCallback(const char* message)
{
	LOG_WARNING("Physics: %s", message);
}

static JPH_JobSystem* s_pJobSystem = NULL;
static JPH_PhysicsSystem* s_pPhysics = NULL;
static JPH_BodyInterface* s_pBodyInterface = NULL;
static const JPH_BodyLockInterface* s_pBodyLockInterface = NULL;
static const JPH_BodyLockInterface* s_pBodyNoLockInterface = NULL;
static const JPH_BroadPhaseQuery* s_pBPQuery = NULL;

static JPH_ObjectLayerPairFilter* s_pOLPFTable = NULL;
static JPH_BroadPhaseLayerInterface* s_pBPLITable = NULL;
static JPH_ObjectVsBroadPhaseLayerFilter* s_pOVBPLFTable = NULL;

static physobj_t s_PhysObjects[MAX_PHYSICS_OBJECTS];
static uint16_t s_iPhysObjectCount = 0;
static uint16_t s_iPhysObjectReach = 0; // Weird name, but it is (biggest index + 1), good for iterating s_PhysObjects

bool Phys_Init()
{
	if (!JPH_Init()) {
		LOG_FATAL("Failed to initialize Physics Library");
		return false;
	}

	JPH_SetTraceHandler(jphMessageCallback);

	s_pJobSystem = JPH_JobSystemThreadPool_Create(NULL);

	s_pOLPFTable = JPH_ObjectLayerPairFilterTable_Create(2);
	JPH_ObjectLayerPairFilterTable_EnableCollision(s_pOLPFTable, PHYS_LAYER_NONMOVING, PHYS_LAYER_MOVING);
	JPH_ObjectLayerPairFilterTable_EnableCollision(s_pOLPFTable, PHYS_LAYER_MOVING, PHYS_LAYER_NONMOVING);

	s_pBPLITable = JPH_BroadPhaseLayerInterfaceTable_Create(2, 2);
	JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(s_pBPLITable, PHYS_LAYER_NONMOVING, PHYS_LAYER_NONMOVING);
	JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(s_pBPLITable, PHYS_LAYER_MOVING, PHYS_LAYER_MOVING);

	s_pOVBPLFTable = JPH_ObjectVsBroadPhaseLayerFilterTable_Create(s_pBPLITable, 2, s_pOLPFTable, 2);

	JPH_PhysicsSystemSettings physSettings = {};
	physSettings.maxBodies = MAX_PHYSICS_OBJECTS;
	physSettings.maxBodyPairs = MAX_PHYSICS_OBJECTS;
	physSettings.maxContactConstraints = MAX_PHYSICS_OBJECTS;
	physSettings.numBodyMutexes = 0;
	physSettings.objectLayerPairFilter = s_pOLPFTable;
	physSettings.broadPhaseLayerInterface = s_pBPLITable;
	physSettings.objectVsBroadPhaseLayerFilter = s_pOVBPLFTable;

	s_pPhysics = JPH_PhysicsSystem_Create(&physSettings);
	s_pBodyInterface = JPH_PhysicsSystem_GetBodyInterface(s_pPhysics);
	s_pBodyLockInterface = JPH_PhysicsSystem_GetBodyLockInterface(s_pPhysics);
	s_pBodyNoLockInterface = JPH_PhysicsSystem_GetBodyLockInterfaceNoLock(s_pPhysics);
	s_pBPQuery = JPH_PhysicsSystem_GetBroadPhaseQuery(s_pPhysics);

	for (uint16_t i = 0; i < MAX_PHYSICS_OBJECTS; i++) {
		physobj_t* obj = &s_PhysObjects[i];
		obj->bodyId = INVALID_BODYID;
		obj->index = 0;
		obj->layer = PHYS_LAYER_NONMOVING;
		obj->shape = PHYS_SHAPE_CUBE;
		obj->flags = 0;
	}

	return true;
}

void Phys_Destroy()
{
	for (uint32_t i = 0; i < MAX_PHYSICS_OBJECTS; i++) {
		physobj_t* obj = &s_PhysObjects[i];
		if (obj->bodyId == INVALID_BODYID) continue;

		JPH_BodyInterface_RemoveAndDestroyBody(s_pBodyInterface, obj->bodyId);
		obj->bodyId = INVALID_BODYID;
	}

	JPH_JobSystem_Destroy(s_pJobSystem);

	JPH_PhysicsSystem_Destroy(s_pPhysics);

	JPH_Shutdown();
}

void Phys_Update()
{
	const float cDelta = 1.0f / 60.0f;
	const int cSteps = 1;

	JPH_PhysicsSystem_Update(s_pPhysics, cDelta, cSteps, s_pJobSystem);

	// TODO: Iterate all Dynamic and Kinematic Objects and update their position, orientation, etc
	JPH_BodyLockRead lockRead;
	for (uint16_t i = 0; i < s_iPhysObjectReach; i++) {
		physobj_t* obj = &s_PhysObjects[i];
		if (obj->bodyId == INVALID_BODYID || obj->layer == PHYS_LAYER_NONMOVING) {
			continue;
		}

		JPH_BodyLockInterface_LockRead(s_pBodyNoLockInterface, obj->bodyId, &lockRead);

		JPH_Body_GetCenterOfMassPosition((JPH_Body*)lockRead.body, (JPH_RVec3*)&obj->position);
		JPH_Body_GetRotation((JPH_Body*)lockRead.body, (JPH_Quat*)&obj->rotation);
		JPH_Body_GetLinearVelocity((JPH_Body*)lockRead.body, (JPH_Vec3*)&obj->velocity);
		JPH_Body_GetAngularVelocity((JPH_Body*)lockRead.body, (JPH_Vec3*)&obj->angularVelocity);
	}

}

static uint16_t PhysObjFreeId()
{
	uint16_t index = UINT16_MAX;
	for (uint16_t i = 0; i < MAX_PHYSICS_OBJECTS; i++) {
		physobj_t* obj = &s_PhysObjects[i];

		if (obj->bodyId != INVALID_BODYID) continue;

		index = i;
		break;
	}

	if (index == UINT16_MAX) {
		LOG_ERROR("Couldn't add Physics Object due to reaching limit");
	}

	return index;
}

static physobj_t* PhysObjAdd(const physobj_t* data)
{
	uint32_t count = data->index + 1;
	if (count > s_iPhysObjectReach) {
		s_iPhysObjectReach = count;
	}

	physobj_t* obj = &s_PhysObjects[data->index];

	*obj = *data;
	s_iPhysObjectCount++;

	return obj;
}

physobj_t* Phys_AddCube(vec3_t position, quat_t rotation, vec3_t size, phys_type_t type, phys_layer_t layer)
{
	uint16_t index = PhysObjFreeId();
	if (index == UINT16_MAX) {
		return NULL;
	}

	JPH_Vec3 jphSize = { size.x, size.y, size.z };
	JPH_BoxShape* shape = JPH_BoxShape_Create(&jphSize, JPH_DEFAULT_CONVEX_RADIUS);

	JPH_BodyCreationSettings* settings = JPH_BodyCreationSettings_Create3(
		(const JPH_Shape*)shape,
		(const JPH_RVec3*)&position,
		(const JPH_Quat*)&rotation,
		type,
		layer);

	JPH_BodyCreationSettings_SetUserData(settings, (uint64_t)index); // Store the index on the body

	JPH_BodyID bodyId = JPH_BodyInterface_CreateAndAddBody(s_pBodyInterface, settings, JPH_Activation_Activate);
	JPH_BodyCreationSettings_Destroy(settings);

	if (bodyId == INVALID_BODYID) {
		return NULL;
	}

	physobj_t obj;
	obj.bodyId = bodyId;
	obj.index = index;
	obj.layer = layer;
	obj.shape = PHYS_SHAPE_CUBE;
	obj.flags = 0;

	obj.position = position;
	obj.velocity = NEW_VEC3S(0.0f);
	obj.angularVelocity = NEW_VEC3S(0.0f);
	obj.friction = 0.0f;
	obj.restitution = 0.0f;

	return PhysObjAdd(&obj);
}

physobj_t* Phys_AddSphere(vec3_t position, quat_t rotation, float radius, phys_type_t type, phys_layer_t layer)
{
	uint16_t index = PhysObjFreeId();
	if (index == UINT16_MAX) {
		return NULL;
	}

	JPH_SphereShape* shape = JPH_SphereShape_Create(radius);

	JPH_BodyCreationSettings* settings = JPH_BodyCreationSettings_Create3(
		(const JPH_Shape*)shape,
		(const JPH_RVec3*)&position,
		(const JPH_Quat*)&rotation,
		type,
		layer);

	JPH_BodyCreationSettings_SetUserData(settings, (uint64_t)index); // Store the index on the body

	JPH_BodyID bodyId = JPH_BodyInterface_CreateAndAddBody(s_pBodyInterface, settings, JPH_Activation_Activate);
	JPH_BodyCreationSettings_Destroy(settings);

	if (bodyId == INVALID_BODYID) {
		return NULL;
	}

	physobj_t obj;
	obj.bodyId = bodyId;
	obj.index = index;
	obj.layer = layer;
	obj.shape = PHYS_SHAPE_SPHERE;
	obj.flags = 0;

	obj.position = position;
	obj.velocity = NEW_VEC3S(0.0f);
	obj.angularVelocity = NEW_VEC3S(0.0f);
	obj.friction = 0.0f;
	obj.restitution = 0.0f;

	return PhysObjAdd(&obj);
}

physobj_t* Phys_AddCylinder(vec3_t position, quat_t rotation, float radius, float height, phys_type_t type, phys_layer_t layer)
{
	uint16_t index = PhysObjFreeId();
	if (index == UINT16_MAX) {
		return NULL;
	}

	JPH_CylinderShape* shape = JPH_CylinderShape_Create(height, radius);

	JPH_BodyCreationSettings* settings = JPH_BodyCreationSettings_Create3(
		(const JPH_Shape*)shape,
		(const JPH_RVec3*)&position,
		(const JPH_Quat*)&rotation,
		type,
		layer);

	JPH_BodyCreationSettings_SetUserData(settings, (uint64_t)index); // Store the index on the body

	JPH_BodyID bodyId = JPH_BodyInterface_CreateAndAddBody(s_pBodyInterface, settings, JPH_Activation_Activate);
	JPH_BodyCreationSettings_Destroy(settings);

	if (bodyId == INVALID_BODYID) {
		return NULL;
	}

	physobj_t obj;
	obj.bodyId = bodyId;
	obj.index = index;
	obj.layer = layer;
	obj.shape = PHYS_SHAPE_CYLINDER;
	obj.flags = 0;

	obj.position = position;
	obj.velocity = NEW_VEC3S(0.0f);
	obj.angularVelocity = NEW_VEC3S(0.0f);
	obj.friction = 0.0f;
	obj.restitution = 0.0f;

	return PhysObjAdd(&obj);
}

physobj_t* Phys_AddCapsule(vec3_t position, quat_t rotation, float radius, float height, phys_type_t type, phys_layer_t layer)
{
	uint16_t index = PhysObjFreeId();
	if (index == UINT16_MAX) {
		return NULL;
	}

	JPH_CapsuleShape* shape = JPH_CapsuleShape_Create(height, radius);

	JPH_BodyCreationSettings* settings = JPH_BodyCreationSettings_Create3(
		(const JPH_Shape*)shape,
		(const JPH_RVec3*)&position,
		(const JPH_Quat*)&rotation,
		type,
		layer);

	JPH_BodyCreationSettings_SetUserData(settings, (uint64_t)index); // Store the index on the body

	JPH_BodyID bodyId = JPH_BodyInterface_CreateAndAddBody(s_pBodyInterface, settings, JPH_Activation_Activate);
	JPH_BodyCreationSettings_Destroy(settings);

	if (bodyId == INVALID_BODYID) {
		return NULL;
	}

	physobj_t obj;
	obj.bodyId = bodyId;
	obj.index = index;
	obj.layer = layer;
	obj.shape = PHYS_SHAPE_CAPSULE;
	obj.flags = 0;

	obj.position = position;
	obj.velocity = NEW_VEC3S(0.0f);
	obj.angularVelocity = NEW_VEC3S(0.0f);
	obj.friction = 0.0f;
	obj.restitution = 0.0f;

	return PhysObjAdd(&obj);
}

physobj_t* Phys_AddConvex(vec3_t position, quat_t rotation, const vec3_t* points, uint32_t count, phys_type_t type, phys_layer_t layer)
{
	uint16_t index = PhysObjFreeId();
	if (index == UINT16_MAX) {
		return NULL;
	}

	JPH_ConvexHullShapeSettings* shape = JPH_ConvexHullShapeSettings_Create((const JPH_Vec3*)points, count, JPH_DEFAULT_CONVEX_RADIUS);

	JPH_BodyCreationSettings* settings = JPH_BodyCreationSettings_Create3(
		(const JPH_Shape*)shape,
		(const JPH_RVec3*)&position,
		(const JPH_Quat*)&rotation,
		type,
		layer);

	JPH_BodyCreationSettings_SetUserData(settings, (uint64_t)index); // Store the index on the body

	JPH_BodyID bodyId = JPH_BodyInterface_CreateAndAddBody(s_pBodyInterface, settings, JPH_Activation_Activate);
	JPH_BodyCreationSettings_Destroy(settings);

	if (bodyId == INVALID_BODYID) {
		return NULL;
	}

	physobj_t obj;
	obj.bodyId = bodyId;
	obj.index = index;
	obj.layer = layer;
	obj.shape = PHYS_SHAPE_CONVEX;
	obj.flags = 0;

	obj.position = position;
	obj.velocity = NEW_VEC3S(0.0f);
	obj.angularVelocity = NEW_VEC3S(0.0f);
	obj.friction = 0.0f;
	obj.restitution = 0.0f;

	return PhysObjAdd(&obj);
}

void Phys_RemoveObject(physobj_t* physobj)
{
	if (physobj->bodyId == INVALID_BODYID) {
		return;
	}

	JPH_BodyInterface_RemoveAndDestroyBody(s_pBodyInterface, physobj->bodyId);

	if ((physobj->index + 1) >= s_iPhysObjectReach) {
		s_iPhysObjectReach = physobj->index;
	}
	s_iPhysObjectCount--;

	physobj->bodyId = INVALID_BODYID;
}
