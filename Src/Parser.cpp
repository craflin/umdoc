
#include <nstd/File.h>
#include <nstd/Directory.h>

#include "Parser.h"
#include "InputData.h"
#include "OutputData.h"
#include "Generator.h"

Parser::~Parser()
{
  delete environmentParser;
  for(List<OutputData::Segment*>::Iterator i = outputSegments.begin(), end = outputSegments.end(); i != end; ++i)
    delete *i;
}

void Parser::addSegment(OutputData::Segment& newSegment)
{
  if(!segments.isEmpty())
  {
    OutputData::Segment* lastSegment = segments.back();
    if(!lastSegment->isValid())
      segments.removeBack();
    if(!segments.isEmpty())
    {
      lastSegment = segments.back();
      bool lastIsSeparator = dynamic_cast<OutputData::SeparatorSegment*>(lastSegment) != 0;
      if(lastIsSeparator && dynamic_cast<OutputData::SeparatorSegment*>(&newSegment))
        lastSegment->merge(newSegment, false);
      else
      {
        if(lastIsSeparator && segments.size() > 1)
          lastSegment = *--(--segments.end());
        for(OutputData::Segment* segment = lastSegment;;)
        {
          if(segment->merge(newSegment, lastIsSeparator))
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
  }

  outputSegments.append(&newSegment);
  segments.append(&newSegment);
}

bool Parser::matchFigureImage(const char* s, const char* end, String& title, String& path)
{
  if(*s != '!')
    return false;
  if(*(++s) != '[')
    return false;
  const char* titleStart = ++s;
  while(*s != ']')
    if(++s >= end)
      return false;
  const char* titleEnd = s++;
  if(*s != '(')
    return false;
  const char* pathStart = ++s;
  const char* pathEnd = 0;
  while(*s != ')')
  {
    if(*s == ' ' && !pathEnd)
      pathEnd = s;
    if(++s >= end)
      return false;
  }
  if(!pathEnd)
    pathEnd = s;
  title.attach(titleStart, titleEnd - titleStart);
  path.attach(pathStart, pathEnd - pathStart);
  return true;
}

bool Parser::parseMarkdownLine(const String& line, usize additionalIndent)
{
  if(parserMode == environmentMode)
  {
    if(!environmentParser->parseMarkdownLine(line, additionalIndent))
      return error = environmentParser->error, false;
    return true;
  }

  int indent = additionalIndent;
  OutputData::Segment* segment = 0;
  const char* p = line;
  for(; *p == ' '; ++p);
  indent += p - (const char*)line;
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
        OutputData::TitleSegment* titleSegment = new OutputData::TitleSegment(indent, titleLevel);
        if(!titleSegment->parseArguments(title, error.string))
          return false;
        segment = titleSegment;
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
          OutputData::TitleSegment* titleSegment = new OutputData::TitleSegment(indent, 1);
          if(!titleSegment->parseArguments(paragraphSegment->getText(), error.string))
            return false;
          segment = titleSegment;
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
        segment = new OutputData::RuleSegment(indent);
        break;
      }
      if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int childIndent = i - (const char*)line;
        OutputData::BulletListSegment* listSegment = new OutputData::BulletListSegment(indent, '*', childIndent);
        addSegment(*listSegment);
        usize offset = i - (const char*)remainingLine;
        remainingLine.attach(i, remainingLine.length() - offset);
        return parseMarkdownLine(remainingLine, indent + offset);
      }
    }
    break;
  case '_':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      for(; i < end && (String::isSpace(*i) || *i == '_'); ++i);
      if(i == end)
      {
        segment = new OutputData::RuleSegment(indent);
        break;
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
          OutputData::TitleSegment* titleSegment = new OutputData::TitleSegment(indent, 2);
          if(!titleSegment->parseArguments(paragraphSegment->getText(), error.string))
            return false;
          segment = titleSegment;
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
        usize offset = i - (const char*)remainingLine;
        remainingLine.attach(i, remainingLine.length() - offset);
        return parseMarkdownLine(remainingLine, indent + offset);
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
        usize offset = i - (const char*)remainingLine;
        remainingLine.attach(i, remainingLine.length() - offset);
        return parseMarkdownLine(remainingLine, indent + offset);
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
        environmentParser = new Parser(this);
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
      p += 2;
      usize offset = p - (const char*)remainingLine;
      remainingLine.attach(p, remainingLine.length() - offset);
      return parseMarkdownLine(remainingLine, indent + offset);
    }
    break;
  case '!':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      String title, path;
      if(matchFigureImage(i, end, title, path))
        segment = new OutputData::FigureSegment(indent, title, path);
    }
    break;
  case '|':
    {
      OutputData::TableSegment* tableSegment = new OutputData::TableSegment(indent);
      List<OutputData::TableSegment::ColumnData> columns;
      if(!tableSegment->parseArguments(remainingLine, columns, error.string))
      {
        delete tableSegment;
        return false;
      }
      addSegment(*tableSegment);
      for(List<OutputData::TableSegment::ColumnData>::Iterator i = columns.begin(), end = columns.end(); i != end; ++i)
        if(!parseMarkdownLine(i->text, i->indent))
          return false;
      return true;
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
        usize offset = i - (const char*)remainingLine;
        remainingLine.attach(i, remainingLine.length() - offset);
        return parseMarkdownLine(remainingLine, indent + offset);
      }
    }
  }

  if(!segment)
    segment = new OutputData::ParagraphSegment(indent, remainingLine);

  addSegment(*segment);
  return true;
}

bool Parser::parseMarkdown(const String& filePath, const String& fileContent)
{
  int line = 1;
  String lineStr;
  error.file = filePath;
  error.line = 1;

  bool inComment = false;
  String preCommentLine;

  for(const char* p = fileContent, * end; *p; (p = end), ++line)
  {
    end = String::findOneOf(p, "\r\n");
    if(!end)
      end = p + String::length(p);
    
    lineStr.attach(p, end - p);
    char endChar = *end;
    *(char*)end = '\0';

    for(;;)
    {
      if(inComment)
      {
        const char* commentEnd = lineStr.find("-->");
        if(commentEnd)
        {
          lineStr = preCommentLine + lineStr.substr(commentEnd + 3 - (const char*)lineStr);
          inComment = false;
          continue;
        }
        else
          break;
      }
      else
      {
        const char* commentStart = lineStr.find("<!--");
        if(commentStart)
        {
          preCommentLine = lineStr.substr(0, commentStart - (const char*)lineStr);
          lineStr =  lineStr.substr(commentStart + 4- (const char*)lineStr);
          inComment = true;
          continue;
        }
        else
        {
          if(!parseMarkdownLine(lineStr, 0))
            return false;
          break;
        }
      }
    }

    *(char*)end = endChar;

    if(*end == '\r' && end[1] == '\n')
      ++end;
    if(*end)
      ++end;
    ++error.line;
  }
  return true;
}

void Parser::extractArguments(String& line, Map<String, Variant>& args)
{
  const char* attributeStart = line.findLast('{');
  if(!attributeStart)
    return;
  ++attributeStart;
  const char* end = String::findOneOf(attributeStart, "}\r\n");
  if(!end && *end != '}')
    return;
  List<String> attributes;
  line.substr(attributeStart - (const char*)line, end - attributeStart).split(attributes, " \t");
  for(List<String>::Iterator i = attributes.begin(), end = attributes.end(); i != end; ++i)
  {
    if(i->startsWith("#"))
      args.insert("#", i->substr(1));
    else
    {
      const char* x = i->find('=');
      if(x)
        args.insert(i->substr(0, x - (const char*)*i), i->substr(x + 1 - (const char*)*i));
      else
        args.insert(*i, Variant());
    }
  }
  line = line.substr(0, attributeStart - 1 - (const char*)line);
}

bool Parser::extractStringArgument(String& line, String& result)
{
  const char* start = line;
  const char* i = start;
  const char* end = start + line.length();
  while(String::isSpace(*i) && i < end)
    ++i;
  if(*i == '{')
    return false;
  if(*i == '"')
  {
    ++i;
    result.clear();
    for(; i < end; ++i)
    {
      if(*i == '\\' && i + 1 < end)
      {
        ++i;
        result.append(*i);
      }
      else if(*i == '"')
      {
        ++i;
        break;
      }
      else
        result.append(*i);
    }
    while(String::isSpace(*i) && i < end)
      ++i;
    line = line.substr(i - start);
    return true;
  }
  else if(i < end)
  {
    const char* strStart = i;
    while(!String::isSpace(*i) && i < end)
      ++i;
    result = line.substr(strStart - start, i - strStart);
    while(String::isSpace(*i) && i < end)
      ++i;
    line = line.substr(i - start);
    return true;
  }
  return false;
}

bool OutputData::ParagraphSegment::merge(Segment& segment, bool newParagraph)
{
  if(newParagraph)
    return false;
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

bool OutputData::SeparatorSegment::merge(Segment& segment, bool newParagraph)
{
  if(dynamic_cast<SeparatorSegment*>(&segment))
  {
    ++lines;
    segment.invalidate();
    return true;
  }
  return false;
}

bool OutputData::TitleSegment::parseArguments(const String& title, String& error)
{
  this->title = title;
  Parser::extractArguments(this->title, arguments);
  return true;
}

bool OutputData::BulletListSegment::merge(Segment& segment, bool newParagraph)
{
  BulletListSegment* listSegment = dynamic_cast<BulletListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == indent)
  {
    if(parent)
    {
      BulletListSegment* parentListSegment = dynamic_cast<BulletListSegment*>(parent);
      if(parentListSegment && parentListSegment->getIndent() == indent)
        return parentListSegment->merge(segment, newParagraph);
    }

    listSegment->setParent(*this);
    siblingSegments.append(listSegment); // todo: merge?
    return true;
  }
  if(segment.getIndent() == this->childIndent)
  {
    segment.setParent(*this);
    childSegments.append(&segment); // todo: merge?
    return true;
  }
  return false;
}

bool OutputData::NumberedListSegment::merge(Segment& segment, bool newParagraph)
{
  NumberedListSegment* listSegment = dynamic_cast<NumberedListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == indent)
  {
    if(parent)
    {
      NumberedListSegment* parentListSegment = dynamic_cast<NumberedListSegment*>(parent);
      if(parentListSegment && parentListSegment->getIndent() == indent)
        return parentListSegment->merge(segment, newParagraph);
    }

    listSegment->setParent(*this);
    siblingSegments.append(listSegment); // todo: merge?
    return true;
  }
  if(segment.getIndent() == this->childIndent)
  {
    segment.setParent(*this);
    childSegments.append(&segment); // todo: merge?
    return true;
  }
  return false;
}

bool OutputData::BlockquoteSegment::merge(Segment& segment, bool newParagraph)
{
  BlockquoteSegment* blockSegment = dynamic_cast<BlockquoteSegment*>(&segment);
  if(blockSegment && blockSegment->getIndent() == indent)
  {
    if(parent)
    {
      BlockquoteSegment* parentBlockquoteSegment = dynamic_cast<BlockquoteSegment*>(parent);
      if(parentBlockquoteSegment && parentBlockquoteSegment->getIndent() == indent)
        return parentBlockquoteSegment->merge(segment, newParagraph);
    }

    blockSegment->setParent(*this);
    siblingSegments.append(blockSegment); // todo: merge?
    return true;
  }
  if(segment.getIndent() == this->childIndent)
  {
    segment.setParent(*this);
    childSegments.append(&segment); // todo: merge?
    return true;
  }
  return false;
}

bool OutputData::EnvironmentSegment::parseArguments(const String& line, const HashMap<String, EnvironmentInfo>& knownEnvironments, String& error)
{
  const char* i = line;
  while(*i == '`')
    ++i;
  String argLine;
  argLine.attach(i, line.length() - (i - (const char*)line));
  String caption;
  if(Parser::extractStringArgument(argLine, language))
    Parser::extractStringArgument(argLine, caption);
  Parser::extractArguments(argLine, arguments);

  if(!caption.isEmpty())
    arguments.insert("caption", caption);

  if(!language.isEmpty())
  {
    HashMap<String, EnvironmentInfo>::Iterator it = knownEnvironments.find(Generator::getEnvironmentName(language));
    if(it == knownEnvironments.end())
    {
      error = String("Unknown environment '") + language + "'";
      return false;
    }
    verbatim = it->verbatim;
  }

  return true;
}

bool OutputData::TableSegment::merge(Segment& segment, bool newParagraph)
{
  TableSegment* tableSegment = dynamic_cast<TableSegment*>(&segment);
  if(tableSegment && tableSegment->getIndent() == indent && !newParagraph)
  {
    segment.invalidate();
    
    if(tableSegment->columns.size() > columns.size())
    {
      usize newColumnCount = tableSegment->columns.size();
      for(List<RowData>::Iterator i = rows.begin(), end = rows.end(); i != end; ++i)
        i->cellData.resize(newColumnCount);
      for(usize i = columns.size(); i < newColumnCount; ++i)
        columns.append(0);
    }
    for(usize i = 0; i < tableSegment->columns.size(); ++i)
    {
      ColumnInfo& columnInfo = columns[i];
      const ColumnInfo& srcColumnInfo = tableSegment->columns[i];
      columnInfo.indent = srcColumnInfo.indent;
      if(tableSegment->isSeparatorLine && columnInfo.alignment == ColumnInfo::undefinedAlignment)
        columnInfo.alignment = srcColumnInfo.alignment;
    }

    if(!tableSegment->isSeparatorLine)
      rows.append(RowData()).cellData.resize(columns.size());
    return true;
  }
  usize column = 0;
  for(Array<ColumnInfo>::Iterator i = columns.begin(), end = columns.end(); i != end; ++i, ++column)
  {
    const ColumnInfo& columnInfo = *i;
    if(segment.getIndent() == columnInfo.indent)
    {
      if(rows.isEmpty())
        rows.append(RowData()).cellData.resize(columns.size());
      RowData& rowData = rows.back();
      CellData& cellData = rowData.cellData[column];
      if(cellData.segments.isEmpty() || newParagraph || !cellData.segments.back()->merge(segment, false))
        cellData.segments.append(&segment);
      segment.setParent(*this);
      return true;
    }
  }
  return false;
}

bool OutputData::TableSegment::parseArguments(const String& line, List<ColumnData>& columnData, String& error)
{
  const char* start = line;
  const char* end = start + line.length();
  for(const char* i = start; i < end; ++i)
    if(*i == '|')
    {
      ++i;
      while(i < end && String::isSpace(*i))
        ++i;
      if(i >= end)
        break;
      const char* columStart = i++;
      for(; i < end && *i != '|'; ++i)
        if(*i == '\\' && i[1] == '|')
          ++i;
      const char* columEnd = i;
      if(columEnd == columStart)
        break;
      ColumnInfo& columnInfo = columns.append(ColumnInfo(indent + (columStart - start)));
      ColumnData& column = columnData.append(ColumnData());
      column.indent = columnInfo.indent;
      column.text.attach(columStart, columEnd - columStart);
      Parser::extractArguments(column.text, columnInfo.arguments);
      --i;
    }

  isSeparatorLine = true;
  usize columnIndex = 0;
  for(List<ColumnData>::Iterator i = columnData.begin(), end = columnData.end(); i != end; ++i, ++columnIndex)
  {
    const ColumnData& column = *i;
    for(const char* i = column.text, * end = i + column.text.length(); i < end; ++i)
    {
      while(String::isSpace(*i) && i < end)
        ++i;
      bool leftTick = false, rightTick = false;
      if(*i == ':')
      {
        ++i;
        leftTick = true;
      }
      if(*i != '-' || i[1] != '-' || i[2] != '-')
      {
        isSeparatorLine = false;
        break;
      }
      i += 3;
      while(*i == '-' && i < end)
        ++i;
      if(*i == ':')
      {
        ++i;
        rightTick = true;
      }
      while(String::isSpace(*i) && i < end)
        ++i;
      if(i != end)
      {
        isSeparatorLine = false;
        break;
      }
      if(leftTick && rightTick)
        columns[columnIndex].alignment = ColumnInfo::centerAlignment;
      else if(rightTick)
        columns[columnIndex].alignment = ColumnInfo::rightAlignment;
      else if(leftTick)
        columns[columnIndex].alignment = ColumnInfo::leftAlignment;
    }
    if(!isSeparatorLine)
      break;
  }

  if(isSeparatorLine)
    columnData.clear();
  return true;
}

bool Parser::parse(const InputData& inputData, const String& outputFile, OutputData& outputData)
{
  this->outputData = &outputData;

  outputData.inputDirectory = File::simplifyPath(File::dirname(File::isAbsolutePath(inputData.inputFile) ? inputData.inputFile : Directory::getCurrent() + "/" + inputData.inputFile));
  outputData.outputDirectory = File::simplifyPath(File::dirname(File::isAbsolutePath(outputFile) ? outputFile : Directory::getCurrent() + "/" + outputFile));
  outputData.className = inputData.className;
  outputData.variables = inputData.variables;
  for(HashMap<String, String>::Iterator i = inputData.environments.begin(), end = inputData.environments.end(); i != end; ++i)
  {
    OutputData::EnvironmentInfo& environmentInfo = outputData.environments.append(i.key(), OutputData::EnvironmentInfo());
    environmentInfo.verbatim = i->toBool();
  }

  for(List<String>::Iterator i = inputData.headerTexFiles.begin(), end = inputData.headerTexFiles.end(); i != end; ++i)
    outputData.headerTexFiles.append(*i);

  if(outputData.className.isEmpty())
  {
    outputData.environments.append("latexexample", OutputData::EnvironmentInfo()).verbatim = false;
    outputData.environments.append("plain", OutputData::EnvironmentInfo()).verbatim = true;
    for(usize i = 0; i < Generator::numOfDefaultListingsLanguages; ++i)
      outputData.environments.append(Generator::getEnvironmentName(String::fromCString(Generator::defaultListingsLanguages[i])), OutputData::EnvironmentInfo()).verbatim = true;
  }

  for(List<InputData::Component>::Iterator i = inputData.document.begin(), end = inputData.document.end(); i != end; ++i)
  {
    const InputData::Component& component = *i;
    switch(component.type)
    {
    case InputData::Component::texType:
      {
        String value = component.value;
        for(HashMap<String, String>::Iterator i = outputData.variables.begin(), end = outputData.variables.end(); i != end; ++i)
          value.replace(String("%") + i.key() + "%", Generator::texEscape(*i, outputData));
        outputSegments.append(new OutputData::TexSegment(value));
      }
      break;
    case InputData::Component::texTocType:
      outputSegments.append(new OutputData::TexSegment("\\tableofcontents"));
      break;
    case InputData::Component::texNewPageType:
      outputSegments.append(new OutputData::TexSegment("\\clearpage"));
      break;
    case InputData::Component::texPartType:
      outputSegments.append(new OutputData::TexPartSegment(component.value));
      break;
    case InputData::Component::pdfType:
      outputSegments.append(new OutputData::PdfSegment(component.filePath));
      outputData.hasPdfSegments = true;
      break;
    case InputData::Component::mdType:
      segments.clear();
      if(!parseMarkdown(component.filePath, component.value))
        return false;
      break;
    }
  }
  outputData.segments.swap(outputSegments);
  return true;
}
