/**
 *
 *
 *
 */
#ifndef __EXPAT_XML_H_
#define __EXPAT_XML_H_

struct chunk_file_info_xml {
	struct chunk_file_info *pcfi;
	int i, depth;
	void XMLCALL (*start)(void *, const char *, const char **);
};

int init_chunk_file(struct chunk_file_info *base, const char *path);
int release_chunk_file(struct chunk_file_info *base, const char *path);

#endif
