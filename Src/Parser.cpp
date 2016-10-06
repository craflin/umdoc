
#include "Parser.h"
#include "InputData.h"
#include "OutputData.h"

class Parser::Private
{
public:
  enum ParserMode
  {
    normalMode,
    codeMode,
  };

public:
  ParserMode parserMode;
  OutputData* outputData;

public:
  Private() : parserMode(normalMode) {}

  void_t addSegment(OutputData::Segment& segment)
  {
    if(outputData->segments.isEmpty() || !outputData->segments.back()->merge(segment))
      outputData->segments.append(&segment);
  }

  void_t parseMarkdown(const String& filePath, const String& fileContent);
  void_t parseMarkdownLine(const String& line, size_t offset);
};

Parser::Parser() : p(new Private) {}
Parser::~Parser() {delete p;}

bool_t Parser::parse(const InputData& inputData, OutputData& outputData)
{
  p->outputData = &outputData;
  outputData.className = inputData.className;

  for(List<String>::Iterator i = inputData.headerTexFiles.begin(), end = inputData.headerTexFiles.end(); i != end; ++i)
    outputData.headerTexFiles.append(*i);

  for(List<InputData::Component>::Iterator i = inputData.document.begin(), end = inputData.document.end(); i != end; ++i)
  {
    const InputData::Component& component = *i;
    switch(component.type)
    {
    case InputData::Component::texType:
      outputData.segments.append(new OutputData::TexSegment(component.content));
      break;
    case InputData::Component::texTocType:
      outputData.segments.append(new OutputData::TexTocSegment);
      break;
    case InputData::Component::texPartType:
      outputData.segments.append(new OutputData::TexPartSegment(component.content));
      break;
    case InputData::Component::pdfType:
      outputData.segments.append(new OutputData::PdfSegment(component.filePath));
      outputData.hasPdfSegments = true;
      break;
    case InputData::Component::mdType:
      p->parseMarkdown(component.filePath, component.content);
      break;
    }
  }
  return true;
}

void_t Parser::Private::parseMarkdownLine(const String& line, size_t offset)
{
begin:
  int_t indent = 0;
  OutputData::Segment* segment = 0;
  const char_t* p = line;
  for(p += offset; *p == ' '; ++p);
  indent = p - (const char_t*)line;
  String remainingLine;
  remainingLine.attach(p, line.length() - (p - (const char_t*)line));

  if(parserMode == codeMode)
  {
    if(String::compare(p, "```", 3) == 0)
      parserMode = normalMode;
    else
      ((OutputData::CodeSegment*)outputData->segments.back())->addLine(line);
    return;
  }

  switch(*p)
  {
  case '#':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; *i == '#'; ++i)
      if(i < end && String::isSpace(*i))
        segment = new OutputData::TitleSegment(indent, remainingLine);
    }
    break;
  case '=':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; i < end && (String::isSpace(*i) || *i == '='); ++i);
      if(i == end)
        segment = new OutputData::RuleSegment(indent);
    }
    break;
  case '*':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; i < end && (String::isSpace(*i) || *i == '*'); ++i);
      if(i == end)
        segment = new OutputData::RuleSegment(indent);
      else if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int_t childIndent = i - (const char_t*)line;
        segment = new OutputData::ListSegment(indent, childIndent);
        addSegment(*segment);
        offset = i - (const char_t*)line;
        goto begin;
      }
    }
    break;
  case '`':
    if(String::compare(p + 1, "``", 2) == 0)
      segment = new OutputData::CodeSegment(indent);

    break;
  case '\r':
  case '\n':
  case '\0':
    segment = new OutputData::SeparatorSegment(indent);
    break;
  default:;
  }

  if(!segment)
    segment = new OutputData::ParagraphSegment(indent, remainingLine);

  addSegment(*segment);
}

void_t Parser::Private::parseMarkdown(const String& filePath, const String& fileContent)
{
  int_t line = 1;
  String lineStr;
  for(const char_t* p = fileContent, * end; *p; (p = end), ++line)
  {
    end = String::findOneOf(p, "\r\n");
    if(!end)
      end = p + String::length(p);
    
    lineStr.attach(p, end - p);
    parseMarkdownLine(lineStr, 0);

    if(*end == '\r' && end[1] == '\n')
      ++end;
    if(*end)
      ++end;
  }
}
