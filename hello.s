	.file	"hello.c"
gcc_compiled.:
	.stabs "hello.c",100,0,0,Ltext
Ltext:
.stabs "int:t1=r1;-2147483648;2147483647;",128,0,0,0
.stabs "char:t2=r2;0;127;",128,0,0,0
.stabs "long int:t3=r1;-2147483648;2147483647;",128,0,0,0
.stabs "unsigned int:t4=r1;0;-1;",128,0,0,0
.stabs "long unsigned int:t5=r1;0;-1;",128,0,0,0
.stabs "short int:t6=r1;-32768;32767;",128,0,0,0
.stabs "long long int:t7=r1;0;-1;",128,0,0,0
.stabs "short unsigned int:t8=r1;0;65535;",128,0,0,0
.stabs "long long unsigned int:t9=r1;0;-1;",128,0,0,0
.stabs "signed char:t10=r1;-128;127;",128,0,0,0
.stabs "unsigned char:t11=r1;0;255;",128,0,0,0
.stabs "float:t12=r1;4;0;",128,0,0,0
.stabs "double:t13=r1;8;0;",128,0,0,0
.stabs "long double:t14=r1;8;0;",128,0,0,0
.stabs "void:t15=15",128,0,0,0
.stabs "size_t:t4",128,0,0,0
.stabs "time_t:t3",128,0,0,0
.stabs "ptrdiff_t:t3",128,0,0,0
.stabs "pid_t:t1",128,0,0,0
.stabs "uid_t:t8",128,0,0,0
.stabs "gid_t:t11",128,0,0,0
.stabs "dev_t:t8",128,0,0,0
.stabs "ino_t:t8",128,0,0,0
.stabs "mode_t:t8",128,0,0,0
.stabs "umode_t:t8",128,0,0,0
.stabs "nlink_t:t11",128,0,0,0
.stabs "daddr_t:t1",128,0,0,0
.stabs "off_t:t3",128,0,0,0
.stabs "u_char:t11",128,0,0,0
.stabs "ushort:t8",128,0,0,0
.stabs "div_t:t16=s8quot:1,0,32;rem:1,32,32;;",128,0,0,0
.stabs "ldiv_t:t17=s8quot:3,0,32;rem:3,32,32;;",128,0,0,0
.stabs "ustat:T18=s20f_tfree:1,0,32;f_tinode:8,32,16;\\",128,0,0,0
.stabs "f_fname:19=ar1;0;5;2,48,48;f_fpack:20=ar1;0;5;2,96,48;;",128,0,0,0
.stabs "gint:G1",32,0,0,0
.globl _gint
.data
	.align 2
_gint:
	.long 5
.stabs "gint2:G1",32,0,0,0
.globl _gint2
	.align 2
_gint2:
	.long 6
.text
	.align 2
.globl _foo
_foo:
	.stabd 68,0,4
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
LBB2:
	.stabd 68,0,5
	movl $5,-4(%ebp)
	.stabd 68,0,6
	cmpl $6,-4(%ebp)
	jne L2
	.stabd 68,0,7
	movl $7,-4(%ebp)
L2:
	.stabd 68,0,9
	movl -4(%ebp),%eax
	jmp L1
	.align 2
LBE2:
	.stabd 68,0,10
L1:
	leave
	ret
.stabs "foo:F1",36,0,0,_foo
.stabs "i:1",128,0,0,-4
.stabn 192,0,0,LBB2
.stabn 224,0,0,LBE2
LC0:
	.ascii "trivial string\0"
	.align 2
.globl _main
_main:
	.stabd 68,0,12
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
LBB3:
	.stabd 68,0,13
	movl $LC0,-4(%ebp)
	.stabd 68,0,14
	call _foo
	.stabd 68,0,15
	xorl %eax,%eax
	jmp L3
	.align 2
LBE3:
	.stabd 68,0,16
L3:
	leave
	ret
.stabs "main:F1",36,0,0,_main
.stabs "arbit:21=*2",128,0,0,-4
.stabn 192,0,0,LBB3
.stabn 224,0,0,LBE3
