	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14	sdk_version 10, 14
	.globl	_thread                 ## -- Begin function thread
	.p2align	4, 0x90
_thread:                                ## @thread
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movl	(%rdi), %eax
	movl	%eax, -16(%rbp)
	movl	$0, -12(%rbp)
LBB0_1:                                 ## =>This Inner Loop Header: Depth=1
	movl	-12(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jge	LBB0_4
## %bb.2:                               ##   in Loop: Header=BB0_1 Depth=1
	movl	_cnt(%rip), %eax
	addl	$1, %eax
	movl	%eax, _cnt(%rip)
## %bb.3:                               ##   in Loop: Header=BB0_1 Depth=1
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	LBB0_1
LBB0_4:
	xorl	%eax, %eax
                                        ## kill: def $rax killed $eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	xorl	%eax, %eax
	movl	%eax, %esi
	movl	$0, -4(%rbp)
	movl	$100000, -8(%rbp)       ## imm = 0x186A0
	leaq	-8(%rbp), %rcx
	leaq	-16(%rbp), %rdi
	leaq	_thread(%rip), %rdx
	callq	_pthread_create
	xorl	%r8d, %r8d
	movl	%r8d, %esi
	leaq	-8(%rbp), %rcx
	leaq	-24(%rbp), %rdi
	leaq	_thread(%rip), %rdx
	movl	%eax, -28(%rbp)         ## 4-byte Spill
	callq	_pthread_create
	xorl	%r8d, %r8d
	movl	%r8d, %esi
	movq	-16(%rbp), %rdi
	movl	%eax, -32(%rbp)         ## 4-byte Spill
	callq	_pthread_join
	xorl	%r8d, %r8d
	movl	%r8d, %esi
	movq	-24(%rbp), %rdi
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	callq	_pthread_join
	movl	_cnt(%rip), %r8d
	movl	-8(%rbp), %r9d
	shll	$1, %r9d
	cmpl	%r9d, %r8d
	movl	%eax, -40(%rbp)         ## 4-byte Spill
	je	LBB1_2
## %bb.1:
	movl	_cnt(%rip), %esi
	leaq	L_.str(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -44(%rbp)         ## 4-byte Spill
	jmp	LBB1_3
LBB1_2:
	movl	_cnt(%rip), %esi
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -48(%rbp)         ## 4-byte Spill
LBB1_3:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_cnt                    ## @cnt
.zerofill __DATA,__common,_cnt,4,2
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"ops cnt is %d"

L_.str.1:                               ## @.str.1
	.asciz	"you are luck! cnt is %d"


.subsections_via_symbols
