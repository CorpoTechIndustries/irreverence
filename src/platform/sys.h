#pragma once

#include <stddef.h>

void Sys_Init();
void Sys_Close();

void Sys_Sleep(size_t ms);
void Sys_Exit(int exit_code);
