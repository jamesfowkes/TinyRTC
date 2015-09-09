#ifndef _IO_H_
#define _IO_H_

enum io_type
{
	INPUT,
	OUTPUT
};
typedef enum io_type IO_TYPE;

bool parse_chars_to_io_type(IO_TYPE * io_type, char * chars);

#endif