/**
 *
 *
 */
#ifndef _DB_OPS_H_
#define _DB_OPS_H_

#ifdef __cplusplus
extern "C" {
#endif

struct block_data {
	int fd_index;
	int ref_count;
	off_t offset;
};

int DB_init(const char *db_path);
int DB_clean();

int db_put(void *hash, struct block_data *pmd);

int db_get(void *hash, struct block_data *pbd);

int db_del(void *hash);

void db_err_log(int ret, const char *err_msg);
#ifdef __cplusplus
}
#endif	/** __cplusplus **/
#endif
