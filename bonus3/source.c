#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
	char buffer[132]; // 65 + 66 + 1 for null terminator

	FILE *f = fopen("/home/user/end/.pass", "r");

	memset(buffer, 0, 132);
	if (!f)
	{
		return -1;
	}
	if (argc != 2)
	{
		return -1;
	}

	fread(buffer, 1, 66, f);
	buffer[65] = 0;

	int atoi_retval = atoi(argv[1]);
	buffer[atoi_retval] = 0;

	fread(&buffer[66], 1, 65, f);
	fclose(f);

	if (strcmp(buffer, av[1]) == 0)
		execl("/bin/sh", "sh", NULL);
	else
		puts(&buffer[66]);

	return (0);
}