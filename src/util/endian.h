#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t Endian_SwapUInt16(uint16_t value);
uint32_t Endian_SwapUInt32(uint32_t value);
uint64_t Endian_SwapUInt64(uint64_t value);

#if CPU_BIG_ENDIAN

#define Endian_HostToBig16(__val) (__val)
#define Endian_HostToBig32(__val) (__val)
#define Endian_HostToBig64(__val) (__val)
#define Endian_HostToLittle16(__val) Endian_SwapUInt16(__val)
#define Endian_HostToLittle32(__val) Endian_SwapUInt32(__val)
#define Endian_HostToLittle64(__val) Endian_SwapUInt64(__val)

#define Endian_BigToHost16(__val) (__val)
#define Endian_BigToHost32(__val) (__val)
#define Endian_BigToHost64(__val) (__val)
#define Endian_LittleToHost16(__val) Endian_SwapUInt16(__val)
#define Endian_LittleToHost32(__val) Endian_SwapUInt32(__val)
#define Endian_LittleToHost64(__val) Endian_SwapUInt64(__val)

#else

#define Endian_HostToBig16(__val) Endian_SwapUInt16(__val)
#define Endian_HostToBig32(__val) Endian_SwapUInt32(__val)
#define Endian_HostToBig64(__val) Endian_SwapUInt64(__val)
#define Endian_HostToLittle16(__val) (__val)
#define Endian_HostToLittle32(__val) (__val)
#define Endian_HostToLittle64(__val) (__val)

#define Endian_BigToHost16(__val) Endian_SwapUInt16(__val)
#define Endian_BigToHost32(__val) Endian_SwapUInt32(__val)
#define Endian_BigToHost64(__val) Endian_SwapUInt64(__val)
#define Endian_LittleToHost16(__val) (__val)
#define Endian_LittleToHost32(__val) (__val)
#define Endian_LittleToHost64(__val) (__val)

#endif

#ifdef __cplusplus
}
#endif
