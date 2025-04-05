#include "cfuncs.h"
#include "util.h"

MAKE_CVAR_FLOAT(cl_test, 1.5f);

extern "C" DLL_EXPORT bool ClientInit(client_functions_t* funcs, client_exports_t* exports, int version)
{
	if (CLIENT_INTERFACE_VERSION != version) {
		return false;
	}

	g_ClientFuncs = *funcs;

	REGISTER_CVAR(&cl_test);

	exports->pInit = []() {};
	exports->pClose = []() {};

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
