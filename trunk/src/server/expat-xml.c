/* This is simple demonstration of how to use expat. This program
	reads an XML document from standard input and writes a line with
	the name of each element to standard output indenting child
	elements by one tab stop more than their parent element.
	It must be used with Expat compiled for UTF-8 output.
*/

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "expat.h"

#if defined(__amigaos__) && defined(__USE_INLINE__)
#include <proto/expat.h>
#endif

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#include "expat-xml.h"
#include "chunks.h"


static void XMLCALL start_element_init(void *user_data, const char *name, const char **atts)
{
	struct chunk_file_info_xml *pcfix = (struct chunk_file_info_xml *)user_data;
	if (!strcmp(name, "total"))	{
		int total = atoi(atts[1]);
		pcfix->pcfi->total = total;
		pcfix->pcfi->fcls = (struct free_chunk_list *)malloc(sizeof(struct free_chunk_list) * total);
		pcfix->pcfi->fds = (int *)malloc(sizeof(int) * total);
		pcfix->pcfi->cur_size = (long *)malloc(sizeof(long) * total);
	}
	else if (!strcmp(name, "file"))	{
		struct stat statbuf;
		int ret = open(atts[1], O_RDWR | O_CREAT, 0644);
		pcfix->pcfi->fds[pcfix->i] = ret;
		ret = fstat(ret, &statbuf);
		pcfix->pcfi->cur_size[pcfix->i] = statbuf.st_size;
		init_free_chunk(pcfix->pcfi->fcls + pcfix->i, atts[3]);
		++pcfix->i;
	}
	++pcfix->depth;
}

static void XMLCALL start_element_release(void *user_data, const char *name, const char **atts)
{
	struct chunk_file_info_xml *pcfix = (struct chunk_file_info_xml *)user_data;

	if (!strcmp(name, "file")) {
		close(pcfix->pcfi->fds[pcfix->i]);
		flush_free_chunk(pcfix->pcfi->fcls + pcfix->i, atts[3]);
		++pcfix->i;
	}
	++pcfix->depth;
}

static void XMLCALL end_element(void *user_data, const char *name)
{
	struct chunk_file_info_xml *pcfix = (struct chunk_file_info_xml *)user_data;
	--pcfix->depth;
}

static int chunk_file_parser(const char *xmlpath, struct chunk_file_info_xml *pcfix)
{
	int done;
	char buf[BUFSIZ];
	FILE *fp = fopen(xmlpath, "rb");
	if (!fp) {
		printf("No configratin file avaliable!!\n");
		return -1;
	}
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, pcfix);
	XML_SetElementHandler(parser, pcfix->start, end_element);
	do {
		int len = (int)fread(buf, 1, sizeof(buf), fp);
		done = len < sizeof(buf);
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
			fprintf(stderr,
				"%s at line %" XML_FMT_INT_MOD "u\n",
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser));
		return -1;
		}
	} while (!done);
	XML_ParserFree(parser);
	fclose(fp);
	return 0;
}

int init_chunk_file(struct chunk_file_info *base, const char *path)
{
	struct chunk_file_info_xml cfix = {
		.pcfi = base,
		.i = 0,
		.depth = 0,
		.start = start_element_init
	};
	return chunk_file_parser(path, &cfix);
}

int release_chunk_file(struct chunk_file_info *base, const char *path)
{
	struct chunk_file_info_xml cfix = {
		.pcfi = base,
		.i = 0,
		.depth = 0,
		.start = start_element_release
	};
	if (-1 == chunk_file_parser(path, &cfix))
		return -1;
	if (base->fcls)	{
		free(base->fcls);
		base->fcls = NULL;
	}
	if (base->fds) {
		free(base->fds);
		base->fds = NULL;
	}
	if (base->cur_size)	{
		free(base->cur_size);
		base->cur_size = NULL;
	}
	return 0;
}

