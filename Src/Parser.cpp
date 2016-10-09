
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
  String errorFile;
  int_t errorLine;
  String errorString;

public:
  Private() : parserMode(normalMode), errorLine(0) {}

  void_t addSegment(OutputData::Segment& segment)
  {
    //if(sgemtn == ListSgement && back == SeparatorSegment && back->level == 0 && back -1 == ListSgement)
    //  (back -1 ).merge(segment)

    if(outputData->segments.isEmpty() || !outputData->segments.back()->merge(segment))
      outputData->segments.append(&segment);
  }

  bool_t parseMarkdown(const String& filePath, const String& fileContent);
  bool_t parseMarkdownLine(const String& line, size_t offset);
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
      if(!p->parseMarkdown(component.filePath, component.content))
        return false;
      break;
    }
  }
  return true;
}

String Parser::getErrorFile() const {return p->errorFile;}
int_t Parser::getErrorLine() const {return p->errorLine;}
String Parser::getErrorString() const {return p->errorString;}

bool_t Parser::Private::parseMarkdownLine(const String& line, size_t offset)
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
    return true;
  }

  switch(*p)
  {
  case '#':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; *i == '#'; ++i);
      if(i < end && String::isSpace(*i))
      {
        segment = new OutputData::TitleSegment(indent, remainingLine);
        break;
      }
    }
    break;
  case '=':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; i < end && *i == '='; ++i);
      for(; i < end && String::isSpace(*i); ++i);
      if(i == end)
      {
        OutputData::ParagraphSegment* paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(outputData->segments.back());
        if(paragraphSegment && paragraphSegment->getIndent() == indent)
        {
          segment = new OutputData::TitleSegment(indent, 1, paragraphSegment->getText());
          delete paragraphSegment;
          outputData->segments.removeBack();
          break;
        }
      }
    }
    break;
  case '-':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; i < end && *i == '-'; ++i);
      for(; i < end && String::isSpace(*i); ++i);
      if(i == end)
      {
        OutputData::ParagraphSegment* paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(outputData->segments.back());
        if(paragraphSegment && paragraphSegment->getIndent() == indent)
        {
          segment = new OutputData::TitleSegment(indent, 2, paragraphSegment->getText());
          delete paragraphSegment;
          outputData->segments.removeBack();
          break;
        }
      }
      i = remainingLine;
      for(; i < end && (*i == '-' || String::isSpace(*i)); ++i);
      if(i == end)
      {
        segment = new OutputData::RuleSegment(indent);
        break;
      }
    }
    break;
  case '*':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; i < end && (String::isSpace(*i) || *i == '*'); ++i);
      if(i == end)
      {
        segment = new OutputData::RuleSegment(indent);
        break;
      }
      if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int_t childIndent = i - (const char_t*)line;
        segment = new OutputData::ListSegment(indent, childIndent);

        //??

        addSegment(*segment);
        offset = i - (const char_t*)line;
        goto begin;
      }
    }
    break;
  case '`':
    if(String::compare(p + 1, "``", 2) == 0)
    {
      OutputData::CodeSegment* codeSegment = new OutputData::CodeSegment(indent);
      if(!codeSegment->parseArguments(remainingLine))
        return false;
      segment = codeSegment;
      parserMode = codeMode;
      break;
    }
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
  return true;
}

bool_t Parser::Private::parseMarkdown(const String& filePath, const String& fileContent)
{
  int_t line = 1;
  String lineStr;
  errorFile = filePath;
  errorLine = 1;
  for(const char_t* p = fileContent, * end; *p; (p = end), ++line)
  {
    end = String::findOneOf(p, "\r\n");
    if(!end)
      end = p + String::length(p);
    
    lineStr.attach(p, end - p);
    if(!parseMarkdownLine(lineStr, 0))
      return false;

    if(*end == '\r' && end[1] == '\n')
      ++end;
    if(*end)
      ++end;
    ++errorLine;
  }
  return true;
}

bool_t OutputData::ParagraphSegment::merge(Segment& segment)
{
  ParagraphSegment* paragraphSegment = dynamic_cast<ParagraphSegment*>(&segment);
  if(paragraphSegment && paragraphSegment->getIndent() == getIndent())
  {
    int_t len = text.length();
    text.append(' ');
    text.append(paragraphSegment->text);
    delete paragraphSegment;
    return true;
  }
  return false;
}

bool_t OutputData::SeparatorSegment::merge(Segment& segment)
{
  if(dynamic_cast<SeparatorSegment*>(&segment))
  {
    ++level;
    delete &segment;
    return true;
  }
  return false;
}

bool_t OutputData::ListSegment::merge(Segment& segment)
{
  ListSegment* listSegment = dynamic_cast<ListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == indent)
  {
    if(parent)
      segment.setParent(*parent);
    siblingSegments.append(listSegment);
    return true;
  }
  ListSegment* lastSibling = siblingSegments.isEmpty() ? this : siblingSegments.back();
  if(segment.getIndent() == lastSibling->childIndent)
  {
    segment.setParent(*lastSibling);
    lastSibling->childSegments.append(&segment);
    return true;
  }
  if(parent)
    return parent->merge(segment);
  return false;
}
