#include "cbase.h"

#include <unordered_map>
#include <string>

static std::unordered_map<std::string, IClientEntityRegister*> s_Classes;

IClientEntityRegister::IClientEntityRegister(const char* className)
{
	s_Classes[className] = this;
}

bool EntitySpawn(client_entity_t* ent, const char* className)
{
	IClientEntityRegister* reg = s_Classes[className];

	// TODO: Throw engine level error if we get this.

	MESSAGE("%s", className);
	if (!reg) {
		return false;
	}

	reg->Create(ent);

	return true;
}

void EntityDestroy(client_entity_t* entity)
{
	C_BaseEntity* ent = (C_BaseEntity*)entity->data;

	delete ent;

	entity->data = nullptr;
	entity->v = {};
}

void EntityThink(client_entity_t* entity)
{
	C_BaseEntity* ent = (C_BaseEntity*)entity->data;

	if (!ent) {
		return;
	}

	ent->Think();
}

void EntityRender(client_entity_t* entity)
{
	C_BaseEntity* ent = (C_BaseEntity*)entity->data;

	if (!ent) {
		return;
	}

	ent->Render();
}
