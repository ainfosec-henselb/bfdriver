;
; Bareflank Hypervisor
;
; Copyright (C) 2015 Assured Information Security, Inc.
; Author: Bradley Hensel        <henselb@ainfosec.com>
;
; This library is free software; you can redistribute it and/or
; modify it under the terms of the GNU Lesser General Public
; License as published by the Free Software Foundation; either
; version 2.1 of the License, or (at your option) any later version.
;
; This library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
; Lesser General Public License for more details.
;
; You should have received a copy of the GNU Lesser General Public
; License along with this library; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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


global  _set_vmxe
_set_vmxe:

    mov rax, cr4
    or rax, (1<<13)
    mov cr4, rax


global  _unset_vmxe
_unset_vmxe:

    mov rax, cr4
    and rax, ~(1<<13)    
    mov cr4, rax
