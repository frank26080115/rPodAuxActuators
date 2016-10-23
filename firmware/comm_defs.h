#ifndef _COMM_DEFS_H_
#define _COMM_DEFS_H_

#include <stdint.h>

enum
{
	OPCODE_PING, // receiver must reply with the same data buffer, but with "OPCODE_PONG" as the opcode
	OPCODE_PONG,
	OPCODE_ALL_STOP, // stop all motors exactly where they are
	OPCODE_LIFT, // sets a target for the lifting linear actuator servos
	OPCODE_DRIVE, // sets a speed for aux service propulsion
};

typedef struct
{
	uint32_t target;
	uint32_t speed;
}
comm_liftcmd_t;

typedef struct
{
	int32_t speed;
}
comm_drivecmd_t;

typedef struct
{
	uint32_t addr;
	enum 
	{
		T_U64,
		T_S64,
		T_U32,
		T_S32,
		T_U16,
		T_S16,
		T_U8,
		T_S8,
		T_FLT,
		T_DBL,
	} typecode;
	union
	{
		uint64_t u64;
		int64_t s64;
		uint32_t u32;
		int32_t s32;
		uint16_t u16;
		int16_t s16;
		uint16_t u8;
		int16_t s8;
		float flt;
		double dbl;
	} data;
}
comm_setparam_t;

typedef struct
{
	uint32_t addr;
	uint8_t len;
	uint8_t data[16];
}
comm_setparamblock_t;

#endif