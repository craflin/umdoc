
#pragma once

#include "OutputData.h"

class InputData;

class Parser
{
public:
  Parser() : parserMode(normalMode), outputData(0), environmentParser(0), parentParser(0) {}
  Parser(Parser* parentParser) : parserMode(childMode), outputData(0), environmentParser(0), parentParser(parentParser) {}
  ~Parser();

  bool parse(const InputData& inputData, const String& outputFile, OutputData& outputData);

  String getErrorFile() const {return error.file;}
  int getErrorLine() const {return error.line;}
  String getErrorString() const {return error.string;}

public:
  static void extractArguments(String& line, Map<String, Variant>& args);

private:
  enum ParserMode
  {
    normalMode,
    environmentMode,
    childMode,
    verbatimMode,
  };

  class Error
  {
  public:
    String file;
    int line;
    String string;

  public:
    Error() : line(0) {}
  };


private:
  ParserMode parserMode;
  OutputData* outputData;
  Error error;
  List<OutputData::Segment*> outputSegments;
  List<OutputData::Segment*> segments;
  Parser* environmentParser;
  Parser* parentParser;

private:
  void addSegment(OutputData::Segment& segment);

  bool matchFigureImage(const char* s, const char* end, String& title, String& path);

  bool parseMarkdown(const String& filePath, const String& fileContent);
  bool parseMarkdownLine(const String& line, size_t offset, size_t additionalIndent = 0);
};
