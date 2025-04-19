#include "cfuncs.h"
#include "util.h"

#include "cbase.h"

#include <cmath>

MAKE_CVAR_FLOAT(cl_test, 1.5f);

class C_Foo : public C_BaseEntity
{
public:
	void Spawn() override
	{
		MESSAGE("We spawned...?");
	}

	void Render() override
	{
		m_fCur += 0.01f;

		if (m_fCur > 1.0f) {
			m_fCur = 0.0f;
		}

		vec3_t size = { 1.0f, 1.0f, 1.0f };
		vec4_t color = { (5.0f + cosf(m_fCur * 5.0f)) * 2.0f, (5.0f + sinf(m_fCur * 5.0f)) * 2.0f, 5.0f, 1.0f };
		DRAW_DEBUG_CUBE(m_pVars->origin, size, color);
	}

	void Think() override
	{
	}

private:
	float m_fCur = 0.0f;

};

LINK_ENTITY_TO_CLASS(C_Foo, npc_test);

extern "C" DLL_EXPORT bool ClientInit(client_functions_t* funcs, client_exports_t* exports, int version)
{
	if (CLIENT_INTERFACE_VERSION != version) {
		return false;
	}

	g_ClientFuncs = *funcs;

	REGISTER_CVAR(&cl_test);

	exports->pInit = []() {};
	exports->pClose = []() {};

	exports->pEntityCreate = EntitySpawn;
	exports->pEntityDestroy = EntityDestroy;
	exports->pEntityThink = EntityThink;
	exports->pEntityRender = EntityRender;

	exports->pRenderPreDepth = []() { return false; };
	exports->pRenderPostDepth = []() {};
	exports->pRenderPreShadows = []() { return false; };
	exports->pRenderPostShadows = []() {};
	exports->pRenderPreOpaque = []() { return false; };
	exports->pRenderPostOpaque = []() {};
	exports->pRenderPreTransparent = []() { return false; };
	exports->pRenderPostTransparent = []() {};
	exports->pRenderPreDecals = []() { return false; };
	exports->pRenderPostDecals = []() {};
	exports->pRenderPreOverlay = []() { return false; };
	exports->pRenderPostOverlay = []() {};

	MESSAGE("Hello from client dll!");

	return true;
}
