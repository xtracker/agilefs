/**
 * filename
 * Author:	Zhao Guoyu
 * Date:	2012
 *
 */

#include <string.h>
#include <db.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "agilefs-def.h"
#include "db_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

 
/** global db environment handle **/
static DB_ENV *g_db_env = NULL;	
/** global db handles array **/
static DB *dbp[NUM_SERVER_DBS];

struct db_data {
	DBT key;
	DBT val;
};

int DB_init(const char *db_env_path)
{
	int ret = 0, i;
	u_int32_t db_flags, env_flags;
	char db_path[256];

	ret = db_env_create(&g_db_env, 0);
	if (ret != 0) {
		fprintf(stderr, "Error : creating db env: %s\n", db_strerror(ret));
		return -1;
	}
	env_flags = DB_CREATE | DB_INIT_MPOOL;

	ret = g_db_env->open(g_db_env, db_env_path, env_flags, 0);
	if (ret != 0) {
		fprintf(stderr, "Error : opening db env: %s\n", db_strerror(ret));
		return (-1);
	}
	for (i = 0; i < NUM_SERVER_DBS; ++i) {
		ret = db_create(&dbp[i], g_db_env, 0);
		if (ret) {
			dbp[i]->err(dbp[i], ret,
					"Database create failed: %s\n",
					dbp[i]);
			goto db_init_err;
		}
		sprintf(db_path, "%010d.db", i);
		db_flags = DB_CREATE; // | DB_THREAD;
		ret = dbp[i]->open(dbp[i], NULL, db_path, NULL, DB_BTREE, db_flags, 0664);
		if (ret) {
			dbp[i]->err(dbp[i], ret,
					"Database open failed: %s\n",
					db_env_path);
			goto db_init_err;
		}
	}
	return 0;

db_init_err:
	/**
	for (j = 0; j < i; ++i)
		dbp[i]->close(dbp[i], 0);
	g_db_env->close(g_db_env, 0);
	*/
	DB_clean(i);
	return (-1);
}

int DB_clean(int cnt)
{
	int i;
	if (cnt > NUM_SERVER_DBS)
		cnt = NUM_SERVER_DBS;
	for (i = 0; i < cnt; ++i) {
		if (dbp[i])
			dbp[i]->close(dbp[i], 0);
	}
	if (g_db_env)
		g_db_env->close(g_db_env, 0);
	return 0;
}

inline void init_DBT(struct db_data * pdd)
{
	memset(pdd, 0, sizeof(struct db_data));
}

int db_put(void *hash, struct block_data *pbd)
{
	struct db_data dd;
	int i = hash_to_index(hash);
	init_DBT(&dd);
	dd.key.data = hash;
	dd.key.size = HASH_SIZE;
	dd.val.data = pbd;
	dd.val.size = sizeof(struct block_data);
	return dbp[i]->put(dbp[i], NULL, &dd.key, &dd.val, 0);
}

int db_get(void *hash, struct block_data *pbd)
{
	struct db_data dd;
	int i = hash_to_index(hash);
	init_DBT(&dd);
//	dd.key.data = hash;
//	dd.key.ulen = 16;//sizeof(float);
//	dd.key.flags = DB_DBT_USERMEM;

	dd.key.data = hash;
	dd.key.size = HASH_SIZE;
	dd.val.data = pbd;
	dd.val.ulen = sizeof(struct block_data);
	dd.val.flags = DB_DBT_USERMEM;
	return dbp[i]->get(dbp[i], NULL, &dd.key, &dd.val, 0);
}

int db_del(void *hash)
{
	int i = hash_to_index(hash);
	DBT key;
	key.data = hash;
	key.size = HASH_SIZE;
	return dbp[i]->del(dbp[i], NULL, &key, 0);
}

void db_err_log(int ret, const char *err_msg)
{
	//dbp->err(dbp, ret, err_msg);
}

#ifdef __cplusplus
}
#endif /** __cplusplus **/
