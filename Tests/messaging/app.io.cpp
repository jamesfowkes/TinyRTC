/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*
 * Application Includes
 */

#include "io.h"
#include "app.io.h"
 
IO_STATE app_get_io_state(int input_to_read)
{
	static IO_STATE io_states[] = {ON, OFF, OFF, ON};

	if (input_to_read < 4)
	{
		return io_states[input_to_read];
	}

	return UNKNOWN;
}
