#include "game.h"
#include "efuncs.h"
#include "cbase.h"

#include <cstring>

class CTest : public CBaseEntity
{
public:
	void Spawn() override
	{
		MESSAGE("Beep");
	}

	~CTest() override
	{
		MESSAGE("Boop");
	}

	void Think() override
	{
		m_pVars->origin.x += 0.001f;
	}
};

LINK_ENTITY_TO_CLASS(CTest, npc_test);

MAKE_CVAR_FLOAT(sv_test, 0.5f);

bool GameDLLInit(engine_functions_t* functions, game_exports_t* exports, int interface_version)
{
	if (interface_version != ENGINE_INTERFACE_VERSION) {
		return false;
	}

	g_EngineFuncs = *functions;

	REGISTER_CVAR(&sv_test);

	exports->pGameInit = []() {};
	exports->pGameClose = []() {};
	exports->pSpawn = DispatchEntitySpawn;
	exports->pFree = DispatchEntityFree;
	exports->pThink = DispatchEntityThink;

	exports->pGetEntityClass = [](edict_t* edict, char* buff, size_t size) {
		CBaseEntity* ent = (CBaseEntity*)edict->data;

		std::string str = ent->GetClass();

		std::strncpy(buff, str.c_str(), size);
	};

	MESSAGE("Hello gamers!");

	CBaseEntity* entity = CBaseEntity::Create("npc_test");

	return true;
}
