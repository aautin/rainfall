#include <stdlib.h>
#include <string.h>

char	*p(char *s, char *str)
{
	char	buffer[4096];

	puts(str);
	
    read(0, buffer, 4096);
	strchr(buffer, '\n')[0] = 0;

	return strncpy(s, buffer, 20);
}

char *pp(char *buffer)
{
	char		 a[20];
	char		 b[20];
	unsigned int len;

	p(b, " - ");
	p(a, " - ");
	strcpy(buffer, b);

	len = strlen(buffer);
	buffer[len] = ' ';
	buffer[len + 1] = 0;

	return strcat(buffer, a);
}

int main()
{
	char buffer[0x40 - 0x16]; // 42

	pp(buffer);
	puts(buffer);

    return 0;
}