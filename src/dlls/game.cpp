#include "game.h"
#include "efuncs.h"
#include "cbase.h"

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
};

LINK_ENTITY_TO_CLASS(CTest, npc_test);

bool GameDLLInit(engine_functions_t* functions, game_exports_t* exports, int interface_version)
{
	if (interface_version != ENGINE_INTERFACE_VERSION) {
		return false;
	}

	g_EngineFuncs = *functions;

	exports->pGameInit = []() {};
	exports->pGameClose = []() {};
	exports->pSpawn = DispatchEntitySpawn;
	exports->pFree = DispatchEntityFree;
	exports->pThink = DispatchEntityThink;

	MESSAGE("Hello gamers!");

	CBaseEntity* entity = CBaseEntity::Create("npc_test");

	entity->Remove();

	return true;
}
