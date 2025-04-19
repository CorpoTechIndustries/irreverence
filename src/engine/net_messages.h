#pragma once

// Client to server net messages
typedef enum {
	CLC_NOP,
	CLC_DISCONNECT,
	CLC_MOVE
} clc_messages_t;

// Server to client net messages
typedef enum {
	SVC_NOP,
	SVC_DISCONNECT,

	SVC_EDICT_CREATE,
	SVC_EDICT_DESTROY,
	SVC_EDICT_UPDATE,
} svc_messages_t;
