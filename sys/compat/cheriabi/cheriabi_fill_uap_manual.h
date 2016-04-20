/*-
 * Copyright (c) 2016 SRI International
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract FA8750-10-C-0237
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

static inline int
CHERIABI_SYS_cheriabi_ioctl_fill_uap(struct thread *td,
    struct cheriabi_ioctl_args *uap)
{
	struct chericap tmpcap;
	u_int tag;
	register_t reqperms;
	int error;

	/* [0] int fd */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_ioctl, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->fd, CHERI_CR_CTEMP0);

	/* [1] u_long com */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_ioctl, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->com, CHERI_CR_CTEMP0);

	/* [2] _Inout_opt_ caddr_t data */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_ioctl, 2);
	if (uap->com & IOC_VOID) {
		CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
		CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
		if (!tag)
			CHERI_CTOINT(uap->data, CHERI_CR_CTEMP0);
		else
			return (EPROT);
	} else {
		reqperms = (CHERI_PERM_GLOBAL);
		if (uap->com & IOC_IN)
			reqperms |= CHERI_PERM_LOAD;
		if (uap->com & IOC_OUT)
			reqperms |= CHERI_PERM_STORE;
		if (ioctl_data_contains_pointers(uap->com)) {
			if (reqperms & CHERI_PERM_LOAD)
				reqperms |= CHERI_PERM_LOAD_CAP;
			if (reqperms & CHERI_PERM_STORE)
				reqperms |= CHERI_PERM_STORE_CAP;
		}

		/*
		 * XXX-BD: not sure about may_be_null=1 here, but lower
		 * levels will fail cleanly is it is a problem.
		 */
		error = cheriabi_cap_to_ptr((caddr_t *)&uap->data,
		    &tmpcap, IOCPARM_LEN(uap->com), reqperms, 1);
		if (error != 0)
			return (error);
	}

	return (0);
}

static inline int
CHERIABI_SYS_mincore_fill_uap(struct thread *td,
    struct mincore_args *uap)
{
	struct chericap tmpcap;
	u_int tag;
	int error;
	register_t perms, reqperms;
	register_t sealed;
	size_t base, length, offset;
	size_t addr_adjust;

	/* [1] size_t len */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_mincore, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->len, CHERI_CR_CTEMP0);

	/* [0] _In_pagerange_(len) const void * addr */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_mincore, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
	if (!tag) {
		return (EPROT);
	} else {
		CHERI_CGETPERM(perms, CHERI_CR_CTEMP0);
		reqperms = (CHERI_PERM_GLOBAL|CHERI_PERM_LOAD);
		if ((perms & reqperms) != reqperms)
			return (EPROT);

		CHERI_CGETSEALED(sealed, CHERI_CR_CTEMP0);
		if (sealed)
			return (EPROT);

		CHERI_CGETLEN(length, CHERI_CR_CTEMP0);
		CHERI_CGETLEN(offset, CHERI_CR_CTEMP0);
		if (offset >= length)
			return (EPROT);
		length -= offset;
		CHERI_CGETLEN(base, CHERI_CR_CTEMP0);
		if (rounddown2(base + offset, PAGE_SIZE) < base)
			return (EPROT);
		addr_adjust = ((base + offset) & PAGE_MASK);
		length += addr_adjust;
		if (length < roundup2(uap->len + addr_adjust, PAGE_SIZE))
			return (EPROT);

		CHERI_CTOPTR(uap->addr, CHERI_CR_CTEMP0, CHERI_CR_KDC);
	}

	/* [2] _Out_writes_bytes_(len/PAGE_SIZE) char * vec */
	/*
	 * The actual length depends on the alignment of addr and the
	 * alignment of length.  If everything is well aligned, then
	 * len/PAGE_SIZE is the number of bytes written, but if addr is
	 * near the end of a page and len spans into the beginning of the
	 * last page, then we may write another byte.
	 */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_mincore, 2);
	error = cheriabi_cap_to_ptr((caddr_t *)&uap->vec, &tmpcap,
	    roundup2(uap->len + addr_adjust, PAGE_SIZE) / PAGE_SIZE,
	    CHERI_PERM_GLOBAL|CHERI_PERM_STORE, 0);
	if (error != 0)
		return (error);

	return (0);
}

static inline int
CHERIABI_SYS_fcntl_fill_uap(struct thread *td,
    struct fcntl_args *uap)
{
	struct chericap tmpcap;
	int error;

	/* [0] int fd */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_fcntl, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->fd, CHERI_CR_CTEMP0);

	/* [1] int cmd */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_fcntl, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->cmd, CHERI_CR_CTEMP0);

	/* [2] intptr_t arg */
	/*
	 * There are three cases.  arg is ignored, arg is an int, and arg
	 * is a pointer to struct flock.  We rely on userspace to have
	 * promoted integers to intptr_t so we're only dealing with a
	 * capability argument.
	 */
	switch (uap->cmd) {
	case F_GETFD:
	case F_GETFL:
	case F_GETOWN:
		uap->arg = (intptr_t)NULL;
		break;

	case F_DUPFD:
	case F_DUPFD_CLOEXEC:
	case F_DUP2FD:
	case F_DUP2FD_CLOEXEC:
	case F_SETFD:
	case F_SETFL:
	case F_SETOWN:
	case F_READAHEAD:
	case F_RDAHEAD:
		cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_fcntl, 2);
		CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
		CHERI_CTOINT(uap->arg, CHERI_CR_CTEMP0);
		break;

	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
		cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_fcntl, 2);
		error = cheriabi_cap_to_ptr((caddr_t *)&uap->arg,
		    &tmpcap, sizeof(struct flock),
		    (CHERI_PERM_GLOBAL|CHERI_PERM_LOAD), 0);
		if (error != 0)
			return (error);
		break;
	default:
		return (EINVAL);
	}

	return (0);
}

static inline int
CHERIABI_SYS_cheriabi_sysarch_fill_uap(struct thread *td,
    struct cheriabi_sysarch_args *uap)
{
	struct chericap tmpcap;

	/* [0] int op */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_sysarch, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->op, CHERI_CR_CTEMP0);

	/* [1] char * parms */
	/*
	 * parms could be basically anything and sysarch is fundamentally
	 * machine dependant.  Punt all the duty of checking and
	 * handling parms to the MD cheriabi_sysarch.  This is safe enough
	 * because if porter's fail to do the work, they will end up with
	 * NULL in uap->parms.
	 */
	uap->parms = NULL;

	return (0);
}

static inline int
CHERIABI_SYS_cheriabi_shmat_fill_uap(struct thread *td,
    struct cheriabi_shmat_args *uap)
{
	struct chericap tmpcap;
	u_int tag;
	register_t perms, reqperms;
	register_t sealed;

	/* [0] int shmid */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_shmat, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->shmid, CHERI_CR_CTEMP0);

	/* [2] int shmflg */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_shmat, 2);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->shmflg, CHERI_CR_CTEMP0);

	/* [1] _In_pagerange_opt_(1) void * shmaddr */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_shmat, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
	if (!tag) {
		/* If shmaddr is NULL, then there's nothing further to do.  */
		CHERI_CTOINT(uap->shmaddr, CHERI_CR_CTEMP0);
		if (uap->shmaddr != NULL)
			return (EPROT);
	} else {
		/*
		 * Don't check for any particular permissions beyond
		 * global.  As with mmap(), we'll sort it out on the
		 * outbound path.
		 */
		CHERI_CGETPERM(perms, CHERI_CR_CTEMP0);
		reqperms = (CHERI_PERM_GLOBAL);
		if ((perms & reqperms) != reqperms)
			return (EPROT);

		CHERI_CGETSEALED(sealed, CHERI_CR_CTEMP0);
		if (sealed)
			return (EPROT);

		/*
		 * We need to make sure the capability spans a region
		 * sufficient to fit the segment.  To do that, we need to
		 * look up the segment and find its size.  Unfortunatly,
		 * we can't do that here because we can't hold the sysvshm
		 * lock.  We instead, punt this check to cheriabi_shmat.
		 *
		 * To signal the need to check later, we set shmaddr to
		 * (void *)-1.  This also prevents failure to check later
		 * from working.
		 */
		uap->shmaddr = (void *)-1;
	}

	return (0);
}

static inline int
CHERIABI_SYS_cheriabi_shmdt_fill_uap(struct thread *td,
    struct cheriabi_shmdt_args *uap)
{
	struct chericap tmpcap;
	u_int tag;
	register_t perms, reqperms;
	register_t sealed;

	/* [0] _In_pagerange_(1) void * shmaddr */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_cheriabi_shmdt, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
	if (!tag) {
		return (EPROT);
	} else {
		/*
		 * Don't check for any particular permissions beyond global.
		 */
		CHERI_CGETPERM(perms, CHERI_CR_CTEMP0);
		reqperms = (CHERI_PERM_GLOBAL);
		if ((perms & reqperms) != reqperms)
			return (EPROT);

		CHERI_CGETSEALED(sealed, CHERI_CR_CTEMP0);
		if (sealed)
			return (EPROT);

		/*
		 * We need to make sure the capability spans a region
		 * sufficient to fit the segment.  To do that, we need to
		 * look up the segment and find its size.  Unfortunatly,
		 * we can't do that here because we can't hold the right
		 * locks.  We instead, punt this check to cheriabi_shmdt.
		 *
		 * To signal the need to check later, we set shmaddr to
		 * (void *)-1.  This also prevents failure to check later
		 * from working.
		 */
		uap->shmaddr = (void *)-1;
	}

	return (0);
}

static inline int
CHERIABI_SYS_mac_syscall_fill_uap(struct thread *td,
    struct mac_syscall_args *uap)
{
	struct chericap tmpcap;
	u_int tag;
	int error;

	/* [1] int call */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_mac_syscall, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->call, CHERI_CR_CTEMP0);

	/* [0] _In_z_ char * policy */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_mac_syscall, 0);
	error = cheriabi_strcap_to_ptr(&uap->policy, &tmpcap, 0);
	if (error != 0)
		return (error);

	/* [2] _In_opt_ void * arg */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_mac_syscall, 2);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
	if (!tag) {
		/*
		 * XXX-BD: it may well be that other integers make sense,
		 * but be conservative for now.
		 */
		CHERI_CTOINT(uap->arg, CHERI_CR_CTEMP0);
		if (uap->arg != NULL)
			return (EPROT);
	} else {
		/*
		 * XXX-BD: I've not been able to find the code that
		 * actually uses arg and I believe it will vary by
		 * module.  For now, just fail if the user passes a
		 * capability as I don't know what might be safe.
		 *
		 * Fixing this likely requires pushing user capabilty
		 * pointers down through the MAC framework and into the
		 * modules.
		 */
		return (EINVAL);
	}

	return (0);
}

static inline int
CHERIABI_SYS_auditon_fill_uap(struct thread *td,
    struct auditon_args *uap)
{
	struct chericap tmpcap;
	int error;
	register_t reqperms;

	/* [0] int cmd */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_auditon, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->cmd, CHERI_CR_CTEMP0);

	/* [2] u_int length */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_auditon, 2);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->length, CHERI_CR_CTEMP0);

	/* [1] _Inout_updates_bytes_opt_(length) void * data */
	/*
	 * The required length of data varies considerably.  We operate
	 * on the assumption that the underlying implementation correctly
	 * checks uap->length for the given cmd and just make sure data
	 * has the authority to access that range.
	 *
	 * We also check if we have appropriate permissions for the
	 * given command.  We currently require CHERI_PERM_LOAD in all
	 * cases.  In a few cases, this may be excessive, but it seem
	 * unlikely to break real programs.
	 */
	reqperms = (CHERI_PERM_GLOBAL|CHERI_PERM_LOAD);
	switch (uap->cmd) {
	case A_SETPOLICY:
	case A_SETKAUDIT:
	case A_SETKMASK:
	case A_SETQCTRL:
	case A_SETCOND:
	case A_SETCLASS:
	case A_SETPMASK:
	case A_SETFSIZE:
		/* CHERI_PERM_LOAD added above */
		break;

	case A_GETCLASS:
	case A_GETKAUDIT:
	case A_GETPINFO:
	case A_GETPINFO_ADDR:
	case A_GETSINFO_ADDR:
	case A_GETKMASK:
	case A_GETPOLICY:
	case A_GETQCTRL:
	case A_GETFSIZE:
	case A_GETCOND:
		reqperms |= CHERI_PERM_STORE;
	default:
		return (EINVAL);
	}

	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_auditon, 1);
	error = cheriabi_cap_to_ptr((caddr_t *)uap->data, &tmpcap,
	    uap->length, reqperms, 1);
	if (error != 0)
		return (error);

	return (0);
}

#include <sys/umtx.h>

static inline int
CHERIABI_SYS__umtx_op_fill_uap(struct thread *td,
    struct _umtx_op_args *uap)
{
	struct chericap tmpcap;
	int error;
	u_int tag;
	register_t reqperms;
	size_t reqsize;

	/* [1] int op */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->op, CHERI_CR_CTEMP0);

	/* Short cut, blocking known unimplemted ops */
	switch (uap->op) {
	case UMTX_OP_RESERVED0:			/* __umtx_op_unimpl */
	case UMTX_OP_RESERVED1:			/* __umtx_op_unimpl */
	case UMTX_OP_SEM_WAIT:			/* __umtx_op_unimpl */
	case UMTX_OP_SEM_WAKE:			/* __umtx_op_unimpl */
		return (EOPNOTSUPP);
	}

	/* [2] u_long val */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 2);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->val, CHERI_CR_CTEMP0);

	/* [0] void * obj */
	reqperms = CHERI_PERM_GLOBAL;
	switch (uap->op) {
	case UMTX_OP_WAIT:			/* __umtx_op_wait */
	case UMTX_OP_WAKE:			/* __umtx_op_wake */
	case UMTX_OP_WAKE_PRIVATE: 		/* __umtx_op_wake_private */
		reqsize = sizeof(u_long);
		reqperms |= CHERI_PERM_LOAD;
		break;

	case UMTX_OP_MUTEX_TRYLOCK:		/* __umtx_op_trylock_umutex */
	case UMTX_OP_MUTEX_LOCK:		/* __umtx_op_lock_umutex */
	case UMTX_OP_MUTEX_UNLOCK: 		/* __umtx_op_unlock_umutex */
	case UMTX_OP_MUTEX_WAIT:		/* __umtx_op_wait_umutex */
	case UMTX_OP_MUTEX_WAKE:		/* __umtx_op_wake_umutex */
	case UMTX_OP_MUTEX_WAKE2:		/* __umtx_op_wake2_umutex */
	case UMTX_OP_SET_CEILING:		/* __umtx_op_set_ceiling */
		reqsize = sizeof(struct umutex);
		reqperms |= CHERI_PERM_LOAD|CHERI_PERM_STORE;
		break;

	case UMTX_OP_CV_WAIT:			/* __umtx_op_cv_wait */
	case UMTX_OP_CV_SIGNAL:			/* __umtx_op_cv_signal */
	case UMTX_OP_CV_BROADCAST:		/* __umtx_op_cv_broadcast */
		reqsize = sizeof(struct ucond);
		reqperms |= CHERI_PERM_LOAD|CHERI_PERM_STORE;
		break;

	case UMTX_OP_WAIT_UINT:			/* __umtx_op_wait_uint */
	case UMTX_OP_WAIT_UINT_PRIVATE:	/* __umtx_op_wait_uint_private */
		reqsize = sizeof(u_int);
		reqperms |= CHERI_PERM_LOAD;
		break;

	case UMTX_OP_RW_RDLOCK:			/* __umtx_op_rw_rdlock */
	case UMTX_OP_RW_WRLOCK:			/* __umtx_op_rw_wrlock */
	case UMTX_OP_RW_UNLOCK:			/* __umtx_op_rw_unlock */
		reqsize = sizeof(struct urwlock);
		reqperms |= CHERI_PERM_LOAD|CHERI_PERM_STORE;
		break;

	case UMTX_OP_NWAKE_PRIVATE:		/* __umtx_op_nwake_private */
		/* obj points to an array of (int*) with val elements. */
		reqsize = sizeof(struct chericap) * uap->val;
		reqperms |= CHERI_PERM_LOAD|CHERI_PERM_LOAD_CAP;
		break;

	case UMTX_OP_SEM2_WAIT:			/* __umtx_op_sem2_wait */
	case UMTX_OP_SEM2_WAKE:			/* __umtx_op_sem2_wake */
		reqsize = sizeof(struct _usem2);
		reqperms |= CHERI_PERM_LOAD|CHERI_PERM_STORE;
		break;

#ifdef UMTX_OP_SHM
	case UMTX_OP_SHM:			/* __umtx_op_shm */
		reqsize = 0; /* Expect NULL, obj is unused */
		break;
#endif

	default:
		return (EINVAL);
	}
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 0);
	error = cheriabi_cap_to_ptr((caddr_t *)&uap->obj, &tmpcap,
	    reqsize, reqperms,
#ifdef UMTX_OP_SHM
	    (uap->op == UMTX_OP_SHM));
#else
	    0);
#endif
	if (error != 0)
		return (error);

	/* [3] void * uaddr1 */
	switch (uap->op) {
	case UMTX_OP_WAKE:			/* __umtx_op_wake */
	case UMTX_OP_MUTEX_TRYLOCK:		/* __umtx_op_trylock_umutex */
	case UMTX_OP_MUTEX_UNLOCK: 		/* __umtx_op_unlock_umutex */
	case UMTX_OP_CV_SIGNAL:			/* __umtx_op_cv_signal */
	case UMTX_OP_CV_BROADCAST:		/* __umtx_op_cv_broadcast */
	case UMTX_OP_RW_UNLOCK:			/* __umtx_op_rw_unlock */
	case UMTX_OP_WAKE_PRIVATE: 		/* __umtx_op_wake_private */
	case UMTX_OP_MUTEX_WAKE:		/* __umtx_op_wake_umutex */
	case UMTX_OP_NWAKE_PRIVATE:		/* __umtx_op_nwake_private */
	case UMTX_OP_MUTEX_WAKE2:		/* __umtx_op_wake2_umutex */
	case UMTX_OP_SEM2_WAKE:			/* __umtx_op_sem2_wake */
		/* uaddr1 and uaddr2 are ignored */
		uap->uaddr1 = NULL;
		uap->uaddr2 = NULL;
		return (0);

	case UMTX_OP_WAIT:			/* __umtx_op_wait */
	case UMTX_OP_MUTEX_LOCK:		/* __umtx_op_lock_umutex */
	case UMTX_OP_WAIT_UINT:			/* __umtx_op_wait_uint */
	case UMTX_OP_RW_RDLOCK:			/* __umtx_op_rw_rdlock */
	case UMTX_OP_RW_WRLOCK:			/* __umtx_op_rw_wrlock */
	case UMTX_OP_WAIT_UINT_PRIVATE:	/* __umtx_op_wait_uint_private */
	case UMTX_OP_MUTEX_WAIT:		/* __umtx_op_wait_umutex */
	case UMTX_OP_SEM2_WAIT:			/* __umtx_op_sem2_wait */
		/* uaddr1 is a size_t to pass to umtx_copyin_umtx_time() */
		cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 3);
		CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
		CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
		if (!tag) {
			CHERI_CTOINT(uap->uaddr1, CHERI_CR_CTEMP0);
			/*
			 * The actual test in umtx_copyin_umtx_time() is
			 * very loose and makes little sense.  Allow 0 and
			 * appropriate sizes until such a time as weirder
			 * variants are found in the wild.
			 */
			switch ((size_t)uap->uaddr1) {
			case 0:
				reqsize = sizeof(struct timespec);
				break;
			case sizeof(struct timespec):
			case sizeof(struct _umtx_time):
				reqsize = (size_t)uap->uaddr1;
				break;
			default:
				return (EINVAL);
			}
		} else {
			/* Reject pointers */
			return (EPROT);
		}
		break;

	case UMTX_OP_SET_CEILING:		/* __umtx_op_set_ceiling */
		/*
		 * uaddr1 is a pointer to a writeable uint32_t and may be NULL
		 */
		cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 3);
		error = cheriabi_cap_to_ptr((caddr_t *)&uap->uaddr1, &tmpcap,
		    sizeof(uint32_t), (CHERI_PERM_GLOBAL|CHERI_PERM_STORE), 1);
		if (error != 0)
			return (error);
		/* uaddr2 is ignored */
		uap->uaddr2 = NULL;
		return (0);

	case UMTX_OP_CV_WAIT:			/* __umtx_op_cv_wait */
		cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 3);
		CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
		error = cheriabi_cap_to_ptr((caddr_t *)&uap->uaddr1, &tmpcap,
		    sizeof(struct umutex),
		    (CHERI_PERM_GLOBAL|CHERI_PERM_STORE), 0);
		if (error != 0)
			return (error);
		/* uaddr2 is a struct timespec or NULL */
		reqsize = sizeof(struct timespec);
		break;

#ifdef UMTX_OP_SHM
	case UMTX_OP_SHM:			/* __umtx_op_shm */
		/*
		 * uaddr1 is an address.  The access requirements depend
		 * on val.
		 */
		cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 3);
		CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
		CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
		if (!tag) {
			return (EPROT);
		} else {
			CHERI_CGETPERM(perms, CHERI_CR_CTEMP0);
			reqperms = (CHERI_PERM_GLOBAL);
			if ((perms & reqperms) != reqperms)
				return (EPROT);

			CHERI_CGETSEALED(sealed, CHERI_CR_CTEMP0);
			if (sealed)
				return (EPROT);

			/*
			 * XXX-BD: unclear what length is required and if
			 * length can be checked here and when must be
			 * checked in the syscall.
			 */

			CHERI_CTOPTR(uap->uaddr1, CHERI_CR_CTEMP0, CHERI_CR_KDC);
		}
		/*
		 * XXX-BD: not yet implemented.
		 */
		return (EOPNOTSUPP);

		/* uaddr2 is ignored */
		uap->uaddr2 = NULL;
		return (0);
#endif
	default:
		return (EINVAL);
	}

	/* [4] void * uaddr2 */
	/* Sanity check that only expected ops have made it this far */
	switch (uap->op) {
	case UMTX_OP_WAIT:			/* __umtx_op_wait */
	case UMTX_OP_MUTEX_LOCK:		/* __umtx_op_lock_umutex */
	case UMTX_OP_WAIT_UINT:			/* __umtx_op_wait_uint */
	case UMTX_OP_RW_RDLOCK:			/* __umtx_op_rw_rdlock */
	case UMTX_OP_RW_WRLOCK:			/* __umtx_op_rw_wrlock */
	case UMTX_OP_WAIT_UINT_PRIVATE:	/* __umtx_op_wait_uint_private */
	case UMTX_OP_MUTEX_WAIT:		/* __umtx_op_wait_umutex */
	case UMTX_OP_SEM2_WAIT:			/* __umtx_op_sem2_wait */
	case UMTX_OP_CV_WAIT:			/* __umtx_op_cv_wait */
		break;
	default:
		panic("%s: unexepected op made it to uaddr2 %d", __func__,
		    uap->op);
	}
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS__umtx_op, 4);
	error = cheriabi_cap_to_ptr((caddr_t *)&uap->uaddr2, &tmpcap,
	    reqsize, reqperms, 1);
	if (error != 0)
		return (error);

	return (0);
}

static inline int
CHERIABI_SYS_cheriabi_sigqueue_fill_uap(struct thread *td,
    struct cheriabi_sigqueue_args *uap)
{
	struct chericap tmpcap;

	/* [0] pid_t pid */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi_sigqueue, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->pid, CHERI_CR_CTEMP0);

	/* [1] int signum */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi_sigqueue, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->signum, CHERI_CR_CTEMP0);

	/* [2] _In_opt_ union sigval_c value */
	/*
	 * XXX-BD: what to do here?  value can be either a literal int or
	 * a pointer.  I'm not even sure which register it will be in!
	 *
	 * Just give up for now.
	 */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi_sigqueue, 2);
	return (EOPNOTSUPP);

	return (0);
}

static inline int
CHERIABI_SYS_shm_open_fill_uap(struct thread *td,
    struct shm_open_args *uap)
{
	struct chericap tmpcap;
	u_int tag;
	int error;
	size_t base;

	/* [1] int flags */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_shm_open, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->flags, CHERI_CR_CTEMP0);

	/* [2] mode_t mode */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_shm_open, 2);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->mode, CHERI_CR_CTEMP0);

	/* [0] _In_z_ const char * path */
	cheriabi_fetch_syscall_arg(td, &tmpcap, CHERIABI_SYS_shm_open, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CGETTAG(tag, CHERI_CR_CTEMP0);
	if (!tag) {
		CHERI_CGETBASE(base, CHERI_CR_CTEMP0);
		if (base != 0)
			return (EPROT);
		CHERI_CGETOFFSET(uap->path, CHERI_CR_CTEMP0);
		if (uap->path != SHM_ANON)
			return (EPROT);
	} else {
		error = cheriabi_strcap_to_ptr(&uap->path, &tmpcap, 0);
		if (error != 0)
			return (error);
	}

	return (0);
}

static inline int
CHERIABI_SYS_cheriabi___semctl_fill_uap(struct thread *td,
    struct cheriabi___semctl_args *uap)
{
	struct chericap tmpcap;
	int error;
	register_t reqperms;

	/* [0] int semid */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi___semctl, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->semid, CHERI_CR_CTEMP0);

	/* [1] int semnum */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi___semctl, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->semnum, CHERI_CR_CTEMP0);

	/* [2] int cmd */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi___semctl, 2);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->cmd, CHERI_CR_CTEMP0);

	/* [3] _In_opt_ union semun_c * arg */
	reqperms = (CHERI_PERM_GLOBAL|CHERI_PERM_LOAD);
	switch (uap->cmd) {
	case IPC_STAT:
	case IPC_SET:
	case GETALL:
	case SETALL:
		reqperms |= CHERI_PERM_LOAD_CAP;
	}
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi___semctl, 3);
	error = cheriabi_cap_to_ptr((caddr_t *)&uap->arg, &tmpcap,
	    sizeof(*uap->arg), reqperms, 1);
	if (error != 0)
		return (error);

	return (0);
}

static inline int
CHERIABI_SYS_cheriabi_procctl_fill_uap(struct thread *td,
    struct cheriabi_procctl_args *uap)
{
	struct chericap tmpcap;
	int error;
	register_t reqperms;
	size_t reqsize;

	/* [0] int idtype */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi_procctl, 0);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->idtype, CHERI_CR_CTEMP0);

	/* [1] id_t id */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi_procctl, 1);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->id, CHERI_CR_CTEMP0);

	/* [2] int com */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi_procctl, 2);
	CHERI_CLC(CHERI_CR_CTEMP0, CHERI_CR_KDC, &tmpcap, 0);
	CHERI_CTOINT(uap->com, CHERI_CR_CTEMP0);

	/* [3] void * data */
	/* May be _In_, _Inout_, _Out_, or NULL based on com */
	cheriabi_fetch_syscall_arg(td, &tmpcap,
	    CHERIABI_SYS_cheriabi_procctl, 3);
	reqsize = SIZE_MAX;	/* Catch unhandled, non-NULL commands */
	reqperms = (CHERI_PERM_GLOBAL);
	switch(uap->com) {
	case PROC_REAP_STATUS:
		reqsize = sizeof(struct procctl_reaper_status);
		reqperms |= CHERI_PERM_STORE;
		break;
	case PROC_REAP_GETPIDS:
		reqsize = sizeof(struct procctl_reaper_pids_c);
		reqperms |= CHERI_PERM_LOAD|CHERI_PERM_LOAD_CAP;
		break;
	case PROC_REAP_KILL:
		reqsize = sizeof(struct procctl_reaper_kill);
		reqperms |= CHERI_PERM_LOAD|CHERI_PERM_STORE;
		break;
	case PROC_SPROTECT:
	case PROC_TRACE_CTL:
		reqsize = sizeof(int);
		reqperms |= CHERI_PERM_LOAD;
		break;
	case PROC_TRACE_STATUS:
		reqsize = sizeof(int);
		reqperms |= CHERI_PERM_STORE;
		break;
	/* PROC_REAP_RELEASE: no data */
	}
	error = cheriabi_cap_to_ptr((caddr_t *)&uap->data, &tmpcap, reqsize,
	    reqperms, 1);
	if (error != 0)
		return (error);

	return (0);
}
