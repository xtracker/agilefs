/**
 *
 *
 */
//#ifdef USE_BERKELEYDB 
#ifndef _DB_OPS_H_
#define _DB_OPS_H_
struct block_data;
struct db_data;

int DB_init();
int DB_clean();

inline void init_DBT(struct db_data *pdd);

int db_put(void *hash, struct block_data *pmd);

int db_get(void *hash, struct block_data *pbd);

int db_del(void *hash);

#endif
//#endif // USE_BERKELEYDB 
