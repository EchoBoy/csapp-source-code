main.out:
(__TEXT,__text) section
_main:
0000000100000f30	pushq	%rbp
0000000100000f31	movq	%rsp, %rbp
0000000100000f34	subq	$0x10, %rsp
0000000100000f38	movl	$0x0, -0x4(%rbp)
0000000100000f3f	leaq	0x56(%rip), %rsi
0000000100000f46	xorl	%eax, %eax
0000000100000f48	movb	%al, %cl
0000000100000f4a	movl	$0x1, %edi
0000000100000f4f	movl	$0xd, %edx
0000000100000f54	movl	%eax, -0x8(%rbp)
0000000100000f57	movb	%cl, %al
0000000100000f59	callq	0x100000f70
0000000100000f5e	movl	-0x8(%rbp), %edi
0000000100000f61	movl	%eax, -0xc(%rbp)
0000000100000f64	callq	0x100000f6a
