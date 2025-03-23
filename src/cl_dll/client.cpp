#include "cfuncs.h"
#include "util.h"

extern "C" DLL_EXPORT bool ClientInit(client_functions_t* funcs, client_exports_t* exports, int version)
{
	if (CLIENT_INTERFACE_VERSION != version) {
		return false;
	}

	g_ClientFuncs = *funcs;

	exports->pClose = []() {};

	MESSAGE("Hello from client dll!");

	return true;
}
