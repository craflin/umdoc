
#include <nstd/File.h>
#include <nstd/Error.h>
#include <nstd/Document/XML.h>

#include "Reader.h"
#include "InputData.h"

bool Reader::read(const String& inputFile, InputData& inputData)
{
  // todo: if inputFile.endsWitdh(".md") skip xml stuff

  inputData.inputFile = inputFile;

  XML::Parser xmlParser;
  XML::Element xmlFile;
  if(!xmlParser.load(inputFile, xmlFile))
    return errorLine = xmlParser.getErrorLine(), errorColumn = xmlParser.getErrorColumn(), errorString = xmlParser.getErrorString(), false;

  if(xmlFile.type != "umdoc")
    return errorLine = xmlParser.getErrorLine(), errorColumn = xmlParser.getErrorColumn(), errorString = "Expected element 'umdoc'", false;

  inputData.className = *xmlFile.attributes.find("class");

  bool documentRead = false;
  for(List<XML::Variant>::Iterator i = xmlFile.content.begin(), end = xmlFile.content.end(); i != end; ++i)
  {
    const XML::Variant& variant = *i;
    if(!variant.isElement())
      continue;
    const XML::Element& element = variant.toElement();
    if(documentRead)
      return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Unexpected element '%s'", (const char*)element.type), false;

    if(element.type == "tex")
    {
      String filePath = *element.attributes.find("file");
      File file;
      String data;
      if(!file.open(filePath))
        return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)filePath, (const char*)Error::getErrorString()), false;

      if(!file.readAll(data))
        return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)filePath, (const char*)Error::getErrorString()), false;

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
            return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;

          if(!file.readAll(component.content))
            return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
        }
        else if(element.type == "tableOfContents" || element.type == "toc")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texTocType;
        }
        else if(element.type == "newPage")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texNewPageType;
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
        else if(element.type == "environment")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::environmentType;
          component.name = *element.attributes.find("name");
          component.content = *element.attributes.find("verbatim");
        }
        else
            return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Unexpected element '%s'", (const char*)element.type), false;
      }
      documentRead = true;
    }
    else
      return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Unexpected element '%s'", (const char*)element.type), false;
  }

  return true;
}
