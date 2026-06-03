## Check binary permissions
in bash : `ls -l` and `getfacl binary`

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

## Check system endianness

in shell : `echo -n I | od -to2 | head -n1 | cut -f2 -d" " | cut -c6`
if the output is 1, it's little-endian, if it's 0, it's big-endian

## Turn asm instructions into a C code

Use [assembly-to-c-converter](https://www.codeconvert.ai/assembly-to-c-converter)

## Typical stack frame
For a function p:
```
void p(int value)
{
	char buffer[0x4c];
	return;
}
```

The stack frame looks like this :
```
	High Addresses
	+-------------------+
	| **input (0)**     |  ← [EBP + 8]  (argument)
	+-------------------+
	| Return Address    |  ← [EBP + 4]  (where to go after `p` returns)
	+-------------------+
	| Saved EBP         |  ← [EBP]      (saved by `push ebp`)
	+-------------------+
	| buffer[76]        |  ← [EBP - 76] (local variable)
	+-------------------+
	Low Addresses
```
