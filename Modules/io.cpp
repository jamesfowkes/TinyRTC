/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*
 * Code Library Includes
 */

#include "Utility/util_simple_compare.h"

/*
 * Application Includes
 */

#include "io.h"

/*
 * Public Functions
 */

bool parse_chars_to_io_type(IO_TYPE * io_type, char * chars)
{
	if (!io_type || !chars) { return false; }

	if (simple_cmp(3, chars, "OUT")) { *io_type = OUTPUT; return true; }
	if (simple_cmp(2, chars, "IN")) { *io_type = INPUT; return true; }

	return false;
}