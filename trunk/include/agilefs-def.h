#ifndef _AGILEFS_DEF_H_
#define _AGILEFS_DEF_H_

#define FSP_SIZE 4096
#define FSP_OFFSET 12

#ifdef __USE_SHA1__
#define HASH_SIZE 20
#else
#define HASH_SIZE 16
#endif

#define MAX_CHUNK_FILE_SIZE (1L << 32)

#ifdef __x86_64__
typedef long loff_t;
#else
typedef long long loff_t;
#endif

#endif
