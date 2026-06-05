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
