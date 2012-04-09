/**
 * src/server/chunks.h
 * 
 * Author: Jarvis
 * 
 * Date:2012-04-06
 * 
 * chunk server implements
 *
 */
#ifndef _CHUNKS_H_
#define _CHUNKS_H_

ssize_t writechunk(off_t offset, char *buf, size_t size);

#endif
