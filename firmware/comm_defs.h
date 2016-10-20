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

#endif