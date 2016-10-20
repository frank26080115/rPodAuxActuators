#include "pid.h"

void pid_reset(pid_data_t* data)
{
	data->integral = 0;
	data->prev_error = 0;
}

float pid_calc(pid_const_t* consts, pid_data_t* state, float curr, float tgt)
{
	float x, e, i, d;

	e = tgt - curr; // calculate current error;
	i = state->integral + e; // accumulate new error into sum
	d = e - state->prev_error; // derivative term

	// simple PID equation
	x  = e * consts->p;
	x += i * consts->i;
	x += d * consts->d;

	state->integral   = i * consts->i_decay;
	state->integral   = state->integral < consts->i_max ? (state->integral > consts->i_min ? state->integral : consts->i_min) : consts->i_max;
	state->prev_error = e;

	// constrain output to limits
	x = x < consts->out_max ? (x > consts->out_min ? x : consts->out_min) : consts->out_max;

	return x;
}