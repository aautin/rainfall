#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int	 number;
	char string_number[40]; // 0x3c-0x14

	number = atoi(argv[1]);
	if (!(number <= 9))
	{
		return 0;
	}

	memcpy(string_number, argv[2], number * 4);

	if (number == 0x574f4c46)
	{
		execl("/bin/sh", "sh", NULL);
	}

	return 0;
}
