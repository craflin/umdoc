
#pragma once

#include "OutputData.hpp"

struct InputData;

class Parser
{
public:
  Parser() : _parserMode(normalMode), _newParagraphNextLine(false) {}

  bool parse(const InputData& inputData, const String& outputFile, OutputData& outputData);
  bool parseMarkdown(const OutputData::Info& info, const String& filePath, const String& fileContent, List<OutputData::SegmentPtr>& segments);

  String getErrorFile() const {return _error.file;}
  int getErrorLine() const {return _error.line;}
  String getErrorString() const {return _error.string;}

public:
  static void extractArguments(String& line, Map<String, Variant>& args);
  static bool extractStringArgument(String& line, String& result);

private:
  enum ParserMode
  {
    normalMode,
    environmentMode,
    verbatimEnvironmentMode,
  };

  struct Error
  {
    String file;
    int line;
    String string;

    Error() : line(0) {}
  };

private:
  ParserMode _parserMode;
  Error _error;
  List<OutputData::SegmentPtr> _outputSegments;
  List<OutputData::SegmentPtr> _segments;
  bool _newParagraphNextLine;

private:
  void addSegment2(const RefCount::Ptr<OutputData::Segment>& segment, bool newLine, const String& data);

  bool matchFigureImage(const char* s, const char* end, String& title, String& path, String& remainingLine);

  bool parseMarkdown(const OutputData::Info& info, const String& filePath, const String& fileContent);
  bool parseMarkdownLine(const OutputData::Info& info, const String& line, int additionalIndent);
  bool parseMarkdownTableLine(int indent, bool newLine, const String& remainingLine);

  bool process(const OutputData::Info& info);

  static String translateHtmlEntities(const String& line);
  static String replacePlaceholderVariables(const String& data, const HashMap<String, String>& variables, bool allowEscaping);

  friend void test_Parser_translateHtmlEntities();
  friend void test_Parser_replacePlaceholderVariables();
};
