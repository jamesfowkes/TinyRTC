#ifndef _IO_H_
#define _IO_H_

enum io_type
{
	INPUT,
	OUTPUT
};
typedef enum io_type IO_TYPE;

enum io_state
{
	ON,
	OFF,
	UNKNOWN
};
typedef enum io_state IO_STATE;

bool parse_chars_to_io_type(IO_TYPE * io_type, char * chars);

IO_STATE app_get_io_state(int input_to_read);

#endif