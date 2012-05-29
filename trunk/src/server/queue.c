#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct requst_queue {
	int head;
	int tail;
	int size;
	void *base;
};
