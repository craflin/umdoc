
#pragma once

#include "OutputData.hpp"

struct InputData;

class Parser
{
public:
  Parser() : _parserMode(normalMode), _outputData(0),_newParagraphNextLine(false) {}

  bool parse(const InputData& inputData, const String& outputFile, OutputData& outputData);

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
  OutputData* _outputData;
  Error _error;
  List<OutputData::Segment*> _outputSegments;
  List<RefCount::Ptr<OutputData::Segment>> _segments;
  bool _newParagraphNextLine;

private:
  void addSegment(const RefCount::Ptr<OutputData::Segment>& segment, const String& line);

  bool matchFigureImage(const char* s, const char* end, String& title, String& path, String& remainingLine);

  bool parseMarkdown(const String& filePath, const String& fileContent);
  bool parseMarkdownLine(const String& line, int additionalIndent);
  bool parseMarkdownTableLine(int indent, const String& remainingLine);

  bool process(OutputData::OutputFormat format);

  static String translateHtmlEntities(const String& line);
  static String replacePlaceholderVariables(const String& data, const HashMap<String, String>& variables, bool allowEscaping);

  friend class OutputData::EnvironmentSegment;
  friend class OutputData::TableSegment;

  friend void test_Parser_translateHtmlEntities();
  friend void test_Parser_replacePlaceholderVariables();
};
