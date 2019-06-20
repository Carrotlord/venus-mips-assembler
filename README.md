# Venus MIPS Assembler

This program will convert 32-bit MIPS assembly source code into compiled binary files. Other features such as displaying the hexadecimal values from a file are included.

To compile from scratch, simply use `gcc *.c -o venus` in the main directory.

## Usage Details

```
venus [asm source file] [FLAGS]
        -t [number]     Run numbered tests starting at 0
        -h [file]       Read file as hex pairs
        -i [file]       Read file as 32-bit MIPS commands
        -o [file]       Output assembled binary into file
        -l [file]       Label dump of MIPS source file
```

## Usage examples

`$` indicates the shell prompt.

```
$ venus -t 1
$ venus test-cases/program.asm -o program.mips
$ venus -h expected-output/program.mips
$ venus -i expected-output/labels.mips
$ venus -l test-cases/labels2.asm
```
