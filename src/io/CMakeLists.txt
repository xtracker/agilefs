set (THREAD_IO_SRC_LIST
	buffer-queue.h
	thread-mgr.h thread-mgr.c)

add_library(thread-io STATIC ${THREAD_IO_SRC_LIST})

set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)

add_subdirectory (net)
