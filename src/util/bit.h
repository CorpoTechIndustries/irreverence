#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BIT_HEX(bit) (1 << bit)
#define BIT_TURNON(bitset, bit) (bitset |= BIT_HEX(bit))
#define BIT_TURNOFF(bitset, bit) (bitset &= ~BIT_HEX(bit))
#define BIT_TOGGLE(bitset, bit) (bitset ^= BIT_HEX(bit))
#define BIT_ISSET(bitset, bit) ((bitset & (1 << bit)) > 0) // NOTE: don't need "> 0", but lets have output between 0 and 1