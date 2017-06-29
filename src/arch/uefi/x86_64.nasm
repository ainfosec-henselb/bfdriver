

bits 64
default rel

section .text

global  _writeeflags
_writeeflags:
    
    push rdi
    popfq

    ret


global  _readeflags
_readeflags:
    
    pushfq
    pop rax

    ret


global  _sgdt
_sgdt:
    
    sgdt [rdi]

    ret


global  _lgdt
_lgdt:
    
    lgdt [rdi]

    ret


global  _ltr
_ltr:
    
    ltr di

    ret


global  _str
_str:
    
    str ax

    ret


