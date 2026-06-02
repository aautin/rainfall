## Check the System Architecture

in bash : 
```
	~$ uname -m 
	i686
```
meaning it's 32-bits architecture : addresses are 4-bytes sized (a `DWORD` in asm)

## Disassemble a binary into asm dump

in bash : `objdump -M intel -d binary`

in gdb : `disassemble function_name`

## Change gdb asm syntax

in gdb : ` set disassembly-flavor syntax_name`