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

#define ANSI_POSITION "\e7\e[%dG%s\e8"
#define ANSI_VALUES   "\e[33m%6.1f\e[36m%s"

#define dGB (Word64)1073741824
#define dMB (Word64)1048576
#define dKB (Word64)1024
#define dBt (Word64)1

#define qGB "GB"
#define qMB "MB"
#define qKB "KB"
#define qBt "B "

#define dStringBufferSize 128
#define dByteBufferSize   (dKB * 512)

#define CF                 "-c"
#define P1(v, s)           prt(v/(Double)d##s,q##s)
#define P2(v, s, m)        ((v >= d##s)?P1(v, s):m)
#define StrEql(s1, s2)     (strcmp(s1, s2) == 0)
#define StrNEq(s1, s2, ln) (strncmp(s1, s2, ln) == 0)

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
	sprintf(stringBuffer, ANSI_VALUES, c, u);
	return stringBuffer;
}

GR_INLINE Word findColumnParameter(Word argc, pChar const argv[]) {
	Word   argi = 0;
	size_t cfl  = strlen(CF);

	while(argi < argc) {
		pChar pstr = argv[argi++];

		if(StrEql(CF, pstr)) {
			return (argi < argc ? (Word)abs(atoi(argv[argi])) : 1);
		}
		else if(StrNEq(CF, pstr, cfl)) {
			return (Word)abs(atoi(pstr + cfl));
		}
	}

	return 0;
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

	Word64  tc = 0;
	Word    we = 0;
	Word    tb = ((findColumnParameter(argc, argv) * 9) + 2);
	ssize_t re = readBlock(byteBuffer, dByteBufferSize);

	while(re > 0 && we == 0) {
		if((we = writeBlock(byteBuffer, re)) == 0) {
			tc += re;
			fprintf(stderr, ANSI_POSITION, tb, P2(tc, GB, P2(tc, MB, P2(tc, KB, P1(tc, Bt)))));
			re = readBlock(byteBuffer, dByteBufferSize);
		}
	}

	if(re < 0 && we == 0) {
		we = errno;
	}

	fputs("\e[36m\e[0m", stderr);
	fsync(STDOUT_FILENO);
	free(byteBuffer);
	free(stringBuffer);
	return we;
}

#pragma clang diagnostic pop
