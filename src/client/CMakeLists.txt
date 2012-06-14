#add_definitions(-D _USE_SHA1)
add_definitions(`pkg-config fuse --cflags`)


set(CLIENT_SRC_LIST agilefs-fuse.c
	sha1.h sha1.c md5.h md5.c
	fuse_io_util.h fuse_io_util.c
	fuse_cache.h fuse_cache.c)

add_executable(agilefs-client ${CLIENT_SRC_LIST})
target_link_libraries(agilefs-client pthread fuse rt dl)
