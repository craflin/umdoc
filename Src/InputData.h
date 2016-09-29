
#pragma once

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
    };

  public:
    Type type;
    String filePath;
    String content;
  };

public:
  String className;
  List<String> headerTexFiles;
  List<Component> document;
};
