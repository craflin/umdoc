
#pragma once

#include <nstd/String.h>
#include <nstd/List.h>

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
      environmentType,
      setType,
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
  List<Component> document;
};
