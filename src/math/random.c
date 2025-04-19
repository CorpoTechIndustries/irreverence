#include <math/random.h>

static int randomseed = 0;

void Math_RandomSeed(int seed)
{
	randomseed = seed;
}

#define A (1103515245)
#define M (2 << 30)
#define C 12345

int Math_RandomInt()
{
	randomseed = (A * randomseed + C) % M;

	return randomseed;
}

int Math_RandomIntMax(int max)
{
	return Math_RandomInt() % max;
}

int Math_RandomIntRange(int min, int max)
{
	return (Math_RandomInt() % (max - min)) + min;
}

float Math_RandomFloat()
{
	return (float)Math_RandomInt() / M;
}

float Math_RandomFloatMax(float max)
{
	return Math_RandomFloat() * max;
}

float Math_RandomFloatRange(float min, float max)
{
	return Math_RandomFloat() * (max - min) + min;
}
