#include <util/endian.h>

uint16_t Endian_SwapUInt16(uint16_t value)
{
#ifdef PLATFORM_LINUX
	return __builtin_bswap16(value);
#elif PLATFORM_WINDOWS

#endif
}

uint32_t Endian_SwapUInt32(uint32_t value)
{
#ifdef PLATFORM_LINUX
	return __builtin_bswap32(value);
#elif PLATFORM_WINDOWS

#endif
}

uint64_t Endian_SwapUInt64(uint64_t value)
{
#ifdef PLATFORM_LINUX
	return __builtin_bswap64(value);
#elif PLATFORM_WINDOWS

#endif
}
