/**
 *
 *
 */
//#ifdef USE_BERKELEYDB 
#ifndef _DB_OPS_H_
#define _DB_OPS_H_

struct block_data {
	int fd_index;
	int share_num;
	off_t offset;
};

int DB_init(const char *db_path);
int DB_clean();

//inline void init_DBT(struct db_data *pdd);

int db_put(void *hash, struct block_data *pmd);

int db_get(void *hash, struct block_data *pbd);

int db_del(void *hash);

void db_err_log(int ret, const char *err_msg);
#endif
//#endif // USE_BERKELEYDB 
