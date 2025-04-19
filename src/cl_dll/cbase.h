#pragma once

#include "cfuncs.h"

bool EntitySpawn(client_entity_t* entity, const char* className);
void EntityDestroy(client_entity_t* entity);
void EntityThink(client_entity_t* entity);
void EntityRender(client_entity_t* entity);

class C_BaseEntity
{
public:
	virtual ~C_BaseEntity() {}

	virtual void Spawn() {}
	virtual void Think() {}
	virtual void Render() {}

	inline client_entity_t* CLEnt()
	{
		return m_pVars->entity;
	}

	entvars_t* m_pVars;
};

class IClientEntityRegister
{
public:
	IClientEntityRegister(const char* className);

	virtual void Create(client_entity_t* ent) = 0;
};

template <typename T>
class ClientEntityRegister : public IClientEntityRegister
{
public:
	ClientEntityRegister(const char* className)
		: IClientEntityRegister(className)
	{ }

	void Create(client_entity_t* ed) override
	{
		C_BaseEntity* ent = (C_BaseEntity*)new T;

		ent->m_pVars = &ed->v;

		ent->Spawn();
		ed->data = ent;
	}
};

#define LINK_ENTITY_TO_CLASS(__class, __name) \
static ClientEntityRegister<__class> __name(#__name)
