// Generated with Ghidra
// Manually edited for readability and fix its misunderstandings

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *auth    = (void*) 0; // 0x8049aac
char *service = (void*) 0; // 0x8049ab0

int asm_strcompare(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int main()
{
	char buffer[0x80]; // 128 bytes

	while (true)
	{
		printf("%p, %p\n", auth, service);

		if (!fgets(buffer, 128, stdin))
			break;

		if (!asm_strcompare(buffer, "auth ", 5))
		{
			auth = malloc(4);
			auth[0] = 0;
			if (strlen(buffer + 5) <= 30)
				strcpy(auth, buffer + 5);
		}
		
        if (!asm_strcompare(buffer, "reset", 5)) // we wont use this part of code
			free(auth);
		
        if (!asm_strcompare(buffer, "service", 6))
			service = strdup(buffer + 7);
		
        if (!asm_strcompare(buffer, "login", 5))
		{
			if (auth[32] != 0) system("/bin/sh");
			else fwrite("Password:\n", 10, 1, stdout);
		}
	}
}