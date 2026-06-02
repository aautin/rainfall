#include <stdio.h>
#include <stdlib.h>

int run(void)
{
    char output[] = "Good... Wait what?\n";

	fwrite(output, 19, 1, stdout);
	return system("/bin/sh");
}

int main(void)
{
	char	buffer[64];
	gets(buffer);

	return (0);
}