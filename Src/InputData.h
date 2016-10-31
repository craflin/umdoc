
#pragma once

#include <nstd/String.h>
#include <nstd/List.h>
#include <nstd/HashMap.h>

class InputData
{
public:
  class Component
  {
  public:
    enum Type
    {
      texType,
      texTocType,
      texNewPageType,
      texPartType,
      pdfType,
      mdType,
    };

  public:
    Type type;
    String name;
    String filePath;
    String value;
  };

public:
  String inputFile;
  String className;
  List<String> headerTexFiles;
  HashMap<String, String> variables;
  HashMap<String, String> environments;
  List<Component> document;
};
