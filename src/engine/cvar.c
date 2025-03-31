#include <engine/cvar.h>
#include <stddef.h>
#include <string.h>

#include <platform/memory.h>

struct cvar_node_t {
	cvar_t* cvar;
	struct cvar_node_t* next;
};

struct cvar_node_t* s_pCVarHead = NULL;

void CVar_Register(cvar_t* cvar)
{
	if (cvar->type == CVAR_STRING && cvar->value.as_string) {
		const char* tmp = cvar->value.as_string;

		cvar->value.as_string = Sys_Calloc(strlen(tmp) + 1);

		strcpy(cvar->value.as_string, tmp);
	}

	if (!s_pCVarHead) {
		s_pCVarHead = Sys_Calloc(sizeof(struct cvar_node_t));

		s_pCVarHead->cvar = cvar;
		return;
	}

	struct cvar_node_t* node = s_pCVarHead;

	while (node->next) {
		node = node->next;
	}

	struct cvar_node_t* new_node = Sys_Calloc(sizeof(struct cvar_node_t));
	new_node->cvar = cvar;

	node->next = new_node;
}

cvar_t* CVar_Get(const char* name)
{
	struct cvar_node_t* node = s_pCVarHead;

	if (!node) {
		return NULL;
	}

	do {
		if (strcmp(node->cvar->name, name) == 0) {
			return node->cvar;
		}

	} while(node = node->next);

	return NULL;
}

int CVar_GetInt(const char* name)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return 0;
	}

	return cvar->value.as_int;
}

float CVar_GetFloat(const char* name)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return 0.0f;
	}

	return cvar->value.as_float;
}

bool CVar_GetBool(const char* name)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return false;
	}

	return cvar->value.as_bool;
}

const char* CVar_GetString(const char* name)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return NULL;
	}

	if (cvar->type != CVAR_STRING) {
		return NULL;
	}

	return cvar->value.as_string;
}

void CVar_SetInt(const char* name, int value)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return;
	}

	if (cvar->type != CVAR_INT) {
		return;
	}

	cvar->value.as_int = value;
}

void CVar_SetFloat(const char* name, float value)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return;
	}

	if (cvar->type != CVAR_BOOL) {
		return;
	}

	cvar->value.as_float = value;
}

void CVar_SetBool(const char* name, bool value)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return;
	}

	if (cvar->type != CVAR_BOOL) {
		return;
	}

	cvar->value.as_bool = value;
}

void CVar_SetString(const char* name, const char* value)
{
	cvar_t* cvar = CVar_Get(name);

	if (!cvar) {
		return;
	}

	if (cvar->type != CVAR_STRING) {
		return;
	}

	if (cvar->value.as_string) {
		Sys_Free(cvar->value.as_string);
	}

	cvar->value.as_string = Sys_Calloc(strlen(value) + 1);
	strcpy(cvar->value.as_string, value);
}
