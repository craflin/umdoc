
#pragma once

#include <nstd/String.hpp>
#include <nstd/List.hpp>
#include <nstd/HashMap.hpp>

struct InputData
{
  struct Component
  {
    enum Type
    {
      texType,
      texTableOfContentsType,
      texListOfFiguresType,
      texListOfTablesType,
      texNewPageType,
      texPartType,
      pdfType,
      mdType,
    };

    Type type;
    String name;
    String filePath;
    String value;
  };

  struct Environment
  {
    bool verbatim;
    String command;
  };

  String inputFile;
  String className;
  List<String> headerTexFiles;
  HashMap<String, String> variables;
  HashMap<String, Environment> environments;
  List<Component> document;
};
