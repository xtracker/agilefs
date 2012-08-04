/**
 *
 */

#ifndef __DIRECT_IOS_H__
#define __DIRECT_IOS_H__

int direct_write_at(int fd, char *buf, off_t offset, size_t size);
int direct_read_at(int fd, char *buf, off_t offset, size_t size);

#endif
