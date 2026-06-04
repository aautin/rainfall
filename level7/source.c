// Generated with Ghidra
// Manually edited for readability and fix its misunderstandings

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char buffer[68];

void	m(void)
{
	printf("%s - %d\n", buffer, (int)time(0));
}


int main(int argc, char *argv[])
{
    int *ptr1 = malloc(8);        // stored at esp+0x1c (to esp+0x1f included)
    ptr1[0]   = 1;                // ptr1 first  4-bytes are equal to 1
    ptr1[1]   = malloc(8);        // ptr1 second 4-bytes are equal to the address
                                  // ... of a new 8-bytes malloc

    int *ptr2 = (int *)malloc(8); // stored at esp+0x18 (to esp+0x21 included)
    ptr2[0]   = 2;                // ptr2 first  4-bytes are equal to 2
    ptr2[1]   = malloc(8);        // ptr3 second 4-bytes are equal to the address
                                  // ... of a new 8-bytes malloc

    strcpy((char *)ptr1[1], argv[1]);
    strcpy((char *)ptr2[1], argv[2]);

    fgets(buffer, 68, fopen("/home/user/level8/.pass", "r"));
	  puts("~~");

    return 0;
}