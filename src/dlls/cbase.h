#pragma once

#include <public/edict.h>
#include "util.h"
#include "efuncs.h"

bool DispatchEntitySpawn(edict_t* edict);
void DispatchEntityFree(edict_t* edict);
void DispatchEntityThink(edict_t* edict);

class CBaseEntity
{
public:
	virtual ~CBaseEntity() {}

	virtual void Spawn() {}
	virtual void Think() {}

	inline edict_t* Edict()
	{
		return m_pVars->entity;
	}

	void Remove();

	entvars_t* m_pVars;

	static CBaseEntity* Create(const char* name);
};

template <typename T>
T* GetClassPtr(T* p)
{
	edict_t* edict = (edict_t*)p;

	if (!edict) {
		edict = CREATE_ENTITY();
	}

	p = (T*)edict->data;

	if (!p) {
		p = new T;

		edict->data = p;

		p->m_pVars = &edict->v;
	}

	return p;
}

#define LINK_ENTITY_TO_CLASS(__class, __name) \
extern "C" DLL_EXPORT void __name(edict_t* edict); \
void __name(edict_t* edict) { GetClassPtr((__class*)edict); }
