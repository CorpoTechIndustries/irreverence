#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef bool(*sort_func_t)(void* left, void* right);
void Sort_Merge(uint32_t* list, size_t count, sort_func_t fn);
