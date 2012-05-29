//#ifdef USE_BERKELEYDB
#include <string.h>
#include <db.h>
#include "db_ops.h"


static DB *dbp = NULL; //global BerkeleyDB handle

struct block_data
{
	int share_num;
	off_t offset;
};

struct db_data
{
	DBT key;
	DBT val;
};

int DB_init()
{
	int ret = 0;
	u_int32_t flags;
	ret = db_create(&dbp, NULL, 0);
	if (ret)
		return ret;
	flags = DB_CREATE;	
	ret = dbp->open(dbp, NULL, "/var/cas/cas.db", NULL, DB_BTREE, flags, 0);
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
	dd.key.size = 16;
	dd.val.data = pbd;
	dd.val.size = sizeof(struct block_data);
	return dbp->put(dbp, NULL, &dd.key, &dd.val, DB_OVERWRITE);
}

int db_get(void *hash, struct block_data *pbd)
{
	struct db_data dd;
	init_DBT(&dd);
	dd.key.data = hash;
	dd.key.size = 16;
	dd.val.data = pbd;
	dd.val.size = sizeof(struct block_data); 

	return dbp->get(dbp, NULL, &dd.key, &dd.val, 0);
}

int db_del(void *hash)
{
	DBT key;
	key.data = hash;
	key.size = 16;
	return dbp->del(dbp, NULL, &key, 0);
}
//#endif // USE_BERKELEYDB
