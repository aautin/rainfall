# 

```
level7@RainFall:~$ ./level7
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 a
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 a a
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

our goal:     substitute the puts calling at the end by the call of m
how to do it: use vulnerability of malloc and strcpy to overwrite the return address of the puts in the plt section