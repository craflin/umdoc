
#include <nstd/Console.h>
#include <nstd/File.h>
#include <nstd/Error.h>
#include <nstd/Document/XML.h>

#include "Reader.h"
#include "InputData.h"

bool_t Reader::read(const String& inputFile, InputData& inputData)
{
  XML::Parser xmlParser;
  XML::Element xmlFile;
  if(!xmlParser.load(inputFile, xmlFile))
  {
    Console::errorf("%s:%d:%d: error: %s\n", (const char_t*)inputFile, xmlParser.getErrorLine(), xmlParser.getErrorColumn(), (const char_t*)xmlParser.getErrorString());
    return false;
  }

  if(xmlFile.type != "md2tex")
  {
    Console::errorf("%s:%d: error: Expected element 'md2tex'\n", (const char_t*)inputFile, xmlFile.line);
    return false;
  }
  inputData.className = *xmlFile.attributes.find("class");

  bool documentRead = false;
  for(List<XML::Variant>::Iterator i = xmlFile.content.begin(), end = xmlFile.content.end(); i != end; ++i)
  {
    const XML::Variant& variant = *i;
    if(!variant.isElement())
      continue;
    const XML::Element& element = variant.toElement();
    if(documentRead)
    {
      Console::errorf("%s:%d:%d: error: Unexpected element '%s'\n", (const char_t*)inputFile, element.line, element.column, (const char_t*)element.type);
      return false;
    }
    if(element.type == "tex")
    {
      String filePath = *element.attributes.find("file");
      File file;
      String data;
      if(!file.open(filePath))
      {
        Console::errorf("%s:%d:%d: error: Could not open file '%s': %s\n", (const char_t*)inputFile, element.line, element.column, (const char_t*)filePath, (const char_t*)Error::getErrorString());
        return false;
      }
      if(!file.readAll(data))
      {
        Console::errorf("%s:%d:%d: error: Could not read file '%s': %s\n", (const char_t*)inputFile, element.line, element.column, (const char_t*)filePath, (const char_t*)Error::getErrorString());
        return false;
      }
      inputData.headerTexFiles.append(data);

    }
    else if(element.type == "document")
    {
      for(List<XML::Variant>::Iterator i = element.content.begin(), end = element.content.end(); i != end; ++i)
      {
        const XML::Variant& variant = *i;
        if(!variant.isElement())
          continue;
        const XML::Element& element = variant.toElement();
        if(element.type == "tex" || element.type == "md")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = element.type == "md" ? InputData::Component::mdType : InputData::Component::texType;
          component.filePath = *element.attributes.find("file");
          File file;
          if(!file.open(component.filePath))
          {
            Console::errorf("%s:%d:%d: error: Could not open file '%s': %s\n", (const char_t*)inputFile, element.line, element.column, (const char_t*)component.filePath, (const char_t*)Error::getErrorString());
            return false;
          }
          if(!file.readAll(component.content))
          {
            Console::errorf("%s:%d:%d: error: Could not read file '%s': %s\n", (const char_t*)inputFile, element.line, element.column, (const char_t*)component.filePath, (const char_t*)Error::getErrorString());
            return false;
          }
        }
        else if(element.type == "toc")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texTocType;
        }
        else if(element.type == "pdf")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::pdfType;
          component.filePath = *element.attributes.find("file");
        }
        else if(element.type == "part")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texPartType;
          component.content = *element.attributes.find("title");
        }
        else
        {
          Console::errorf("%s:%d: error: Unexpected element '%s'\n", (const char_t*)inputFile, element.line, (const char_t*)element.type);
          return false;
        }
      }
      documentRead = true;
    }
    else
    {
      Console::errorf("%s:%d: error: Unexpected element '%s'\n", (const char_t*)inputFile, element.line, (const char_t*)element.type);
      return false;
    }
  }

  return true;
}
