
#include <nstd/File.h>
#include <nstd/Directory.h>

#include "Parser.h"
#include "InputData.h"
#include "OutputData.h"

class Parser::Private
{
public:
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

public:
  ParserMode parserMode;
  OutputData* outputData;
  Error error;
  List<OutputData::Segment*> outputSegments;
  List<OutputData::Segment*> segments;
  Private* environmentParser;
  Private* parentParser;

public:
  Private() : parserMode(normalMode), outputData(0), environmentParser(0), parentParser(0) {}
  Private(Private* parentParser) : parserMode(childMode), outputData(0), environmentParser(0), parentParser(parentParser) {}
  ~Private();

  void addSegment(OutputData::Segment& segment);

  bool parseMarkdown(const String& filePath, const String& fileContent);
  bool parseMarkdownLine(const String& line, size_t offset);
};

Parser::Private::~Private()
{
  delete environmentParser;
  for(List<OutputData::Segment*>::Iterator i = outputSegments.begin(), end = outputSegments.end(); i != end; ++i)
    delete *i;
}

void Parser::Private::addSegment(OutputData::Segment& newSegment)
{
  if(!segments.isEmpty())
  {
    OutputData::Segment* lastSegment = segments.back();
    if(!lastSegment->isValid())
      segments.removeBack();
    if(!segments.isEmpty())
    {
      lastSegment = segments.back();
      for(OutputData::Segment* segment = lastSegment;;)
      {
        if(segment->merge(newSegment))
        {
          if(!segments.back()->isValid())
            segments.removeBack();
          if(newSegment.isValid())
            segments.append(&newSegment);
          else
            delete &newSegment;
          return;
        }
        segment = segment->getParent();
        if(!segment)
          break;
      }
    }
  }

  outputSegments.append(&newSegment);
  segments.append(&newSegment);
}

bool Parser::Private::parseMarkdownLine(const String& line, size_t offset)
{
  if(parserMode == environmentMode)
  {
    if(!environmentParser->parseMarkdownLine(line, offset))
      return error = environmentParser->error, false;
    return true;
  }

begin:
  int indent = 0;
  OutputData::Segment* segment = 0;
  const char* p = line;
  for(p += offset; *p == ' '; ++p);
  indent = p - (const char*)line;
  String remainingLine;
  remainingLine.attach(p, line.length() - (p - (const char*)line));

  if(parserMode != normalMode)
  {
    if(String::compare(p, "```", 3) == 0)
    {
      if(parserMode == childMode)
      {
        OutputData::EnvironmentSegment* parentSegment = (OutputData::EnvironmentSegment*)parentParser->segments.back();
        parentSegment->swapSegments(outputSegments);
        parentParser->environmentParser = 0;
        parentParser->parserMode = normalMode;
        delete this;
      }
      else
        parserMode = normalMode;
      return true;
    }
    else if(parserMode == verbatimMode)
    {
      OutputData::EnvironmentSegment* environmentSegment = (OutputData::EnvironmentSegment*)segments.back();
      environmentSegment->addLine(line);
      return true;
    }
  }

  switch(*p)
  {
  case '#':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      for(; *i == '#'; ++i);
      if(i < end && String::isSpace(*i))
      {
        int titleLevel = i - ( const char*)remainingLine;
        ++i;
        String title;
        title.attach(i, remainingLine.length() - (i - (const char*)remainingLine));
        segment = new OutputData::TitleSegment(indent, titleLevel, title);
        break;
      }
    }
    break;
  case '=':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      for(; i < end && *i == '='; ++i);
      for(; i < end && String::isSpace(*i); ++i);
      if(i == end)
      {
        OutputData::ParagraphSegment* paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(segments.back());
        if(paragraphSegment && paragraphSegment->getIndent() == indent)
        {
          paragraphSegment->invalidate();
          segment = new OutputData::TitleSegment(indent, 1, paragraphSegment->getText());
          break;
        }
      }
    }
    break;
  case '*':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      for(; i < end && (String::isSpace(*i) || *i == '*'); ++i);
      if(i == end)
      {
        if(parentParser)
        {
          int k = 42;
        }
        segment = new OutputData::RuleSegment(indent);
        break;
      }
      if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int childIndent = i - (const char*)line;
        OutputData::BulletListSegment* listSegment = new OutputData::BulletListSegment(indent, '*', childIndent);
        addSegment(*listSegment);
        offset = i - (const char*)line;
        goto begin;
      }
    }
    break;
  case '-':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      for(; i < end && *i == '-'; ++i);
      for(; i < end && String::isSpace(*i); ++i);
      if(i == end)
      {
        OutputData::ParagraphSegment* paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(segments.back());
        if(paragraphSegment && paragraphSegment->getIndent() == indent)
        {
          paragraphSegment->invalidate();
          segment = new OutputData::TitleSegment(indent, 2, paragraphSegment->getText());
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
      if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int childIndent = i - (const char*)line;
        OutputData::BulletListSegment* listSegment = new OutputData::BulletListSegment(indent, '-', childIndent);
        addSegment(*listSegment);
        offset = i - (const char*)line;
        goto begin;
      }
    }
    break;
  case '+':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int childIndent = i - (const char*)line;
        OutputData::BulletListSegment* listSegment = new OutputData::BulletListSegment(indent, '+', childIndent);
        addSegment(*listSegment);
        offset = i - (const char*)line;
        goto begin;
      }
    }
    break;
  case '`':
    if(String::compare(p + 1, "``", 2) == 0)
    {
      OutputData::EnvironmentSegment* environmentSegment = new OutputData::EnvironmentSegment(indent);
      if(!environmentSegment->parseArguments(remainingLine, outputData->environments, error.string))
      {
        delete environmentSegment;
        return false;
      }
      segment = environmentSegment;
      if(environmentSegment->isVerbatim())
        parserMode = verbatimMode;
      else
      {
        environmentParser = new Private(this);
        parserMode = environmentMode;
      }
      break;
    }
    break;
  case '>':
    if(String::isSpace(p[1]))
    {
      OutputData::BlockquoteSegment* blockquoteSegment = new OutputData::BlockquoteSegment(indent, indent + 2);
      addSegment(*blockquoteSegment);
      offset = p + 2 - (const char*)line;
      goto begin;
    }
    break;
  case '\r':
  case '\n':
  case '\0':
    segment = new OutputData::SeparatorSegment(indent);
    break;
  default:;
    if(String::isDigit(*p))
    {
      const char* i = p + 1;
      for(; String::isDigit(*i); ++i);
      if(*i == '.' && String::isSpace(i[1]))
      {
        const char* end = i + remainingLine.length();
        for(i += 2; i < end && String::isSpace(*i); ++i);
        int childIndent = i - (const char*)line;
        String numberStr;
        OutputData::NumberedListSegment* numberedListSegment = new OutputData::NumberedListSegment(indent, remainingLine.toUInt(), childIndent);
        addSegment(*numberedListSegment);
        offset = i - (const char*)line;
        goto begin;
      }
    }
  }

  if(!segment)
    segment = new OutputData::ParagraphSegment(indent, remainingLine);

  addSegment(*segment);
  return true;
}

bool Parser::Private::parseMarkdown(const String& filePath, const String& fileContent)
{
  int line = 1;
  String lineStr;
  error.file = filePath;
  error.line = 1;
  for(const char* p = fileContent, * end; *p; (p = end), ++line)
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
    ++error.line;
  }
  return true;
}

bool OutputData::ParagraphSegment::merge(Segment& segment)
{
  ParagraphSegment* paragraphSegment = dynamic_cast<ParagraphSegment*>(&segment);
  if(paragraphSegment && paragraphSegment->getIndent() == getIndent())
  {
    text.append(' ');
    text.append(paragraphSegment->getText());
    segment.invalidate();
    return true;
  }
  return false;
}

bool OutputData::SeparatorSegment::merge(Segment& segment)
{
  if(dynamic_cast<SeparatorSegment*>(&segment))
  {
    ++lines;
    segment.invalidate();
    return true;
  }
  return false;
}

bool OutputData::BulletListSegment::merge(Segment& segment)
{
  BulletListSegment* listSegment = dynamic_cast<BulletListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == indent && listSegment->getSymbol() == symbol)
  {
    if(parent)
    {
      BulletListSegment* parentListSegment = dynamic_cast<BulletListSegment*>(parent);
      if(parentListSegment && parentListSegment->getIndent() == indent)
        return parentListSegment->merge(segment);
    }

    listSegment->setParent(*this);
    siblingSegments.append(listSegment);
    return true;
  }
  if(segment.getIndent() == this->childIndent || dynamic_cast<SeparatorSegment*>(&segment))
  {
    segment.setParent(*this);
    childSegments.append(&segment);
    return true;
  }
  return false;
}

bool OutputData::NumberedListSegment::merge(Segment& segment)
{
  NumberedListSegment* listSegment = dynamic_cast<NumberedListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == indent)
  {
    if(parent)
    {
      NumberedListSegment* parentListSegment = dynamic_cast<NumberedListSegment*>(parent);
      if(parentListSegment && parentListSegment->getIndent() == indent)
        return parentListSegment->merge(segment);
    }

    listSegment->setParent(*this);
    siblingSegments.append(listSegment);
    return true;
  }
  if(segment.getIndent() == this->childIndent || dynamic_cast<SeparatorSegment*>(&segment))
  {
    segment.setParent(*this);
    childSegments.append(&segment);
    return true;
  }
  return false;
}

bool OutputData::BlockquoteSegment::merge(Segment& segment)
{
  BlockquoteSegment* blockSegment = dynamic_cast<BlockquoteSegment*>(&segment);
  if(blockSegment && blockSegment->getIndent() == indent)
  {
    if(parent)
    {
      BlockquoteSegment* parentBlockquoteSegment = dynamic_cast<BlockquoteSegment*>(parent);
      if(parentBlockquoteSegment && parentBlockquoteSegment->getIndent() == indent)
        return parentBlockquoteSegment->merge(segment);
    }

    blockSegment->setParent(*this);
    siblingSegments.append(blockSegment);
    return true;
  }
  if(segment.getIndent() == this->childIndent || dynamic_cast<SeparatorSegment*>(&segment))
  {
    segment.setParent(*this);
    childSegments.append(&segment);
    return true;
  }
  return false;
}

bool OutputData::EnvironmentSegment::parseArguments(const String& line, const HashMap<String, bool>& knownEnvironments, String& error)
{
  const char* start = line;
  const char* end = start + line.length();
  const char* i = start;
  while(*i == '`')
    ++i;
  for(; i < end && String::isSpace(*i); ++i);
  const char* languageStart = i;
  for(; i < end && String::isAlpha(*i); ++i);
  const char* languageEnd = i;
  for(; i < end && String::isSpace(*i); ++i);

  language = String(languageStart, languageEnd - languageStart);

  if(!language.isEmpty())
  {
    language.toLowerCase();
    HashMap<String, bool>::Iterator it = knownEnvironments.find(language);
    if(it == knownEnvironments.end())
    {
      error = String("Unknown environment '") + language + "'";
      return false;
    }
    verbatim = *it;
  }

  return true;
}

Parser::Parser() : p(new Private) {}
Parser::~Parser() {delete p;}

bool Parser::parse(const InputData& inputData, const String& outputFile, OutputData& outputData)
{
  p->outputData = &outputData;

  outputData.inputDirectory = File::simplifyPath(File::dirname(File::isAbsolutePath(inputData.inputFile) ? inputData.inputFile : Directory::getCurrent() + "/" + inputData.inputFile));
  outputData.outputDirectory = File::simplifyPath(File::dirname(File::isAbsolutePath(outputFile) ? outputFile : Directory::getCurrent() + "/" + outputFile));
  outputData.className = inputData.className;

  for(List<String>::Iterator i = inputData.headerTexFiles.begin(), end = inputData.headerTexFiles.end(); i != end; ++i)
    outputData.headerTexFiles.append(*i);

  if(outputData.className.isEmpty())
  {
    outputData.environments.append("latexexample", false);
  }

  for(List<InputData::Component>::Iterator i = inputData.document.begin(), end = inputData.document.end(); i != end; ++i)
  {
    const InputData::Component& component = *i;
    switch(component.type)
    {
    case InputData::Component::texType:
      p->outputSegments.append(new OutputData::TexSegment(component.content));
      break;
    case InputData::Component::texTocType:
      p->outputSegments.append(new OutputData::TexSegment("\\pagestyle{empty}\n\\tableofcontents"));
      break;
    case InputData::Component::texNewPageType:
      p->outputSegments.append(new OutputData::TexSegment("\\clearpage"));
      break;
    case InputData::Component::texPartType:
      p->outputSegments.append(new OutputData::TexPartSegment(component.content));
      break;
    case InputData::Component::pdfType:
      p->outputSegments.append(new OutputData::PdfSegment(component.filePath));
      outputData.hasPdfSegments = true;
      break;
    case InputData::Component::mdType:
      p->segments.clear();
      if(!p->parseMarkdown(component.filePath, component.content))
        return false;
      break;
    case InputData::Component::environmentType:
      outputData.environments.append(component.name, component.content.toBool());
      break;
    }
  }
  outputData.segments.swap(p->outputSegments);
  return true;
}

String Parser::getErrorFile() const {return p->error.file;}
int Parser::getErrorLine() const {return p->error.line;}
String Parser::getErrorString() const {return p->error.string;}
