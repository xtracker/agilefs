#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct req_queue_t {
	int head;
	int tail;
	int size;
	void *base;
};
