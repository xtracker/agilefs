/*
 *  Written by Apache-GT @ 25.Aug.2011
 *  gongtao0607@gmail.com
 */

#include <string.h>
#include <limits.h>
#include <stdio.h>

#define g_block_size 4096
#define hash_size 16

#define CACHE_LINES (1<<10)
#define CACHE_WAYS  8
#define min(a,b) ((a)>(b)?(b):(a))

#ifdef WITH_CACHE
static struct{
    unsigned char  hash_arr [CACHE_LINES][CACHE_WAYS][hash_size];
    unsigned char  block_arr[CACHE_LINES][CACHE_WAYS][g_block_size/sizeof(unsigned char)];
    unsigned short life_arr [CACHE_LINES][CACHE_WAYS];//from USHRT_MAX to 0
//optimized for lookup
}cache;//global static, located in heap 
#endif
/**
 * @func	get a cached data specified in hash 
 * @param	pointer to hash array
 * @param	pointer to buffer to copy the cached data to
 * @param	buffer size
 * @return	copyed size or 0 if faild
 */
size_t cache_read(unsigned char*hash,void*buf,size_t size){
#ifdef WITH_CACHE
	unsigned int line=0;
	int way=-1,i;
	printf("\treading cache 0x");
	for(i=0;i<hash_size;++i){
		line=(line<<8)+hash[i];//calculate the line
		printf("%02x",hash[i]);
	}
	line&=(CACHE_LINES-1);//mod mask
	for(i=0;i<CACHE_WAYS;++i){
		if(cache.life_arr>0){
			if(memcmp(cache.hash_arr[line][i],hash,hash_size)==0){
				way=i;//find the way number
				printf("\thit %d,%d\n",line,way);
			}else{
				--cache.life_arr[line][i];
			}
		}
	}
	if(way>=0){//located
		memcpy(buf,cache.block_arr[line][way],size);
		return size;
	}else{
		printf("\n");
		return 0;
	}
#else
	return 0;
#endif
}

/**
 * @func	store a data in cache
 * @param	pointer to hash array
 * @param	pointer to data buffer to copy
 * @param	buffer size
 */
void cache_write(unsigned char*hash,void*buf,size_t size){
#ifdef WITH_CACHE
	int i,line=0;
	printf("\tsaving cache 0x");
	for(i=0;i<hash_size;++i){
		line=(line<<8)+hash[i];
		printf("%02x",hash[i]);
	}
	line&=(CACHE_LINES-1);

	int way=0;
	for(i=1;(way!=-1)&&(i<CACHE_WAYS);++i){//find line with minimized life
		if(memcmp(cache.hash_arr[line][i],hash,hash_size)==0){
			cache.life_arr[line][i]=USHRT_MAX;//refill
			printf("\trefill %d,%d\n",line,i);
			way=-1;
		}else{
			if(cache.life_arr[line][i]<cache.life_arr[line][way]){
				way=i;
			}
		}
	}
	if(way>=0){
		cache.life_arr[line][way]=USHRT_MAX;
		memcpy(cache.hash_arr[line][way],hash,hash_size);
		memcpy(cache.block_arr[line][way],buf,size);
		printf(" %d,%d\n",line,i);
	}
#else
	return;
#endif
}
