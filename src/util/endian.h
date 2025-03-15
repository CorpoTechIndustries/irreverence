#pragma once

#include <stdint.h>

uint16_t Endian_SwapUInt16(uint16_t value);
uint32_t Endian_SwapUInt32(uint32_t value);
uint64_t Endian_SwapUInt64(uint64_t value);

#if CPU_BIG_ENDIAN

#define Endian_CPUToBig16(__val) (__val)
#define Endian_CPUToBig32(__val) (__val)
#define Endian_CPUToBig64(__val) (__val)
#define Endian_CPUToLittle16(__val) Endian_SwapUInt16(__val)
#define Endian_CPUToLittle32(__val) Endian_SwapUInt32(__val)
#define Endian_CPUToLittle64(__val) Endian_SwapUInt64(__val)

#else

#define Endian_CPUToBig16(__val) Endian_SwapUInt16(__val)
#define Endian_CPUToBig32(__val) Endian_SwapUInt32(__val)
#define Endian_CPUToBig64(__val) Endian_SwapUInt64(__val)
#define Endian_CPUToLittle16(__val) (__val)
#define Endian_CPUToLittle32(__val) (__val)
#define Endian_CPUToLittle64(__val) (__val)

#endif
