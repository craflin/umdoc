
#include <nstd/File.h>
#include <nstd/Error.h>
#include <nstd/Document/XML.h>

#include "Reader.h"
#include "InputData.h"

bool Reader::read(const String& inputFile, InputData& inputData)
{
  inputData.inputFile = inputFile;

  if(File::extension(inputFile).compareIgnoreCase("md") == 0)
  {
    InputData::Component& component = inputData.document.append(InputData::Component());
    component.type = InputData::Component::mdType;
    component.filePath = inputFile;
    File file;
    if(!file.open(component.filePath))
      return errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
    if(!file.readAll(component.value))
      return errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
    return true;
  }

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
      if(!filePath.isEmpty())
      {
        String data;
        if(!file.open(filePath))
          return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)filePath, (const char*)Error::getErrorString()), false;
        if(!file.readAll(data))
          return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)filePath, (const char*)Error::getErrorString()), false;
        inputData.headerTexFiles.append(data);
      }
      for(List<XML::Variant>::Iterator i =  element.content.begin(), end = element.content.end(); i != end; ++i)
        inputData.headerTexFiles.append(i->toString());
    }
    else if(element.type == "set")
      inputData.variables.append(*element.attributes.find("name"), *element.attributes.find("value"));
    else if(element.type == "environment")
      inputData.environments.append(*element.attributes.find("name"), *element.attributes.find("verbatim"));
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
          if(!component.filePath.isEmpty())
          {
            File file;
            if(!file.open(component.filePath))
              return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
            if(!file.readAll(component.value))
              return errorLine = element.line, errorColumn = element.column, errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
          }
          for(List<XML::Variant>::Iterator i =  element.content.begin(), end = element.content.end(); i != end; ++i)
            component.value.append(i->toString());
        }
        else if(element.type == "toc" || element.type == "tableOfContents")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texTocType;
        }
        else if(element.type == "break" || element.type == "newPage")
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
          component.value = *element.attributes.find("title");
        }
        else if(element.type == "environment")
          inputData.environments.append(*element.attributes.find("name"), *element.attributes.find("verbatim"));
        else if(element.type == "set")
          inputData.variables.append(*element.attributes.find("name"), *element.attributes.find("value"));
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
