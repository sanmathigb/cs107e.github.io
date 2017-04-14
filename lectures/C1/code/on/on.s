// Assembly program that turns on GPIO 20

asm:	// configure GPIO 20 for output
ldr r0, =0x20200008
mov r1, #1
str r1, [r0]

// set GPIO 20 high
ldr r0, =0x2020001C
mov r1, #(1<<20)
str r1, [r0] 

