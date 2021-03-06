#-
# Copyright (c) 2012-2017 Robert N. M. Watson
# All rights reserved.
#
# This software was developed by SRI International and the University of
# Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
# ("CTSRD"), as part of the DARPA CRASH research programme.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

#include <machine/cherireg.h>
#define _CHERI_INTERNAL
#include <machine/cheriasm.h>
#include "abicall.s"

#.set mips64
.set noreorder
#.set nobopt
.set noat

#
# CHERI sandbox runtime startup code -- begins execution inside a sandbox and
# is responsible for bootstrapping MIPS-compiled C code (without CHERI
# extensions).
#
# Expectations on the CCall mechanism
#    $c1, $c2 contain the sealed invoked code and data capability
#    $pcc contains the unsealed invoked code capability
#    $idc contains the unsealed invoked data capability
#    $ddc contains a suitable unsealed Default Data Capability
#    $csp will contain a suitable unsealed stack capability
#
# If we want to support executing hybrid-ABI binaries in sandboxes again, we
# will either need the invocation trampoline to install $c11/$sp instead of
# $csp, or we will need the landing vectors for the two ABIs to differ.
#
# Expectations on the caller
#    $v0 contains the method number (if any)
#    $a0-$a7 will contain non-capability arguments
#    $c3-$c10 will contain capability arguments
#
# Expectations on the class code and instance setup:
#    $ddc, $pcc contain access to (100% overlapped) sandbox code and data
#    $csp will be set up to point at a suitable stack
#
# __invoke_vector will jump to the 'invoke' symbol and arrange a CReturn when
# that function returns.  This code also implements 'abort' and '_exit'
# symbols that can be used to fall back out quickly and efficiently.
#
# On return:
#    $v0, $v1 will contain non-capability return values
#    $c3 will contain a capability return value
#
# Sandbox heap information is extracted from the sandbox metadata structure.
#
# For now, assume:
# (1) The caller has not set up the general-purpose register context, that's
#     our job.
# (2) The goal is not to set up the CHERI calling convention .. yet; we will
#     run only native MIPS64 code, or hand-crafted CHERI assembler.  Thus
#     must change before we can run Clang-compiled CHERI code.
#

# XXXAR: LLD linker script output section assignment appears to be broken
# right now. It just assigns the input section name to the output section.
# To work around this we just give it the name of the output section that we want
#       .text
        .section invoke_vector_text,"ax",@progbits
	.option pic0
	.global __invoke_vector
	.ent __invoke_vector
__invoke_vector:

	#
	# Load $idc-relative offset of the Sandbox Metadata (SBM) area.
	#
	dli	$t0, 0x1000

	#
	# The compiler is responsible for scrubbing unused argument registers
	# (since only it has the information required to do so).  CCall is
	# responsible for scrubbing all other registers.
	#

	#
	# Set up global pointer.
	#
	# XXXRW: Should this now be setting $cgp?
	#
	dla	$gp, _gp

	#
	# Skip vtable dispatch if $v0 is -1
	daddi	$t1, $0, -1
	beq	$v0, $t1, legacy_invoke
	nop			# Branch-delay slot

	#
	# Load the vtable address from sandbox metadata.
	#
	# XXXBD: See XXXRW comment in crt_rtld.S constructors
	#
	clc	$c13, $t0, 32 + 2*CHERICAP_SIZE($c26)
	cld	$t9, $v0, 0($c13)
	abicall	$t9
	b	creturn_target
	nop

legacy_invoke:
	#
	# Call the MIPS/CHERI-ABI C "invoke" function.
	#
	dla	$t9, invoke
	abicall	$t9

creturn_target:
	#
	# The compiler is responsible for scrubbing unused return-value
	# registers # (since only it has the information required to do so).
	# CReturn is responsible for scrubbing all other registers.
	#

	#
	# Return to caller -- load sbm_creturn_object.{co_code, co_data} into
	# $c1 and $c2, and ccall into the creturn trampoline.
	#
	# XXXRW: Do we need to CGetDefault to load the right $c26 here?
	#
	cgetdefault	$c26
	dli	$t0, 0x1000
	clc	$c1, $t0, 32 + 4*CHERICAP_SIZE($c26)
	clc	$c2, $t0, 32 + 5*CHERICAP_SIZE($c26)
	CCALL($c1, $c2)

	.end __invoke_vector

	.global abort
	.ent abort
abort:
	#
	# In general, the compiler should be scrubbing unused return values,
	# as  only it has the information to do so.  However, in the case of
	# abort(), that responsibility lies with the runtime: we explicitly
	# set $v0, and clear $v1/$c3.
	#
	cfromptr	$c3, $c3, $zero
	dli	$v0, -2
	b	creturn_target
	dli	$v1, 0		# Branch-delay slot
	.end abort

	.global _exit
	.ent _exit
_exit:
	cfromptr	$c3, $c3, $zero
	move	$v0, $a0
	b	creturn_target
	dli	$v1, 0		# Branch-delay slot
	.end _exit

	.data
