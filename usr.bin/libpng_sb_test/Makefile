# $FreeBSD$

PROG=	libpng_sb_test

LDFLAGS+=	-Wl,-z,notext

MAN=

WARNS=	6

LIBADD=	png_sb cheri
CFLAGS+=        -I${SRCTOP}/contrib/libpng
CFLAGS+=        -I${SRCTOP}/lib/libpng

.include <bsd.prog.mk>
