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

#define dGB ((Word64)1073741824)
#define dMB ((Word64)1048576)
#define dKB ((Word64)1024)
#define dBt ((Word64)1)

#define dStringBufferSize (128)
#define dByteBufferSize   (dKB * 512)

#define P1(v, s)    (prt(((v) / ((Double)(d##s))), #s))
#define P2(v, s, m) ((v >= d##s) ? P1(v, s) : m)
#define ATOI(s)     ((int)(abs(atoi(((pChar)(s))))))
#define Z(a, b, c)  ((a)?ATOI(b):(c))
#define CF          ("-c")
#define CFL         ((size_t)(strlen((CF))))

#ifndef GR_INLINE
	#define GR_INLINE static __inline__ __attribute__((__always_inline__))
#endif

typedef double   Double;
typedef int      Word;
typedef uint64_t Word64;
typedef uint8_t  *pByte;
typedef char     *pChar;

pByte byteBuffer;
pChar stringBuffer;

GR_INLINE pChar prt(Double c, pChar const u) {
	sprintf(stringBuffer, "\e[33m%6.1f\e[36m%s", c, u);
	return stringBuffer;
}

GR_INLINE Word interpretParameter(pChar const args1, pChar const args2) {
	return ((strcmp(CF, args1) == 0) ? Z(args2, args2, 1) : Z(strncmp(CF, args1, CFL) == 0, args1 + 2, 0));
}

GR_INLINE Word findColumnParameter(Word argc, pChar const argv[]) {
	Word argc1 = (argc - 1), column = 0;

	for(Word argi = 0; ((column == 0) && (argi < argc)); argi++) {
		column = interpretParameter(argv[argi], ((argi < argc1) ? argv[argi + 1] : NULL));
	}

	return column;
}

GR_INLINE Word writeBlock(pByte bytes, ssize_t writeCount) {
	ssize_t writeResult = write(STDOUT_FILENO, bytes, (size_t)writeCount);

	while(writeResult >= 0 && writeResult < writeCount) {
		if(writeResult) {
			bytes += writeResult;
			writeCount -= writeResult;
		}

		writeResult = write(STDOUT_FILENO, bytes, (size_t)writeCount);
	}

	return (writeResult < 0 ? errno : 0);
}

GR_INLINE ssize_t readBlock(pByte bytes, size_t maxBytes) {
	return read(STDIN_FILENO, bytes, maxBytes);
}

Word main(Word argc, pChar const argv[]) {
	byteBuffer   = (pByte)malloc((size_t)dByteBufferSize);
	stringBuffer = (pChar)malloc((size_t)dStringBufferSize);

	Word    tb = ((findColumnParameter(argc, argv) * 9) + 2);
	Word    we = 0;
	Word64  tc = 0;
	ssize_t re = readBlock(byteBuffer, dByteBufferSize);

	while(re > 0 && we == 0) {
		if((we = writeBlock(byteBuffer, re)) == 0) {
			tc += re;
			fprintf(stderr, "\e7\e[%dG%s\e8", tb, P2(tc, GB, P2(tc, MB, P2(tc, KB, P1(tc, Bt)))));
			re = readBlock(byteBuffer, dByteBufferSize);
		}
	}

	if(re < 0 && we == 0) {
		we = errno;
	}

	fputs("\n", stderr);
	fsync(STDOUT_FILENO);
	free(byteBuffer);
	free(stringBuffer);
	return we;
}

#pragma clang diagnostic pop
