/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>

static char s_messages[64][64];
static uint8_t s_nMessages = 0;

bool updateMessages()
{
	char c;
	do {
		c = getchar();
		s_messages[s_nMessages][s_charCount++] = c;
	} while (c != '/n');
	s_nMessages++;

	return (s_nMessages > 0);
}


char * getNextMessage()
{
	return (s_nMessages > 0) ? s_messages[s_nMessages-1] : NULL;
	s_nMessages--;
}