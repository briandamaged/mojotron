/*-------------------------------------------------------------------------*\
  <expatcpp.C> -- Expat C++ implementation file

  Date      Programmer  Description
  01/27/07  Dennis      Created.
\*-------------------------------------------------------------------------*/

#include "expatcpp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define XMLBUFFER 2048

ExpatXMLParser::ExpatXMLParser()
{
}

void ExpatXMLParser::parse(const char *filename)
{
 XML_Parser p = XML_ParserCreate(NULL);
 XML_SetUserData(p, this);
 XML_SetElementHandler(p, startElement, endElement);
 XML_SetCharacterDataHandler(p, characterData);
 char *buffer = new char[XMLBUFFER];
 {
  int fd = open(filename, O_RDONLY);
  if (-1 == fd)
  {
   delete [] buffer;
   XML_ParserFree(p);
   return; // Throw exception
  }
  while (true)
  {
   size_t num = read(fd, buffer, XMLBUFFER);
   try
   {
    XML_Parse(p, buffer, num, (0 == num));
   }
   catch(...)
   {
    delete [] buffer;
    close(fd);
    XML_ParserFree(p);
    throw;
   }
   if (0 == num)
    break;
  }
  close(fd);
 }
 delete [] buffer;
 XML_ParserFree(p);
}

void ExpatXMLParser::startElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
 ExpatXMLParser *parser = (ExpatXMLParser*)userData;
 parser->startElement(name, atts);
}

void ExpatXMLParser::endElement(void *userData, const XML_Char *name)
{
 ExpatXMLParser *parser = (ExpatXMLParser*)userData;
 parser->endElement(name);
}

void ExpatXMLParser::characterData(void *userData, const XML_Char *s, int len)
{
 ExpatXMLParser *parser = (ExpatXMLParser*)userData;
 parser->characterData(s, len);
}
