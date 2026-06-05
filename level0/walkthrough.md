# SIMPLE ATOI CHECKING BYPASS

## What are the binary permissions ?
```
~$ ls -l
total 732
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0
```
Meaning that when we execute the binary, the effective user id (euid) becomes level1's. So at the beginning of the binary execution:
- real user id (ruid) = level0
- effective user id (euid) = level1
- group id (gid) = users

## What does this binary do ?

### A vulnerable comparison  

```asm
   0x08048ec9 <+9>:     mov    eax,DWORD PTR [ebp+0xc]
   0x08048ecc <+12>:    add    eax,0x4
   0x08048ecf <+15>:    mov    eax,DWORD PTR [eax]
```
Move argv[0] in eax, then add 4 (size of an address) to get to the next argument argv[1] and move it in eax

```asm
   0x08048ed1 <+17>:    mov    DWORD PTR [esp],eax
   0x08048ed4 <+20>:    call   0x8049710 <atoi>
   0x08048ed9 <+25>:    cmp    eax,0x1a7
```
compare argv[1] to 0x1a7 (423)

```asm
   0x08048ede <+30>:    jne    0x8048f58 <main+152>
```
if not equal, jump to 0x8048f58, which avoid the shell opening code

```
   (gdb) p (char*) 0x80c5348
   $1 = 0x80c5348 "/bin/sh"
```
```asm
   0x08048ee0 <+32>:    mov    DWORD PTR [esp],0x80c5348
   0x08048ee7 <+39>:    call   0x8050bf0 <strdup>
   0x08048eec <+44>:    mov    DWORD PTR [esp+0x10],eax
   0x08048ef0 <+48>:    mov    DWORD PTR [esp+0x14],0x0
```
put the address of the string "/bin/sh" at esp+0x10, and NULL at esp+0x14

```asm
   0x08048ef8 <+56>:    call   0x8054680 <getegid>
   0x08048efd <+61>:    mov    DWORD PTR [esp+0x1c],eax
   0x08048f01 <+65>:    call   0x8054670 <geteuid>
   0x08048f06 <+70>:    mov    DWORD PTR [esp+0x18],eax
   0x08048f0a <+74>:    mov    eax,DWORD PTR [esp+0x1c]
   0x08048f0e <+78>:    mov    DWORD PTR [esp+0x8],eax
   0x08048f12 <+82>:    mov    eax,DWORD PTR [esp+0x1c]
   0x08048f16 <+86>:    mov    DWORD PTR [esp+0x4],eax
   0x08048f1a <+90>:    mov    eax,DWORD PTR [esp+0x1c]
   0x08048f1e <+94>:    mov    DWORD PTR [esp],eax
   0x08048f21 <+97>:    call   0x8054700 <setresgid>
   0x08048f26 <+102>:   mov    eax,DWORD PTR [esp+0x18]
   0x08048f2a <+106>:   mov    DWORD PTR [esp+0x8],eax
   0x08048f2e <+110>:   mov    eax,DWORD PTR [esp+0x18]
   0x08048f32 <+114>:   mov    DWORD PTR [esp+0x4],eax
   0x08048f36 <+118>:   mov    eax,DWORD PTR [esp+0x18]
   0x08048f3a <+122>:   mov    DWORD PTR [esp],eax
   0x08048f3d <+125>:   call   0x8054690 <setresuid>
```
set the real uid and gid to the effective ones, now :
- real user id (ruid) = level1
- effective user id (euid) = level1
- group id (gid) = users

```asm
   0x08048f42 <+130>:   lea    eax,[esp+0x10]
   0x08048f46 <+134>:   mov    DWORD PTR [esp+0x4],eax
   0x08048f4a <+138>:   mov    DWORD PTR [esp],0x80c5348
   0x08048f51 <+145>:   call   0x8054640 <execv>
```
call execv("/bin/sh", ["/bin/sh", NULL]) and the shell is opened with the 3 ids set to level1

## **Summary**

We can bypass the comparison with `argv[1] = "423"` to open a shell as user level1. Then `/home/user/level1` becomes accessible, we have the access to its `.pass`
