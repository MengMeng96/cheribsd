# $FreeBSD$

.include <src.opts.mk>

PROG?=	cheri_minimal_dynamic_exe
MAN=
NEED_CHERI=	pure

CFLAGS+=	-I${SRCTOP}/lib/libsimple_printf -DSIMPLE_PRINTF_PREFIX=simple
LDADD+=		-lsimple_printf -nodefaultlibs

SRCS=	main.c

# For now avoid linking against libsyscalls since it crashes
USE_LIBSYSCALLS?=yes
.if ${USE_LIBSYSCALLS} == "yes"
LIBADD=		syscalls
.else
# Build _exit() from assembly sources
SRCS+=	_exit.S
.if exists(${SRCTOP}/lib/libc/${MACHINE_ARCH})
CFLAGS+=	-I${SRCTOP}/lib/libc/${MACHINE_ARCH}
.else
CFLAGS+=	-I${SRCTOP}/lib/libc/${MACHINE_CPUARCH}
.endif
.endif # ${USE_LIBSYSCALLS} == "yes"

NO_WERROR=	yes
# Always build shared
MK_CHERI_SHARED_PROG=yes

.include <bsd.prog.mk>
