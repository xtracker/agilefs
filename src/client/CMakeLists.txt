#add_definitions(-D _USE_SHA1)
add_definitions (`pkg-config fuse --cflags`)


set (CLIENT_SRC_LIST agilefs-fuse.c
	fuse_io_util.h fuse_io_util.c
	fuse_cache.h fuse_cache.c)

link_directories (${PROJECT_BINARY_DIR}/libs)

add_executable (agilefs-client ${CLIENT_SRC_LIST})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

target_link_libraries (agilefs-client pthread fuse rt dl hash)
