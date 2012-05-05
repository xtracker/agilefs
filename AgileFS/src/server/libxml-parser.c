#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>



int main (int argc , char **argv)
{
  xmlDocPtr     pdoc = NULL;
  xmlNodePtr    proot = NULL, curNode = NULL;
  char *psfilename;

  if (argc < 1)
    {
      printf ("用法: %s xml文件名\n", argv[0]);
      exit (1);
    }

  psfilename = argv[1];
  // 打开 xml 文档
  //xmlKeepBlanksDefault(0);
  pdoc = xmlReadFile (psfilename, "UTF-8", XML_PARSE_RECOVER);

  if (pdoc == NULL)
    {
      printf ("打开文件 %s 出错!\n", psfilename);
      exit (1);
    }

  // 获取 xml 文档对象的根节对象
  proot = xmlDocGetRootElement (pdoc);

  if (proot == NULL)
    {
      printf("错: %s 是空文档（没有root节点）！\n", psfilename);
      exit (1);
    }

  /* 我使用上面程序创建的 xml 文档，它的根节点是“根节点”，这里比较是否
     正确。*/
  if (xmlStrcmp (proot->name, BAD_CAST "root") != 0)
    {
      printf ("错误文档" );
      exit (1);
    }

  /* 如果打开的 xml 对象有 version 属性，那么就输出它的值。 */
  if (xmlHasProp (proot, BAD_CAST "version"))
    {
      xmlChar *szAttr = xmlGetProp (proot, BAD_CAST "版本");
      printf ("版本: %s \n根节点：%s\n" , szAttr, proot->name);
    }
  else
    {
      printf (" xml 文档没有版本信息\n");
    }

  curNode = proot->xmlChildrenNode;

  char n=0;
  while (curNode != NULL)
    {
      if (curNode->name != BAD_CAST "chunk")
        {
          printf ("子节点%d： %s\n", n++,curNode->name);
        }
      curNode = curNode->next;
    }

  /* 关闭和清理 */
  xmlFreeDoc (pdoc);
  xmlCleanupParser ();
  return 0;
}
