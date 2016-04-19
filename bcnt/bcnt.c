//
//  main.c
//  ByteCounter
//
//  Created by Galen Rhodes on 7/18/14.
//  Copyright (c) 2014 Galen Rhodes. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
typedef double   bcFloat;
typedef uint64_t bcUInt;
typedef uint8_t  *pByte;
typedef char     *pChar;

#define dGB ((bcUInt)1073741824)
#define dMB ((bcUInt)1048576)
#define dKB ((bcUInt)1024)
#define dBt ((bcUInt)1)

#define dStringBufferSize (128)
#define dByteBufferSize   (dKB * 512)

#define P1(v, s)    (prt(((v) / ((bcFloat)(d##s))), #s))
#define P2(v, s, m) ((v >= d##s) ? P1(v, s) : m)
#define ATOI(s)     ((int)(abs(atoi(((pChar)(s))))))
#define Z(a, b, c)  ((a)?ATOI(b):(c))
#define CF          ("-c")
#define CFL         ((size_t)(strlen((CF))))

#ifndef NS_INLINE
	#define NS_INLINE static __inline__ __attribute__((__always_inline__))
#endif

pByte byteBuffer;
pChar stringBuffer;

NS_INLINE char *prt(bcFloat c, pChar const u) {
	sprintf(stringBuffer, "\e[33m%6.1f\e[36m%s", c, u);
	return stringBuffer;
}

NS_INLINE int interpretParameter(pChar const args1, pChar const args2) {
	return ((strcmp(CF, args1) == 0) ? Z(args2, args2, 1) : Z(strncmp(CF, args1, CFL) == 0, args1 + 2, 0));
}

NS_INLINE int findColumnParameter(int argc, pChar const argv[]) {
	int argc1  = (argc - 1);
	int column = 0;

	for(int argi = 0; ((column == 0) && (argi < argc)); argi++) {
		column = interpretParameter(argv[argi], ((argi < argc1) ? argv[argi + 1] : NULL));
	}

	return column;
}

NS_INLINE int _write(pByte bytes, ssize_t writeCount) {
	ssize_t writeResult = write(STDOUT_FILENO, bytes, (size_t)writeCount);

	while(writeResult >= 0 && writeResult < writeCount) {
		if(writeResult) {
			bytes += writeResult;
			writeCount -= writeResult;
		}

		writeResult = write(STDOUT_FILENO, bytes, (size_t)writeCount);
	}

	return (writeResult < 0 ? -1 : 0);
}

int main(int argc, pChar const argv[]) {
	byteBuffer   = (pByte)malloc((size_t)dByteBufferSize);
	stringBuffer = (pChar)malloc((size_t)dStringBufferSize);

	int     tb = ((findColumnParameter(argc, argv) * 9) + 2);
	bcUInt  tc = 0;
	ssize_t cc = read(STDIN_FILENO, byteBuffer, dByteBufferSize);

	while(cc > 0) {
		if(_write(byteBuffer, cc)) {
			cc = -1;
			break;
		}

		tc += cc;
		fprintf(stderr, "\e7\e[%dG%s\e8", tb, P2(tc, GB, P2(tc, MB, P2(tc, KB, P1(tc, Bt)))));
		cc = read(STDIN_FILENO, byteBuffer, dByteBufferSize);
	}

	int e = ((cc < 0) ? 0 : errno);

	fputs("\n", stderr);
	fsync(STDOUT_FILENO);
	free(byteBuffer);
	free(stringBuffer);
	return e;
}

#pragma clang diagnostic pop
