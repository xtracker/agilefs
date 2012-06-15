set (HASH_FILE_LIST
	md5.h md5.c
	sha1.h sha1.c)

set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)

add_library (hash ${HASH_FILE_LIST})
