	.file	"sdk_preflight_layout.c"
	.text
	.globl	_probe_lua_State_size
	.section .rdata,"dr"
	.align 4
_probe_lua_State_size:
	.long	96
	.globl	_probe_lua_State_openupval
	.align 4
_probe_lua_State_openupval:
	.long	80
	.globl	_probe_TObject_size
	.align 4
_probe_TObject_size:
	.long	16
	.globl	_probe_TObject_value
	.align 4
_probe_TObject_value:
	.long	8
	.globl	_probe_CallInfo_size
	.align 4
_probe_CallInfo_size:
	.long	24
	.globl	_probe_Number_size
	.align 4
_probe_Number_size:
	.long	8
	.ident	"GCC: (MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders, r8) 13.2.0"
