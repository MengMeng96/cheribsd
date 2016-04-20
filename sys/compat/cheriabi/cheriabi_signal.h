
#ifndef _COMPAT_CHERIABI_CHERIABI_SIGNAL_H_
#define _COMPAT_CHERIABI_CHERIABI_SIGNAL_H_

struct sigaltstack_c {
	struct chericap	ss_sp;		/* signal stack base */
	size_t		ss_size;	/* signal stack length */
	int		ss_flags;	/* SS_DISABLE and/or SS_ONSTACK */
};

union sigval_c {
	int			sival_int;
	struct chericap		sival_ptr;
	/* XXX: no 6.0 compatibility (sigval_*) */
};

struct siginfo_c {
	int		si_signo;
	int		si_errno;
	int		si_code;
	__pid_t		si_pid;
	__uid_t		si_uid;
	int		si_status;
	struct chericap	si_addr;	/* faulting instruction */
	union sigval_c	si_value;
	union   {
		struct {
			int     _trapno;	/* machine specific trap code */
		} _fault;
		struct {
			int     _timerid;
			int     _overrun;
		} _timer;
		struct {
			int     _mqd;
		} _mesgq;
		struct {
			long    _band;		/* band event for SIGPOLL */
		} _poll;			/* was this ever used ? */
		struct {
			long    __spare1__;
			int     __spare2__[7];  
		} __spare__;
	} _reason;
};

struct sigevent_c {
	int	sigev_notify;
	int	sigev_signo;
	union sigval_c sigev_value;
	union {
		__lwpid_t	_threadid;
		struct {
			void (*_function)(union sigval);
			struct chericap	*_attribute;
		} _sigev_thread;
		unsigned short _kevent_flags;
		long __spare__[8];
	} _sigev_un;
};

typedef struct {
	struct chericap ss_sp;
	size_t		ss_size;
	int		ss_flag;
} cheriabi_stack_t;

struct ucontext_c {
	/*
	 * Keep the order of the first two fields. Also,
	 * keep them the first two fields in the structure.
	 * This way we can have a union with struct
	 * sigcontext and ucontext_t. This allows us to
	 * support them both at the same time.
	 * note: the union is not defined, though.
	 */
	__sigset_t		uc_sigmask;
	mcontext_t		uc_mcontext;

	struct chericap		uc_link;	/* struct ucontext_c * */
	cheriabi_stack_t	uc_stack;
	int			uc_flags;
	int			__spare__[4];
};

struct sigevent;
int convert_sigevent_c(struct sigevent_c *sig_c, struct sigevent *sig);
void siginfo_to_siginfo_c(const siginfo_t *src, struct siginfo_c *dst);

#endif /* _COMPAT_CHERIABI_CHERIABI_SIGNAL_H_ */
