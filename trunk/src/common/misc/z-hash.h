/**
 *
 * file:	z-hash.c
 * author:	Jarvis <jarvis.xera@gmail.com>
 * date:
 * descripttion:
 *
 */

#ifndef __Z_HASH__
#define __Z_HASH__

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef __USE_SHA1
/**
 * sha1
 */
static const char zero_hash_key[] = {
    '\x1c', '\xea', '\xf7', '\x3d',
    '\xf4', '\x0e', '\x53', '\x1d',
    '\xf3', '\xbf', '\xb2', '\x6b',
    '\x4f', '\xb7', '\xcd', '\x95',
    '\xfb', '\x7b', '\xff', '\x1d' };
#else
/**
 * md5
 */
static const char zero_hash_key[] = {
    '\x62', '\x0f', '\x0b', '\x67',
    '\xa9', '\x1f', '\x7f', '\x74',
    '\x15', '\x1b', '\xc5', '\xbe',
    '\x74', '\x5b', '\x71', '\x10' };
#endif

#ifdef __USE_SHA1
static inline int __zero_hash(void *key)
{
	return *((long *)key) == *((long *)zero_hash_key)
		&& *(((long *)key) + 1) == *(((long *)zero_hash_key) + 1)
		&& *(((int *)key) + 4) == *(((int *)zero_hash_keykey) + 4);
}
#else
static inline int __zero_hash(void *key)
{
    return *((long *)key) == *((long *)zero_hash_key)
        && *(((long *)key) + 1) == *(((long *)zero_hash_key) + 1);
}

static inline int is_zero(void *key)
{
	return *((long *)key) == (long)0
		|| *(((long *)key) + 1) == (long)0
		|| is_zero(key);
}
#endif

#if defined (__cplusplus)
}
#endif

#endif /* z-hash */
