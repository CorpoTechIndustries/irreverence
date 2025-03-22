#include "cbase.h"

bool DispatchEntitySpawn(edict_t* edict)
{
	CBaseEntity* ent = (CBaseEntity*)edict->data;

	if (!ent) {
		return false;
	}

	ent->Spawn();

	return true;
}

void DispatchEntityFree(edict_t* edict)
{
	CBaseEntity* ent = (CBaseEntity*)edict->data;

	if (ent) {
		delete ent;
	}

	edict->data = nullptr;
	edict->v = {};
}

void DispatchEntityThink(edict_t* edict)
{
	CBaseEntity* ent = (CBaseEntity*)edict->data;

	if (!ent) {
		return;
	}

	ent->Think();
}

void CBaseEntity::Remove()
{
	FREE_ENTITY(Edict()->id);
}

CBaseEntity* CBaseEntity::Create(const char* name)
{
	edict_t* ent = CREATE_ENTITY_FROM_NAME(name);

	if (!ent) {
		ERROR("Couldn't create entity %s", name);
		return nullptr;
	}

	DispatchEntitySpawn(ent);

	return (CBaseEntity*)ent->data;
}
