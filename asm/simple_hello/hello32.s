# -----------------------------------------------------------------------------
# A 32-bit Linux standalone program that writes "Hello, World" to the console
# using system calls only.  The program does not need to link with any external
# libraries at all.
#
# System calls used:
#   4: write(fileid, bufferAddress, numberOfBytes)
#   1: exit(returnCode)
#
# Assemble:
#     gcc -c hello.s
# Link:
#     ld hello.o (to produce a.out)
#     (or) ld -o hello hello.o (to produce hello)
#
# Or, you can assemble and link in one step:
#     gcc -nostdlib hello.s
#
# The symbol _start is the default entry point for ld.
# -----------------------------------------------------------------------------

        .global _start

        .text
_start:
        # write(1, message, 13)
        mov     $4, %eax                # system call 4 is write
        mov     $1, %ebx                # file handle 1 is stdout
        mov     $message, %ecx          # address of string to output
        mov     $13, %edx               # number of bytes to write
        int     $0x80                   # invoke operating system code
        
        # exit(0)
        mov     $1, %eax                # system call 1 is exit
        xor     %ebx, %ebx              # we want return code 0
        int     $0x80                   # invoke operating system code
message:
        .ascii  "Hello, World\n"
