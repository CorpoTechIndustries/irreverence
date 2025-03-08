#include <engine/engine.h>

int main(int argc, const char** argv)
{
	return Engine_Run(argc - 1, argv + 1);
}
