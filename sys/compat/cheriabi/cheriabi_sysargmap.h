/*
 * System call argument map.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 */

#ifndef _CHERIABI_SYSARGMAP_H_
#define	_CHERIABI_SYSARGMAP_H_

#define	CHERIABI_SYS_cheriabi_syscall_PTRMASK	(0x0)
#define	CHERIABI_SYS_exit_PTRMASK	(0x0)
#define	CHERIABI_SYS_read_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_write_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_open_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_close_PTRMASK	(0x0)
#define	CHERIABI_SYS_wait4_PTRMASK	(0x0 | 0x2 | 0x8)
#define	CHERIABI_SYS_cheriabi_link_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_unlink_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_chdir_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_fchdir_PTRMASK	(0x0)
#define	CHERIABI_SYS_chmod_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_chown_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_mount_PTRMASK	(0x0 | 0x1 | 0x2 | 0x8)
#define	CHERIABI_SYS_unmount_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_setuid_PTRMASK	(0x0)
#define	CHERIABI_SYS_ptrace_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_cheriabi_recvmsg_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_sendmsg_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_recvfrom_PTRMASK	(0x0 | 0x2 | 0x10 | 0x20)
#define	CHERIABI_SYS_accept_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_getpeername_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_getsockname_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_access_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_chflags_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_fchflags_PTRMASK	(0x0)
#define	CHERIABI_SYS_kill_PTRMASK	(0x0)
#define	CHERIABI_SYS_dup_PTRMASK	(0x0)
#define	CHERIABI_SYS_profil_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_ktrace_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_getlogin_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_setlogin_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_acct_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_sigaltstack_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_ioctl_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_reboot_PTRMASK	(0x0)
#define	CHERIABI_SYS_revoke_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_symlink_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_readlink_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_execve_PTRMASK	(0x0 | 0x1 | 0x2 | 0x4)
#define	CHERIABI_SYS_umask_PTRMASK	(0x0)
#define	CHERIABI_SYS_chroot_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_msync_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_munmap_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_mprotect_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_madvise_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_mincore_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_getgroups_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_setgroups_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_setpgid_PTRMASK	(0x0)
#define	CHERIABI_SYS_setitimer_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_swapon_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_getitimer_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_dup2_PTRMASK	(0x0)
#define	CHERIABI_SYS_fcntl_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_select_PTRMASK	(0x0 | 0x2 | 0x4 | 0x8 | 0x10)
#define	CHERIABI_SYS_fsync_PTRMASK	(0x0)
#define	CHERIABI_SYS_setpriority_PTRMASK	(0x0)
#define	CHERIABI_SYS_socket_PTRMASK	(0x0)
#define	CHERIABI_SYS_connect_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_getpriority_PTRMASK	(0x0)
#define	CHERIABI_SYS_bind_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_setsockopt_PTRMASK	(0x0 | 0x8)
#define	CHERIABI_SYS_listen_PTRMASK	(0x0)
#define	CHERIABI_SYS_gettimeofday_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_getrusage_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_getsockopt_PTRMASK	(0x0 | 0x8 | 0x10)
#define	CHERIABI_SYS_cheriabi_readv_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_writev_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_settimeofday_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_fchown_PTRMASK	(0x0)
#define	CHERIABI_SYS_fchmod_PTRMASK	(0x0)
#define	CHERIABI_SYS_setreuid_PTRMASK	(0x0)
#define	CHERIABI_SYS_setregid_PTRMASK	(0x0)
#define	CHERIABI_SYS_rename_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_flock_PTRMASK	(0x0)
#define	CHERIABI_SYS_mkfifo_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_sendto_PTRMASK	(0x0 | 0x2 | 0x10)
#define	CHERIABI_SYS_shutdown_PTRMASK	(0x0)
#define	CHERIABI_SYS_socketpair_PTRMASK	(0x0 | 0x8)
#define	CHERIABI_SYS_mkdir_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_rmdir_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_utimes_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_adjtime_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_quotactl_PTRMASK	(0x0 | 0x1 | 0x8)
#define	CHERIABI_SYS_cheriabi_nlm_syscall_PTRMASK	(0x0 | 0x8)
#define	CHERIABI_SYS_cheriabi_nfssvc_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_lgetfh_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_getfh_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_sysarch_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_rtprio_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_setfib_PTRMASK	(0x0)
#define	CHERIABI_SYS_ntp_adjtime_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_setgid_PTRMASK	(0x0)
#define	CHERIABI_SYS_setegid_PTRMASK	(0x0)
#define	CHERIABI_SYS_seteuid_PTRMASK	(0x0)
#define	CHERIABI_SYS_pathconf_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_fpathconf_PTRMASK	(0x0)
#define	CHERIABI_SYS_getrlimit_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_setrlimit_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS___sysctl_PTRMASK	(0x0 | 0x1 | 0x4 | 0x8 | 0x10)
#define	CHERIABI_SYS_mlock_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_munlock_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_undelete_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_futimes_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_getpgid_PTRMASK	(0x0)
#define	CHERIABI_SYS_poll_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_semget_PTRMASK	(0x0)
#define	CHERIABI_SYS_semop_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_msgget_PTRMASK	(0x0)
#define	CHERIABI_SYS_msgsnd_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_msgrcv_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_shmat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_shmdt_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_shmget_PTRMASK	(0x0)
#define	CHERIABI_SYS_clock_gettime_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_clock_settime_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_clock_getres_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_ktimer_create_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_ktimer_delete_PTRMASK	(0x0)
#define	CHERIABI_SYS_ktimer_settime_PTRMASK	(0x0 | 0x4 | 0x8)
#define	CHERIABI_SYS_ktimer_gettime_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_ktimer_getoverrun_PTRMASK	(0x0)
#define	CHERIABI_SYS_nanosleep_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_ffclock_getcounter_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_ffclock_setestimate_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_ffclock_getestimate_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_clock_nanosleep_PTRMASK	(0x0 | 0x4 | 0x8)
#define	CHERIABI_SYS_clock_getcpuclockid2_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_ntp_gettime_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_minherit_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_rfork_PTRMASK	(0x0)
#define	CHERIABI_SYS_lchown_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_aio_read_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_aio_write_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_lio_listio_PTRMASK	(0x0 | 0x2 | 0x8)
#define	CHERIABI_SYS_lchmod_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_lutimes_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_preadv_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_pwritev_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_fhopen_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_modnext_PTRMASK	(0x0)
#define	CHERIABI_SYS_modstat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_modfnext_PTRMASK	(0x0)
#define	CHERIABI_SYS_modfind_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_kldload_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_kldunload_PTRMASK	(0x0)
#define	CHERIABI_SYS_kldfind_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_kldnext_PTRMASK	(0x0)
#define	CHERIABI_SYS_cheriabi_kldstat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_kldfirstmod_PTRMASK	(0x0)
#define	CHERIABI_SYS_getsid_PTRMASK	(0x0)
#define	CHERIABI_SYS_setresuid_PTRMASK	(0x0)
#define	CHERIABI_SYS_setresgid_PTRMASK	(0x0)
#define	CHERIABI_SYS_cheriabi_aio_return_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_aio_suspend_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_cheriabi_aio_cancel_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_aio_error_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_mlockall_PTRMASK	(0x0)
#define	CHERIABI_SYS___getcwd_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_sched_setparam_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_sched_getparam_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_sched_setscheduler_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_sched_getscheduler_PTRMASK	(0x0)
#define	CHERIABI_SYS_sched_get_priority_max_PTRMASK	(0x0)
#define	CHERIABI_SYS_sched_get_priority_min_PTRMASK	(0x0)
#define	CHERIABI_SYS_sched_rr_get_interval_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_utrace_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_kldsym_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_cheriabi_jail_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_sigprocmask_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_sigsuspend_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_sigpending_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_sigtimedwait_PTRMASK	(0x0 | 0x1 | 0x2 | 0x4)
#define	CHERIABI_SYS_cheriabi_sigwaitinfo_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS___acl_get_file_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS___acl_set_file_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS___acl_get_fd_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS___acl_set_fd_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS___acl_delete_file_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS___acl_delete_fd_PTRMASK	(0x0)
#define	CHERIABI_SYS___acl_aclcheck_file_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS___acl_aclcheck_fd_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_extattrctl_PTRMASK	(0x0 | 0x1 | 0x4 | 0x10)
#define	CHERIABI_SYS_extattr_set_file_PTRMASK	(0x0 | 0x1 | 0x4 | 0x8)
#define	CHERIABI_SYS_extattr_get_file_PTRMASK	(0x0 | 0x1 | 0x4 | 0x8)
#define	CHERIABI_SYS_extattr_delete_file_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_cheriabi_aio_waitcomplete_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_getresuid_PTRMASK	(0x0 | 0x1 | 0x2 | 0x4)
#define	CHERIABI_SYS_getresgid_PTRMASK	(0x0 | 0x1 | 0x2 | 0x4)
#define	CHERIABI_SYS_extattr_set_fd_PTRMASK	(0x0 | 0x4 | 0x8)
#define	CHERIABI_SYS_extattr_get_fd_PTRMASK	(0x0 | 0x4 | 0x8)
#define	CHERIABI_SYS_extattr_delete_fd_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS___setugid_PTRMASK	(0x0)
#define	CHERIABI_SYS_eaccess_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_nmount_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi___mac_get_proc_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi___mac_set_proc_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi___mac_get_fd_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi___mac_get_file_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi___mac_set_fd_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi___mac_set_file_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_kenv_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_lchflags_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_uuidgen_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_sendfile_PTRMASK	(0x0 | 0x10 | 0x20)
#define	CHERIABI_SYS_cheriabi_mac_syscall_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_cheriabi___mac_get_pid_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi___mac_get_link_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi___mac_set_link_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_extattr_set_link_PTRMASK	(0x0 | 0x1 | 0x4 | 0x8)
#define	CHERIABI_SYS_extattr_get_link_PTRMASK	(0x0 | 0x1 | 0x4 | 0x8)
#define	CHERIABI_SYS_extattr_delete_link_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_cheriabi___mac_execve_PTRMASK	(0x0 | 0x1 | 0x2 | 0x4 | 0x8)
#define	CHERIABI_SYS_cheriabi_sigaction_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_cheriabi_sigreturn_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_getcontext_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_setcontext_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_swapcontext_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_swapoff_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS___acl_get_link_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS___acl_set_link_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS___acl_delete_link_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS___acl_aclcheck_link_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_sigwait_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_cheriabi_thr_create_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_thr_exit_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_thr_self_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_thr_kill_PTRMASK	(0x0)
#define	CHERIABI_SYS_jail_attach_PTRMASK	(0x0)
#define	CHERIABI_SYS_extattr_list_fd_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_extattr_list_file_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_extattr_list_link_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_ksem_timedwait_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_thr_suspend_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_thr_wake_PTRMASK	(0x0)
#define	CHERIABI_SYS_kldunloadf_PTRMASK	(0x0)
#define	CHERIABI_SYS_audit_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_auditon_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_getauid_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_setauid_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_getaudit_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_setaudit_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_getaudit_addr_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_setaudit_addr_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_auditctl_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS__umtx_op_PTRMASK	(0x0 | 0x1 | 0x8 | 0x10)
#define	CHERIABI_SYS_cheriabi_thr_new_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_sigqueue_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_kmq_open_PTRMASK	(0x0 | 0x1 | 0x8)
#define	CHERIABI_SYS_kmq_setattr_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_kmq_timedreceive_PTRMASK	(0x0 | 0x2 | 0x8 | 0x10)
#define	CHERIABI_SYS_kmq_timedsend_PTRMASK	(0x0 | 0x2 | 0x10)
#define	CHERIABI_SYS_cheriabi_kmq_notify_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_kmq_unlink_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_abort2_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_thr_set_name_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_aio_fsync_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_rtprio_thread_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_sctp_peeloff_PTRMASK	(0x0)
#define	CHERIABI_SYS_sctp_generic_sendmsg_PTRMASK	(0x0 | 0x2 | 0x8 | 0x20)
#define	CHERIABI_SYS_cheriabi_sctp_generic_sendmsg_iov_PTRMASK	(0x0 | 0x2 | 0x8 | 0x20)
#define	CHERIABI_SYS_cheriabi_sctp_generic_recvmsg_PTRMASK	(0x0 | 0x2 | 0x8 | 0x10 | 0x20 | 0x40)
#define	CHERIABI_SYS_pread_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_pwrite_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_mmap_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_lseek_PTRMASK	(0x0)
#define	CHERIABI_SYS_truncate_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_ftruncate_PTRMASK	(0x0)
#define	CHERIABI_SYS_thr_kill2_PTRMASK	(0x0)
#define	CHERIABI_SYS_shm_open_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_shm_unlink_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cpuset_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cpuset_setid_PTRMASK	(0x0)
#define	CHERIABI_SYS_cpuset_getid_PTRMASK	(0x0 | 0x8)
#define	CHERIABI_SYS_cpuset_getaffinity_PTRMASK	(0x0 | 0x10)
#define	CHERIABI_SYS_cpuset_setaffinity_PTRMASK	(0x0 | 0x10)
#define	CHERIABI_SYS_faccessat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_fchmodat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_fchownat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_fexecve_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_futimesat_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_cheriabi_linkat_PTRMASK	(0x0 | 0x2 | 0x8)
#define	CHERIABI_SYS_mkdirat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_mkfifoat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_openat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_readlinkat_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_renameat_PTRMASK	(0x0 | 0x2 | 0x8)
#define	CHERIABI_SYS_symlinkat_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_cheriabi_unlinkat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_posix_openpt_PTRMASK	(0x0)
#define	CHERIABI_SYS_gssd_syscall_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_jail_get_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_jail_set_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_jail_remove_PTRMASK	(0x0)
#define	CHERIABI_SYS_closefrom_PTRMASK	(0x0)
#define	CHERIABI_SYS_cheriabi___semctl_PTRMASK	(0x0 | 0x8)
#define	CHERIABI_SYS_cheriabi_msgctl_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_shmctl_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_lpathconf_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS___cap_rights_get_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_cap_getmode_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_pdfork_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_pdkill_PTRMASK	(0x0)
#define	CHERIABI_SYS_pdgetpid_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_pselect_PTRMASK	(0x0 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20)
#define	CHERIABI_SYS_getloginclass_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_setloginclass_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_rctl_get_racct_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_rctl_get_rules_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_rctl_get_limits_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_rctl_add_rule_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_rctl_remove_rule_PTRMASK	(0x0 | 0x1 | 0x4)
#define	CHERIABI_SYS_posix_fallocate_PTRMASK	(0x0)
#define	CHERIABI_SYS_posix_fadvise_PTRMASK	(0x0)
#define	CHERIABI_SYS_cheriabi_wait6_PTRMASK	(0x0 | 0x4 | 0x10 | 0x20)
#define	CHERIABI_SYS_cap_rights_limit_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cap_ioctls_limit_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cap_ioctls_get_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cap_fcntls_limit_PTRMASK	(0x0)
#define	CHERIABI_SYS_cap_fcntls_get_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_bindat_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_connectat_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_chflagsat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_accept4_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_pipe2_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_aio_mlock_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_cheriabi_procctl_PTRMASK	(0x0 | 0x8)
#define	CHERIABI_SYS_ppoll_PTRMASK	(0x0 | 0x1 | 0x4 | 0x8)
#define	CHERIABI_SYS_futimens_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_utimensat_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_numa_getaffinity_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_numa_setaffinity_PTRMASK	(0x0 | 0x4)
#define	CHERIABI_SYS_fdatasync_PTRMASK	(0x0)
#define	CHERIABI_SYS_fstat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_fstatat_PTRMASK	(0x0 | 0x2 | 0x4)
#define	CHERIABI_SYS_fhstat_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_getdirentries_PTRMASK	(0x0 | 0x2 | 0x8)
#define	CHERIABI_SYS_statfs_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_fstatfs_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_getfsstat_PTRMASK	(0x0 | 0x1)
#define	CHERIABI_SYS_fhstatfs_PTRMASK	(0x0 | 0x1 | 0x2)
#define	CHERIABI_SYS_mknodat_PTRMASK	(0x0 | 0x2)
#define	CHERIABI_SYS_cheriabi_kevent_PTRMASK	(0x0 | 0x2 | 0x8 | 0x20)
#endif /* !_CHERIABI_SYSARGMAP_H_ */
