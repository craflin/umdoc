
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
      texPartType,
      pdfType,
      mdType,
      environmentType,
    };

  public:
    Type type;
    String name;
    String filePath;
    String content;
  };

public:
  String inputFile;
  String className;
  List<String> headerTexFiles;
  List<Component> document;
};
