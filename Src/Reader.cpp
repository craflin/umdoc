
#include "Reader.h"

#include <nstd/File.h>
#include <nstd/Error.h>
#include <nstd/Document/XML.h>

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
      return _errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
    if(!file.readAll(component.value))
      return _errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
    return true;
  }

  XML::Parser xmlParser;
  XML::Element xmlFile;
  if(!xmlParser.load(inputFile, xmlFile))
    return _errorLine = xmlParser.getErrorLine(), _errorColumn = xmlParser.getErrorColumn(), _errorString = xmlParser.getErrorString(), false;

  if(xmlFile.type != "umdoc")
    return _errorLine = xmlParser.getErrorLine(), _errorColumn = xmlParser.getErrorColumn(), _errorString = "Expected element 'umdoc'", false;

  inputData.className = *xmlFile.attributes.find("class");

  bool documentRead = false;
  for(List<XML::Variant>::Iterator i = xmlFile.content.begin(), end = xmlFile.content.end(); i != end; ++i)
  {
    const XML::Variant& variant = *i;
    if(!variant.isElement())
      continue;
    const XML::Element& element = variant.toElement();
    if(documentRead)
      return _errorLine = element.line, _errorColumn = element.column, _errorString = String::fromPrintf("Unexpected element '%s'", (const char*)element.type), false;

    if(element.type == "tex")
    {
      String filePath = *element.attributes.find("file");
      File file;
      if(!filePath.isEmpty())
      {
        String data;
        if(!file.open(filePath))
          return _errorLine = element.line, _errorColumn = element.column, _errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)filePath, (const char*)Error::getErrorString()), false;
        if(!file.readAll(data))
          return _errorLine = element.line, _errorColumn = element.column, _errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)filePath, (const char*)Error::getErrorString()), false;
        inputData.headerTexFiles.append(data);
      }
      for(List<XML::Variant>::Iterator i =  element.content.begin(), end = element.content.end(); i != end; ++i)
        inputData.headerTexFiles.append(i->toString());
    }
    else if(element.type == "set")
      inputData.variables.append(*element.attributes.find("name"), *element.attributes.find("value"));
    else if(element.type == "environment")
    {
      InputData::Environment& environment = inputData.environments.append(*element.attributes.find("name"), InputData::Environment());
      environment.verbatim = element.attributes.find("verbatim")->toBool();
      environment.command = *element.attributes.find("command");
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
          if(!component.filePath.isEmpty())
          {
            File file;
            if(!file.open(component.filePath))
              return _errorLine = element.line, _errorColumn = element.column, _errorString = String::fromPrintf("Could not open file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
            if(!file.readAll(component.value))
              return _errorLine = element.line, _errorColumn = element.column, _errorString = String::fromPrintf("Could not read file '%s': %s", (const char*)component.filePath, (const char*)Error::getErrorString()), false;
          }
          for(List<XML::Variant>::Iterator i =  element.content.begin(), end = element.content.end(); i != end; ++i)
            component.value.append(i->toString());
        }
        else if(element.type == "toc" || element.type == "tableOfContents")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texTableOfContentsType;
        }
        else if(element.type == "lof" || element.type == "listOfFigures")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texListOfFiguresType;
        }
        else if(element.type == "lot" || element.type == "listOfTables")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texListOfTablesType;
        }
        else if(element.type == "break" || element.type == "newPage" || element.type == "pageBreak")
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
        {
          InputData::Environment& environment = inputData.environments.append(*element.attributes.find("name"), InputData::Environment());
          environment.verbatim = element.attributes.find("verbatim")->toBool();
          environment.command = *element.attributes.find("command");
        }
        else if(element.type == "set")
          inputData.variables.append(*element.attributes.find("name"), *element.attributes.find("value"));
        else
          return _errorLine = element.line, _errorColumn = element.column, _errorString = String::fromPrintf("Unexpected element '%s'", (const char*)element.type), false;
      }
      documentRead = true;
    }
    else
      return _errorLine = element.line, _errorColumn = element.column, _errorString = String::fromPrintf("Unexpected element '%s'", (const char*)element.type), false;
  }

  return true;
}
