
#include "Parser.h"

#include <nstd/File.h>
#include <nstd/Directory.h>
#include <nstd/Unicode.h>
#include <nstd/Memory.h>

#include "InputData.h"
#include "OutputData.h"
#include "TexGenerator.h"

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

bool Parser::matchFigureImage(const char* s, const char* end, String& title, String& path, String& remainingLine)
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
  remainingLine.attach(pathEnd, end - pathEnd);
  return true;
}

String Parser::translateHtmlEntities(const String& line)
{
  String result = line;
  for(char* p = result, *entityEnd;; p = entityEnd)
  {
    p = (char*)String::find(p, '&');
    if(!p)
      break;
    entityEnd = (char*)String::findOneOf(p + 1, "; \t");
    if(!entityEnd)
      break;
    if(*entityEnd != ';')
      continue;
    ++entityEnd;
    String entityName;
    entityName.attach(p + 1, entityEnd - p - 2);
    uint32 unicodeValue;
    if(*(const char*)entityName == '#')
    {
      if (entityName.scanf("#%u", &unicodeValue) != 1)
        continue;
    }
    else
    {
      static HashMap<String, uint32> htmlEntitiesMap;
      if(htmlEntitiesMap.isEmpty())
      {
        struct HtmlEntity
        {
          const char* name;
          uint32 unicodeValue;
        };
        HtmlEntity htmlEntityNames[] = {
          {"amp", 38}, {"lt", 60}, {"gt", 62},

          {"Agrave", 192}, {"Aacute", 193}, {"Acirc", 194}, {"Atilde", 195}, {"Auml", 196}, {"Aring", 197},
          {"AElig", 198}, {"Ccedil", 199}, {"Egrave", 200}, {"Eacute", 201}, {"Ecirc", 202}, {"Euml", 203},
          {"Igrave", 204}, {"Iacute", 205}, {"Icirc", 206}, {"Iuml", 207}, {"ETH", 208}, {"Ntilde", 209},
          {"Ograve", 210}, {"Oacute", 211}, {"Ocirc", 212}, {"Otilde", 213}, {"Ouml", 214}, {"Oslash", 216},
          {"Ugrave", 217}, {"Uacute", 218}, {"Ucirc", 219}, {"Uuml", 220}, {"Yacute", 221}, {"THORN", 222},
          {"szlig", 223}, {"agrave", 224}, {"aacute", 225}, {"acirc", 226}, {"atilde", 227}, {"auml", 228},
          {"aring", 229}, {"aelig", 230}, {"ccedil", 231}, {"egrave", 232}, {"eacute", 233}, {"ecirc", 234},
          {"euml", 235}, {"igrave", 236}, {"iacute", 237}, {"icirc", 238}, {"iuml", 239}, {"eth", 240},
          {"ntilde", 241}, {"ograve", 242}, {"oacute", 243}, {"ocirc", 244}, {"otilde", 245}, {"ouml", 246},
          {"oslash", 248}, {"ugrave", 249}, {"uacute", 250}, {"ucirc", 251}, {"uuml", 252}, {"yacute", 253},
          {"thorn", 254}, {"yuml", 255},

          {"nbsp", 160}, {"iexcl", 161}, {"cent", 162}, {"pound", 163}, {"curren", 164}, {"yen", 165},
          {"brvbar", 166}, {"sect", 167}, {"uml", 168}, {"copy", 169}, {"ordf", 170}, {"laquo", 171},
          {"not", 172}, {"shy", 173}, {"reg", 174}, {"macr", 175}, {"deg", 176}, {"plusmn", 177},
          {"sup2", 178}, {"sup3", 179}, {"acute", 180}, {"micro", 181}, {"para", 182}, {"cedil", 184},
          {"sup1", 185}, {"ordm", 186}, {"raquo", 187}, {"frac14", 188}, {"frac12", 189}, {"frac34", 190},
          {"iquest", 191}, {"times", 215}, {"divide", 247},

          {"forall", 8704}, {"part", 8706}, {"exist", 8707}, {"empty", 8709}, {"nabla", 8711}, {"isin", 8712},
          {"notin", 8713}, {"ni", 8715}, {"prod", 8719}, {"sum", 8721}, {"minus", 8722}, {"lowast", 8727},
          {"radic", 8730}, {"prop", 8733}, {"infin", 8734}, {"ang", 8736}, {"and", 8743}, {"or", 8744},
          {"cap", 8745}, {"cup", 8746}, {"int", 8747}, {"there4", 8756}, {"sim", 8764}, {"cong", 8773},
          {"asymp", 8776}, {"ne", 8800}, {"equiv", 8801}, {"le", 8804}, {"ge", 8805}, {"sub", 8834},
          {"sup", 8835}, {"nsub", 8836}, {"sube", 8838}, {"supe", 8839}, {"oplus", 8853}, {"otimes", 8855},
          {"perp", 8869}, {"sdot", 8901},

          {"Alpha", 913}, {"Beta", 914}, {"Gamma", 915}, {"Delta", 916}, {"Epsilon", 917}, {"Zeta", 918},
          {"Eta", 919}, {"Theta", 920}, {"Iota", 921}, {"Kappa", 922}, {"Lambda", 923}, {"Mu", 924},
          {"Nu", 925}, {"Xi", 926}, {"Omicron", 927}, {"Pi", 928}, {"Rho", 929}, {"Sigma", 931},
          {"Tau", 932}, {"Upsilon", 933}, {"Phi", 934}, {"Chi", 935}, {"Psi", 936}, {"Omega", 937},
          {"alpha", 945}, {"beta", 946}, {"gamma", 947}, {"delta", 948}, {"epsilon", 949}, {"zeta", 950},
          {"eta", 951}, {"theta", 952}, {"iota", 953}, {"kappa", 954}, {"lambda", 955}, {"mu", 956},
          {"nu", 957}, {"xi", 958}, {"omicron", 959}, {"pi", 960}, {"rho", 961}, {"sigmaf", 962},
          {"sigma", 963}, {"tau", 964}, {"upsilon", 965}, {"phi", 966}, {"chi", 967}, {"psi", 968},
          {"omega", 969}, {"thetasym", 977}, {"upsih", 978}, {"piv", 982},

          {"OElig", 338}, {"oelig", 339}, {"Scaron", 352}, {"scaron", 353}, {"Yuml", 376}, {"fnof", 402},
          {"circ", 710}, {"tilde", 732}, {"ensp", 8194}, {"emsp", 8195}, {"thinsp", 8201}, {"zwnj", 8204},
          {"zwj", 8205}, {"lrm", 8206}, {"rlm", 8207}, {"ndash", 8211}, {"mdash", 8212}, {"lsquo", 8216},
          {"rsquo", 8217}, {"sbquo", 8218}, {"ldquo", 8220}, {"rdquo", 8221}, {"bdquo", 8222}, {"dagger", 8224},
          {"Dagger", 8225}, {"bull", 8226}, {"hellip", 8230}, {"permil", 8240}, {"prime", 8242}, {"Prime", 8243},
          {"lsaquo", 8249}, {"rsaquo", 8250}, {"oline", 8254}, {"euro", 8364}, {"trade", 8482}, {"larr", 8592},
          {"uarr", 8593}, {"rarr", 8594}, {"darr", 8595}, {"harr", 8596}, {"crarr", 8629}, {"lceil", 8968},
          {"rceil", 8969}, {"lfloor", 8970}, {"rfloor", 8971}, {"loz", 9674}, {"spades", 9824}, {"clubs", 9827},
          {"hearts", 9829}, {"diams", 9830},
        };
        const usize numOfHtmlEntityNames = sizeof(htmlEntityNames) / sizeof(*htmlEntityNames);
        for(usize i = 0; i < numOfHtmlEntityNames; ++i)
          htmlEntitiesMap.append(String::fromCString(htmlEntityNames[i].name), htmlEntityNames[i].unicodeValue);
      }
      HashMap<String, uint32>::Iterator it = htmlEntitiesMap.find(entityName);
      if (it == htmlEntitiesMap.end())
        continue;
      unicodeValue = *it;
    }
    String val = Unicode::toString(unicodeValue);
    Memory::copy(p, (const char*)val, val.length());
    p += val.length();
    Memory::move(p, entityEnd, result.length() - (entityEnd - (char*)result));
    result.resize(result.length() - (entityName.length() + 2 - val.length()));
    entityEnd = p;
  }
  return result;
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
      const char* i = p +3;
      while(*i == '`')
        ++i;
      int backticks = (int)(i - p);

      if(parserMode == childMode)
      {
        OutputData::EnvironmentSegment* parentSegment = (OutputData::EnvironmentSegment*)parentParser->segments.back();
        if(backticks >= parentSegment->getBackticks())
        {
          parentSegment->swapSegments(outputSegments);
          parentParser->environmentParser = 0;
          parentParser->parserMode = parentParser->parentParser ? childMode : normalMode;
          delete this;
          return true;
        }
      }
      if(parserMode == verbatimMode)
      {
        OutputData::EnvironmentSegment* environmentSegment = (OutputData::EnvironmentSegment*)segments.back();
        if(backticks >= environmentSegment->getBackticks())
        {
          parserMode = parentParser ? childMode : normalMode;
          return true;
        }
      }
    }
    if(parserMode == verbatimMode)
    {
      OutputData::EnvironmentSegment* environmentSegment = (OutputData::EnvironmentSegment*)segments.back();
      environmentSegment->addLine(line);
      return true;
    }
  }

  if (remainingLine.find('&'))
  {
    remainingLine = translateHtmlEntities(remainingLine);
    p = remainingLine;
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
      const char* i = p + 3;
      while(*i == '`')
        ++i;
      int backticks = (int)(i - p);
      OutputData::EnvironmentSegment* environmentSegment = new OutputData::EnvironmentSegment(indent, backticks);
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
        environmentParser = new Parser(this, outputData);
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
      String title, path, remainingLine;
      if(matchFigureImage(i, end, title, path, remainingLine))
      {
        OutputData::FigureSegment* figureSegment = new OutputData::FigureSegment(indent, title, path);
        if(!figureSegment->parseArguments(remainingLine, error.string))
        {
          delete figureSegment;
          return false;
        }
        segment = figureSegment;
      }
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
        const char* commentStart = 0;
        if(parserMode != verbatimMode)
            commentStart = lineStr.find("<!--");
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
  const char* attributeStart = 0;
  const char* lineEnd = (const char*)line + line.length();
  for(const char* i = line;;)
  {
    const char* p = String::findOneOf(i, "\\{");
    if(!p || p >= lineEnd)
      break;
    if(*p == '\\')
    {
      i = p + ((p[1] == '\\' || p[1] == '{') ? 2 : 1);
      continue;
    }
    attributeStart = p;
    i = p + 1;
  }
  if(!attributeStart)
    return;
  ++attributeStart;

  const char* end = String::findOneOf(attributeStart, "}\r\n");
  if(!end || *end != '}')
    return;
  for(const char* i = end + 1; i < lineEnd; ++i)
    if(!String::isSpace(*i))
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

bool OutputData::FigureSegment::parseArguments(const String& line, String& error)
{
  String args = line;
  Parser::extractArguments(args, arguments);
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
    HashMap<String, EnvironmentInfo>::Iterator it = knownEnvironments.find(TexGenerator::getEnvironmentName(language));
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

  if(newParagraph && indent == 0 && segment.getIndent() == 0 && !captionSegment)
  {
    ParagraphSegment* paragraphSegment = dynamic_cast<ParagraphSegment*>(&segment);
    if(paragraphSegment && (paragraphSegment->getText().startsWith(":") ||  paragraphSegment->getText().startsWith("Table:")))
    {
      captionSegment = paragraphSegment;
      String text = captionSegment->getText();
      captionSegment->setText(String());
      Parser::extractArguments(text, arguments);
      captionSegment->setText(text);
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

      if(columEnd == end)
      {
        String text = column.text;
        text.trim();
        if(text.isEmpty())
        {
          arguments.insert(columnInfo.arguments);
          columns.removeBack();
          columnData.removeBack();
        }
      }

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

bool Parser::parse(const InputData& inputData, OutputData& outputData)
{
  this->outputData = &outputData;

  outputData.className = inputData.className;
  outputData.variables = inputData.variables;
  for(HashMap<String, String>::Iterator i = inputData.environments.begin(), end = inputData.environments.end(); i != end; ++i)
  {
    OutputData::EnvironmentInfo& environmentInfo = outputData.environments.append(i.key(), OutputData::EnvironmentInfo());
    environmentInfo.verbatim = i->toBool();
  }

  for(List<String>::Iterator i = inputData.headerTexFiles.begin(), end = inputData.headerTexFiles.end(); i != end; ++i)
  {
    String value = *i;
    for(HashMap<String, String>::Iterator i = outputData.variables.begin(), end = outputData.variables.end(); i != end; ++i)
      value.replace(String("%") + i.key() + "%", TexGenerator::texEscape(*i));
    outputData.headerTexFiles.append(value);
  }

  if(outputData.className.isEmpty())
  {
    outputData.environments.append("boxed", OutputData::EnvironmentInfo()).verbatim = false;
    outputData.environments.append("plain", OutputData::EnvironmentInfo()).verbatim = true;
    outputData.environments.append("xplain", OutputData::EnvironmentInfo()).verbatim = true;
    for(usize i = 0; i < TexGenerator::numOfDefaultListingsLanguages; ++i)
      outputData.environments.append(TexGenerator::getEnvironmentName(String::fromCString(TexGenerator::defaultListingsLanguages[i])), OutputData::EnvironmentInfo()).verbatim = true;
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
          value.replace(String("%") + i.key() + "%", TexGenerator::texEscape(*i));
        outputSegments.append(new OutputData::TexSegment(value));
      }
      break;
    case InputData::Component::texTableOfContentsType:
      outputSegments.append(new OutputData::TexSegment("\\tableofcontents"));
      break;
    case InputData::Component::texListOfFiguresType:
      outputSegments.append(new OutputData::TexSegment("\\listoffigures"));
      break;
    case InputData::Component::texListOfTablesType:
      outputSegments.append(new OutputData::TexSegment("\\listoftables"));
      break;
    case InputData::Component::texNewPageType:
      outputSegments.append(new OutputData::TexSegment("\\clearpage"));
      break;
    case InputData::Component::texPartType:
      outputSegments.append(new OutputData::TexPartSegment(component.value));
      break;
    case InputData::Component::pdfType:
      outputSegments.append(new OutputData::PdfSegment(component.filePath));
      this->outputData->hasPdfSegments = true;
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
