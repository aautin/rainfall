#include <stdio.h>
#include <stdlib.h>

int		m = 0; // 0x804988c

int		v(void)
{
	char	buffer[512];

	fgets(buffer, 512, stdin); // 0x200 = 512
	printf(buffer);
	if (m == 64) // 0x40 = 64
	{
		fwrite("Wait what?!\n", 12, 1, stdout);
		system("/bin/sh");
	}
	return (0);
}

int		main(void)
{
	v();
	return (0);
}