//#ifdef USE_BERKELEYDB
#include <string.h>
#include <db.h>
#include "agilefs-def.h"
#include "db_ops.h"

static DB *dbp = NULL; //global BerkeleyDB handle

struct db_data
{
	DBT key;
	DBT val;
};

int DB_init(const char *db_path)
{
	int ret = 0;
	u_int32_t flags;
	ret = db_create(&dbp, NULL, 0);
	if (ret)
		return ret;
//	dbp->set_cachesize(dbp, (1 << 20));
	flags = DB_CREATE; // | DB_THREAD;
	ret = dbp->open(dbp, NULL, db_path, NULL, DB_BTREE, flags, 0664);
	if (ret)
		dbp->err(dbp, ret, "Database open failed: %s\n",db_path);
	return ret;
}

int DB_clean()
{
	if (dbp)
		return dbp->close(dbp, 0);
	else
		return 0;
}

inline void init_DBT(struct db_data * pdd)
{
	memset(pdd, 0, sizeof(struct db_data));
}

int db_put(void *hash, struct block_data *pbd)
{
	struct db_data dd;
	init_DBT(&dd);
	dd.key.data = hash;
	dd.key.size = HASH_SIZE;
	dd.val.data = pbd;
	dd.val.size = sizeof(struct block_data);
	return dbp->put(dbp, NULL, &dd.key, &dd.val, 0);
}

int db_get(void *hash, struct block_data *pbd)
{
	struct db_data dd;
	init_DBT(&dd);
//	dd.key.data = hash;
//	dd.key.ulen = 16;//sizeof(float);
//	dd.key.flags = DB_DBT_USERMEM;

	dd.key.data = hash;
	dd.key.size = HASH_SIZE;
	dd.val.data = pbd;
	dd.val.ulen = sizeof(struct block_data);
	dd.val.flags = DB_DBT_USERMEM;
	return dbp->get(dbp, NULL, &dd.key, &dd.val, 0);
}

int db_del(void *hash)
{
	DBT key;
	key.data = hash;
	key.size = 16;
	return dbp->del(dbp, NULL, &key, 0);
}

void db_err_log(int ret, const char *err_msg)
{
	dbp->err(dbp, ret, err_msg);
}
//#endif // USE_BERKELEYDB
