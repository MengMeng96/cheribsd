/*-
 * Copyright (c) 2011-2015 Robert N. M. Watson
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/syscall.h>
#include <sys/sysctl.h>

#include <ddb/ddb.h>
#include <sys/kdb.h>

#include <machine/atomic.h>
#include <machine/cheri.h>
#include <machine/pcb.h>
#include <machine/sysarch.h>

static const char *cheri_exccode_isa_array[] = {
	"none",					/* CHERI_EXCCODE_NONE */
	"length violation",			/* CHERI_EXCCODE_LENGTH */
	"tag violation",			/* CHERI_EXCCODE_TAG */
	"seal violation",			/* CHERI_EXCCODE_SEAL */
	"type violation",			/* CHERI_EXCCODE_TYPE */
	"call trap",				/* CHERI_EXCCODE_CALL */
	"return trap",				/* CHERI_EXCCODE_RETURN */
	"underflow of trusted system stack",	/* CHERI_EXCCODE_UNDERFLOW */
	"user-defined permission violation",	/* CHERI_EXCCODE_USER */
	"TLB prohibits store capability",	/* CHERI_EXCCODE_TLBSTORE */
    "Bounds cannot be represented precisely", /* 0xa: CHERI_EXCCODE_IMPRECISE */
	"reserved",				/* 0xb: TBD */
	"reserved",				/* 0xc: TBD */
	"reserved",				/* 0xd: TBD */
	"reserved",				/* 0xe: TBD */
	"reserved",				/* 0xf: TBD */
	"global violation",			/* CHERI_EXCCODE_GLOBAL */
	"permit execute violation",		/* CHERI_EXCCODE_PERM_EXECUTE */
	"permit load violation",		/* CHERI_EXCCODE_PERM_LOAD */
	"permit store violation",		/* CHERI_EXCCODE_PERM_STORE */
	"permit load capability violation",	/* CHERI_EXCCODE_PERM_LOADCAP */
	"permit store capability violation",   /* CHERI_EXCCODE_PERM_STORECAP */
     "permit store local capability violation", /* CHERI_EXCCODE_STORE_LOCAL */
	"permit seal violation",		/* CHERI_EXCCODE_PERM_SEAL */
	"reserved",				/* 0x18 */
	"reserved",				/* 0x19 */
	"access EPCC violation",		/* CHERI_EXCCODE_ACCESS_EPCC */
	"access KDC violation",			/* CHERI_EXCCODE_ACCESS_KDC */
	"access KCC violation",			/* CHERI_EXCCODE_ACCESS_KCC */
	"access KR1C violation",		/* CHERI_EXCCODE_ACCESS_KR1C */
	"access KR2C violation",		/* CHERI_EXCCODE_ACCESS_KR2C */
	"reserved",				/* 0x1f */
};
static const int cheri_exccode_isa_array_length =
    sizeof(cheri_exccode_isa_array) / sizeof(cheri_exccode_isa_array[0]);

static const char *cheri_exccode_sw_array[] = {
	"local capability in argument",		/* CHERI_EXCCODE_SW_LOCALARG */
	"local capability in return value",	/* CHERI_EXCCODE_SW_LOCALRET */
	"incorrect CCall registers",		/* CHERI_EXCCODE_SW_CCALLREGS */
};
static const int cheri_exccode_sw_array_length =
    sizeof(cheri_exccode_sw_array) / sizeof(cheri_exccode_sw_array[0]);

const char *
cheri_exccode_string(uint8_t exccode)
{

	if (exccode >= CHERI_EXCCODE_SW_BASE) {
		exccode -= CHERI_EXCCODE_SW_BASE;
		if (exccode >= cheri_exccode_sw_array_length)
			return ("unknown software exception");
		return (cheri_exccode_sw_array[exccode]);
	} else {
		if (exccode >= cheri_exccode_isa_array_length)
			return ("unknown ISA exception");
		return (cheri_exccode_isa_array[exccode]);
	}
}

void
cheri_log_exception_registers(struct trapframe *frame)
{
	struct cheri_frame *cheriframe;

	/* XXXRW: awkward and unmaintainable pointer construction. */
	cheriframe = &(((struct pcb *)frame)->pcb_cheriframe);

	cheri_log_cheri_frame(cheriframe);
}

void
cheri_log_cheri_frame(struct cheri_frame *cheriframe)
{

	/* C0 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c0, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 0);

	/* C1 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c1, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 1);

	/* C2 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c2, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 2);

	/* C3 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c3, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 3);

	/* C4 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c4, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 4);

	/* C5 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c5, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 5);

	/* C6 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c6, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 6);

	/* C7 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c7, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 7);

	/* C8 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c8, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 8);

	/* C9 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c9, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 9);

	/* C10 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c10, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 10);

	/* C11 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c11, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 11);

	/* C12 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c12, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 12);

	/* C13 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c13, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 13);

	/* C14 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c14, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 14);

	/* C15 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c15, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 15);

	/* C16 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c16, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 16);

	/* C17 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c17, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 17);

	/* C18 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c18, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 18);

	/* C19 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c19, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 19);

	/* C20 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c20, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 20);

	/* C21 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c21, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 21);

	/* C22 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c22, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 22);

	/* C23 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c23, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 23);

	/* C24 */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_c24, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 24);

	/* C26 - IDC */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_idc, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 26);

	/* C31 - saved PCC */
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &cheriframe->cf_pcc, 0);
	CHERI_REG_PRINT(CHERI_CR_CTEMP0, 31);
}

void
cheri_log_exception(struct trapframe *frame, int trap_type)
{
	struct cheri_frame *cheriframe;
	register_t cause;
	uint8_t exccode, regnum;

#ifdef SMP
	printf("cpuid = %d\n", PCPU_GET(cpuid));
#endif
	/* XXXRW: awkward and unmaintainable pointer construction. */
	cheriframe = &(((struct pcb *)frame)->pcb_cheriframe);
	if ((trap_type == T_C2E) || (trap_type == T_C2E + T_USER)) {
		cause = cheriframe->cf_capcause;
		exccode = (cause & CHERI_CAPCAUSE_EXCCODE_MASK) >>
		    CHERI_CAPCAUSE_EXCCODE_SHIFT;
		regnum = cause & CHERI_CAPCAUSE_REGNUM_MASK;
		printf("CHERI cause: ExcCode: 0x%02x ", exccode);
		if (regnum < 32)
			printf("RegNum: $c%02d ", regnum);
		else if (regnum == 255)
			printf("RegNum: PCC ");
		else
			printf("RegNum: invalid (%d) ", regnum);
		printf("(%s)\n", cheri_exccode_string(exccode));
	}
	cheri_log_exception_registers(frame);
}
