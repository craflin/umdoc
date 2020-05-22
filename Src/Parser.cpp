
#include "Parser.h"

#include <nstd/File.h>
#include <nstd/Directory.h>
#include <nstd/Unicode.h>
#include <nstd/Memory.h>
#include <nstd/Process.h>
#include <nstd/Error.h>
#include <nstd/Thread.h>

#include "InputData.h"
#include "OutputData.h"
#include "TexGenerator.h"

Parser::~Parser()
{
  delete _environmentParser;
  for(List<OutputData::Segment*>::Iterator i = _outputSegments.begin(), end = _outputSegments.end(); i != end; ++i)
    delete *i;
}

void Parser::addSegment(OutputData::Segment& newSegment)
{
  if(!_segments.isEmpty())
  {
    OutputData::Segment* lastSegment = _segments.back();
    if(!lastSegment->isValid())
      _segments.removeBack();
    if(!_segments.isEmpty())
    {
      lastSegment = _segments.back();
      OutputData::SeparatorSegment* lastSeparator = dynamic_cast<OutputData::SeparatorSegment*>(lastSegment);
      bool lastIsSeparator = lastSeparator && (lastSeparator->getIndent() == 0 || lastSeparator->getIndent() == newSegment.getIndent());
      if(lastIsSeparator && dynamic_cast<OutputData::SeparatorSegment*>(&newSegment))
        lastSegment->merge(newSegment, false);
      else
      {
        if(lastIsSeparator && _segments.size() > 1)
          lastSegment = *--(--_segments.end());
        for(OutputData::Segment* segment = lastSegment;;)
        {
          if(segment->merge(newSegment, lastIsSeparator))
          {
            if(!_segments.back()->isValid())
              _segments.removeBack();
            if(newSegment.isValid())
              _segments.append(&newSegment);
            else
              delete &newSegment;
            return;
          }
          segment = segment->_parent;
          if(!segment)
            break;
        }
      }
    }
  }

  _outputSegments.append(&newSegment);
  _segments.append(&newSegment);
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
          // HTML 1.0
          {"quot", 34}, {"amp", 38}, {"apos", 39}, {"lt", 60}, {"gt", 62},
          /*
          // HTML 2.0
          {"Agrave", 192}, {"Aacute", 193}, {"Acirc", 194}, {"Atilde", 195}, {"Auml", 196}, {"Aring", 197},
          {"AElig", 198}, {"Ccedil", 199}, {"Egrave", 200}, {"Eacute", 201}, {"Ecirc", 202}, {"Euml", 203},
          {"Igrave", 204}, {"Iacute", 205}, {"Icirc", 206}, {"Iuml", 207}, {"ETH", 208}, {"Ntilde", 209},
          {"Ograve", 210}, {"Oacute", 211}, {"Ocirc", 212}, {"Otilde", 213}, {"Ouml", 214},
          {"Oslash", 216},
          {"Ugrave", 217}, {"Uacute", 218}, {"Ucirc", 219}, {"Uuml", 220}, {"Yacute", 221}, {"THORN", 222},
          {"szlig", 223}, {"agrave", 224}, {"aacute", 225}, {"acirc", 226}, {"atilde", 227}, {"auml", 228},
          {"aring", 229}, {"aelig", 230}, {"ccedil", 231}, {"egrave", 232}, {"eacute", 233}, {"ecirc", 234},
          {"euml", 235}, {"igrave", 236}, {"iacute", 237}, {"icirc", 238}, {"iuml", 239}, {"eth", 240},
          {"ntilde", 241}, {"ograve", 242}, {"oacute", 243}, {"ocirc", 244}, {"otilde", 245}, {"ouml", 246},
          {"oslash", 248}, {"ugrave", 249}, {"uacute", 250}, {"ucirc", 251}, {"uuml", 252}, {"yacute", 253},
          {"thorn", 254}, {"yuml", 255},
          */
          // HTML 3.2
          {"nbsp", 160}, /*{"iexcl", 161}, {"cent", 162}, {"pound", 163}, {"curren", 164}, {"yen", 165},
          {"brvbar", 166}, {"sect", 167}, {"uml", 168}, {"copy", 169}, {"ordf", 170}, {"laquo", 171},
          {"not", 172}, {"shy", 173}, {"reg", 174}, {"macr", 175}, {"deg", 176}, {"plusmn", 177},
          {"sup2", 178}, {"sup3", 179}, {"acute", 180}, {"micro", 181}, {"para", 182}, {"middot", 183},
          {"cedil", 184}, {"sup1", 185}, {"ordm", 186}, {"raquo", 187}, {"frac14", 188}, {"frac12", 189},
          {"frac34", 190}, {"iquest", 191}, {"times", 215}, {"divide", 247},

          // HTML 4.0
          {"OElig", 338}, {"oelig", 339}, {"Scaron", 352}, {"scaron", 353},
          {"Yuml", 376}, {"fnof", 402}, {"circ", 710}, {"tilde", 732}, 
          {"Alpha", 913}, {"Beta", 914}, {"Gamma", 915}, {"Delta", 916}, {"Epsilon", 917}, {"Zeta", 918},
          {"Eta", 919}, {"Theta", 920}, {"Iota", 921}, {"Kappa", 922}, {"Lambda", 923}, {"Mu", 924},
          {"Nu", 925}, {"Xi", 926}, {"Omicron", 927}, {"Pi", 928}, {"Rho", 929}, {"Sigma", 931},
          {"Tau", 932}, {"Upsilon", 933}, {"Phi", 934}, {"Chi", 935}, {"Psi", 936}, {"Omega", 937},
          {"alpha", 945}, {"beta", 946}, {"gamma", 947}, {"delta", 948}, {"epsilon", 949}, {"zeta", 950},
          {"eta", 951}, {"theta", 952}, {"iota", 953}, {"kappa", 954}, {"lambda", 955}, {"mu", 956},
          {"nu", 957}, {"xi", 958}, {"omicron", 959}, {"pi", 960}, {"rho", 961}, {"sigmaf", 962},
          {"sigma", 963}, {"tau", 964}, {"upsilon", 965}, {"phi", 966}, {"chi", 967}, {"psi", 968},
          {"omega", 969}, {"thetasym", 977}, {"upsih", 978}, {"piv", 982},

          {"ensp", 8194}, {"emsp", 8195}, {"thinsp", 8201}, {"zwnj", 8204},
          {"zwj", 8205}, {"lrm", 8206}, {"rlm", 8207}, {"ndash", 8211}, {"mdash", 8212}, {"lsquo", 8216},
          {"rsquo", 8217}, {"sbquo", 8218}, {"ldquo", 8220}, {"rdquo", 8221}, {"bdquo", 8222}, {"dagger", 8224},
          {"Dagger", 8225}, {"bull", 8226}, {"hellip", 8230}, {"permil", 8240}, {"prime", 8242}, {"Prime", 8243},
          {"lsaquo", 8249}, {"rsaquo", 8250}, {"oline", 8254}, {"frasl", 6260}, {"euro", 8364},
          {"image", 8465}, {"weierp", 8472}, {"real", 8476}, {"trade", 8482}, {"alefsym", 8501},

          {"larr", 8592}, {"uarr", 8593}, {"rarr", 8594}, {"darr", 8595}, {"harr", 8596}, {"crarr", 8629},
          {"lArr", 8656}, {"uArr", 8657}, {"rArr", 8658}, {"dArr", 8659}, {"hArr", 8660},

          {"forall", 8704}, {"part", 8706}, {"exist", 8707}, {"empty", 8709}, {"nabla", 8711}, {"isin", 8712},
          {"notin", 8713}, {"ni", 8715}, {"prod", 8719}, {"sum", 8721}, {"minus", 8722}, {"lowast", 8727},
          {"radic", 8730}, {"prop", 8733}, {"infin", 8734}, {"ang", 8736}, {"and", 8743}, {"or", 8744},
          {"cap", 8745}, {"cup", 8746}, {"int", 8747}, {"there4", 8756}, {"sim", 8764}, {"cong", 8773},
          {"asymp", 8776}, {"ne", 8800}, {"equiv", 8801}, {"le", 8804}, {"ge", 8805}, {"sub", 8834},
          {"sup", 8835}, {"nsub", 8836}, {"nsup", 8837}, {"sube", 8838}, {"supe", 8839}, {"oplus", 8853},
          {"otimes", 8855}, {"perp", 8869}, {"sdot", 8901},

          {"lceil", 8968}, {"rceil", 8969}, {"lfloor", 8970}, {"rfloor", 8971}, 
          {"lang", 9000}, {"rang", 9001},
          {"loz", 9674}, {"spades", 9824}, {"clubs", 9827}, {"hearts", 9829}, {"diams", 9830},
          */
          // HTML 5.0
          // I have no idea why the HTML guys keep adding new ones.
          // This is a pointless endeavor that is leading nowhere.
          // You cannot cover all unicode charaters using this approach.
          // The HTML 1.0 entities serve a purpose and &nbsp; is quite useful for some quick formatting workarounds.
          // Just use UTF-8 or the numeric format if you are restricted to the ascii charset for some reason.
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

bool Parser::parseMarkdownTableLine(int indent, const String& remainingLine)
{
    OutputData::TableSegment* tableSegment = new OutputData::TableSegment(indent);
    List<OutputData::TableSegment::ColumnData> columns;
    if(!tableSegment->parseArguments(remainingLine, columns, _error.string))
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

bool Parser::parseMarkdownLine(const String& line, usize additionalIndent)
{
  if(_parserMode == environmentMode)
  {
    if(!_environmentParser->parseMarkdownLine(line, additionalIndent))
      return _error = _environmentParser->_error, false;
    return true;
  }

  int indent = additionalIndent;
  OutputData::Segment* segment = 0;
  const char* p = line;
  for(; *p == ' '; ++p);
  indent += p - (const char*)line;
  String remainingLine;
  remainingLine.attach(p, line.length() - (p - (const char*)line));

  if(_parserMode != normalMode)
  {
    if(String::compare(p, "```", 3) == 0)
    {
      const char* i = p +3;
      while(*i == '`')
        ++i;
      int backticks = (int)(i - p);

      if(_parserMode == childMode)
      {
        OutputData::EnvironmentSegment* parentSegment = (OutputData::EnvironmentSegment*)_parentParser->_segments.back();
        if(backticks >= parentSegment->_backticks)
        {
          parentSegment->_segments.swap(_outputSegments);
          _parentParser->_environmentParser = 0;
          _parentParser->_parserMode = _parentParser->_parentParser ? childMode : normalMode;
          delete this;
          return true;
        }
      }
      if(_parserMode == verbatimMode)
      {
        OutputData::EnvironmentSegment* environmentSegment = (OutputData::EnvironmentSegment*)_segments.back();
        if(backticks >= environmentSegment->_backticks)
        {
          if(!environmentSegment->process(_outputData->format, _error.string))
            return false;
          _parserMode = _parentParser ? childMode : normalMode;
          return true;
        }
      }
    }
    if(_parserMode == verbatimMode)
    {
      OutputData::EnvironmentSegment* environmentSegment = (OutputData::EnvironmentSegment*)_segments.back();
      environmentSegment->_lines.append(line);
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
        if(!titleSegment->parseArguments(title, _error.string))
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
        OutputData::ParagraphSegment* paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(_segments.back());
        if(paragraphSegment && paragraphSegment->getIndent() == indent)
        {
          paragraphSegment->invalidate();
          OutputData::TitleSegment* titleSegment = new OutputData::TitleSegment(indent, 1);
          if(!titleSegment->parseArguments(paragraphSegment->_text, _error.string))
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
        OutputData::ParagraphSegment* paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(_segments.back());
        if(paragraphSegment && paragraphSegment->getIndent() == indent)
        {
          paragraphSegment->invalidate();
          OutputData::TitleSegment* titleSegment = new OutputData::TitleSegment(indent, 2);
          if(!titleSegment->parseArguments(paragraphSegment->_text, _error.string))
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
    if(p[1] == '-' || p[1] == '=')
      return parseMarkdownTableLine(indent, remainingLine);
    else
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
      if(!environmentSegment->parseArguments(remainingLine, _outputData->environments, _error.string))
      {
        delete environmentSegment;
        return false;
      }
      segment = environmentSegment;
      if(environmentSegment->_verbatim || !environmentSegment->_command.isEmpty())
        _parserMode = verbatimMode;
      else
      {
        _environmentParser = new Parser(this, _outputData);
        _parserMode = environmentMode;
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
        if(!figureSegment->parseArguments(remainingLine, _error.string))
        {
          delete figureSegment;
          return false;
        }
        segment = figureSegment;
      }
    }
    break;
  case '|':
    return parseMarkdownTableLine(indent, remainingLine);
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
  _error.file = filePath;
  _error.line = 1;

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
        if(_parserMode != verbatimMode)
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
    ++_error.line;
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
    else if(*i == "-")
      args.insert(".unnumbered", Variant());
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
    _text.append(' ');
    _text.append(paragraphSegment->_text);
    segment.invalidate();
    return true;
  }
  return false;
}

bool OutputData::SeparatorSegment::merge(Segment& segment, bool newParagraph)
{
  if(dynamic_cast<SeparatorSegment*>(&segment))
  {
    ++_lines;
    segment.invalidate();
    return true;
  }
  return false;
}

bool OutputData::TitleSegment::parseArguments(const String& title, String& error)
{
  _title = title;
  Parser::extractArguments(_title, _arguments);
  return true;
}

bool OutputData::FigureSegment::parseArguments(const String& line, String& error)
{
  String args = line;
  Parser::extractArguments(args, _arguments);
  return true;
}

bool OutputData::BulletListSegment::merge(Segment& segment, bool newParagraph)
{
  BulletListSegment* listSegment = dynamic_cast<BulletListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == _indent)
  {
    if(_parent)
    {
      BulletListSegment* parentListSegment = dynamic_cast<BulletListSegment*>(_parent);
      if(parentListSegment && parentListSegment->getIndent() == _indent)
        return parentListSegment->merge(segment, newParagraph);
    }

    listSegment->_parent = this;
    _siblingSegments.append(listSegment); // todo: merge?
    return true;
  }
  if(segment.getIndent() == _childIndent)
  {
    segment._parent = this;
    _childSegments.append(&segment); // todo: merge?
    return true;
  }
  return false;
}

bool OutputData::NumberedListSegment::merge(Segment& segment, bool newParagraph)
{
  NumberedListSegment* listSegment = dynamic_cast<NumberedListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == _indent)
  {
    if(_parent)
    {
      NumberedListSegment* parentListSegment = dynamic_cast<NumberedListSegment*>(_parent);
      if(parentListSegment && parentListSegment->getIndent() == _indent)
        return parentListSegment->merge(segment, newParagraph);
    }

    listSegment->_parent = this;
    _siblingSegments.append(listSegment); // todo: merge?
    return true;
  }
  if(segment.getIndent() == _childIndent)
  {
    segment._parent = this;
    _childSegments.append(&segment); // todo: merge?
    return true;
  }
  return false;
}

bool OutputData::BlockquoteSegment::merge(Segment& segment, bool newParagraph)
{
  BlockquoteSegment* blockSegment = dynamic_cast<BlockquoteSegment*>(&segment);
  if(blockSegment && blockSegment->getIndent() == _indent)
  {
    if(_parent)
    {
      BlockquoteSegment* parentBlockquoteSegment = dynamic_cast<BlockquoteSegment*>(_parent);
      if(parentBlockquoteSegment && parentBlockquoteSegment->getIndent() == _indent)
        return parentBlockquoteSegment->merge(segment, newParagraph);
    }

    blockSegment->_parent = this;
    _siblingSegments.append(blockSegment); // todo: merge?
    return true;
  }
  if(segment.getIndent() == _childIndent)
  {
    segment._parent = this;
    _childSegments.append(&segment); // todo: merge?
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
  if(Parser::extractStringArgument(argLine, _language))
    Parser::extractStringArgument(argLine, caption);
  Parser::extractArguments(argLine, _arguments);

  if(!caption.isEmpty())
    _arguments.insert("caption", caption);

  if(!_language.isEmpty())
  {
    HashMap<String, EnvironmentInfo>::Iterator it = knownEnvironments.find(TexGenerator::getEnvironmentName(_language));
    if(it == knownEnvironments.end())
    {
      error = String("Unknown environment '") + _language + "'";
      return false;
    }
    _verbatim = it->verbatim;
    _command = it->command;
  }

  return true;
}

namespace {
  class ReaderThreadData
  {
  public:
    uint read()
    {
      char buf[4096];
      _output.append(String());
      for (ssize len;;)
      {
        len = _process.read(buf, sizeof(buf) - 1);
        if(len > 0)
        {
          buf[len] = '\0';
          for (const char* i = buf, *end;;)
          {
            if((end = String::find(i, '\n')) != 0)
            {
              const char* realEnd = end;
              while(realEnd > i && String::isSpace(realEnd[-1]))
                --realEnd;
              _output.back().append(String(i, realEnd - i));
              _output.append(String());
              i = end + 1;
              continue;
            }
            else
            {
              _output.back().append(String::fromCString(i));
              break;
            }
          }
        }
        else
          break;
      }
      if(_output.back().isEmpty())
        _output.removeBack();
      return 0;
    }
    ReaderThreadData(Process& process, List<String>& output) : _process(process), _output(output) {}
  public:
    Process& _process;
    List<String>& _output;
  };
}

bool OutputData::EnvironmentSegment::process(OutputData::OutputFormat format_, String& error)
{
  if(_command.isEmpty())
    return true;

  String format = "latex";
  if (format_ == OutputData::htmlFormat)
    format = "html";

  String command = _command;
#ifndef _WIN32
  if (!File::isAbsolutePath(command) && !command.find("/"))
    command.prepend("./");
#endif

  Process process;
  if(!process.open(String("\"") + command + "\" " + format, Process::stdoutStream | Process::stdinStream))
  {
#ifdef _WIN32
    if(!process.open(String("\"") + command + ".bat\" " + format, Process::stdoutStream | Process::stdinStream))
#endif
      return error = Error::getErrorString(), false;
  }
  List<String> input;
  input.swap(_lines);
  Thread readerThread;
  ReaderThreadData readerThreadData(process, _lines);
  if(!readerThread.start(readerThreadData, &ReaderThreadData::read))
    return error = Error::getErrorString(), false;
  for(List<String>::Iterator i = input.begin(), end = input.end(); i != end; ++i)
    if(process.write((const char*)*i, i->length()) != i->length())
      return error = Error::getErrorString(), false;
  process.close();
  if(readerThread.join() != 0)
    return error = "Could not join reader thread", false;

  if(!_verbatim)
  {
    Parser parser;
    String fileContent;
    fileContent.join(_lines, '\n');
    if (!parser.parseMarkdown(_command, fileContent))
      return error = parser.getErrorString(), false;
    _segments.swap(parser._outputSegments);
  }

  return true;
}

bool OutputData::TableSegment::merge(Segment& segment, bool newParagraph)
{
  TableSegment* tableSegment = dynamic_cast<TableSegment*>(&segment);
  if(tableSegment && tableSegment->getIndent() == _indent && !newParagraph)
  {
    segment.invalidate();

    if(tableSegment->_tableType != TableSegment::PipeTable)
      _tableType = tableSegment->_tableType;

    if(tableSegment->_columns.size() > _columns.size())
    {
      usize newColumnCount = tableSegment->_columns.size();
      for(List<RowData>::Iterator i = _rows.begin(), end = _rows.end(); i != end; ++i)
        i->cellData.resize(newColumnCount);
      for(usize i = _columns.size(); i < newColumnCount; ++i)
        _columns.append(tableSegment->_columns[i].indent);
    }
    for(usize i = 0; i < tableSegment->_columns.size(); ++i)
    {
      ColumnInfo& columnInfo = _columns[i];
      const ColumnInfo& srcColumnInfo = tableSegment->_columns[i];
      columnInfo.indent = srcColumnInfo.indent;
      if(tableSegment->_isSeparatorLine && columnInfo.alignment == ColumnInfo::undefinedAlignment)
        columnInfo.alignment = srcColumnInfo.alignment;
    }

    if(_tableType != TableSegment::GridTable || tableSegment->_isSeparatorLine)
      _forceNewRowNextMerge = true;
    return true;
  }
  if (_forceNewRowNextMerge)
  {
    _forceNewRowNextMerge = false;
    _rows.append(RowData()).cellData.resize(_columns.size());
  }
  usize column = 0;
  for(Array<ColumnInfo>::Iterator i = _columns.begin(), end = _columns.end(); i != end; ++i, ++column)
  {
    const ColumnInfo& columnInfo = *i;
    if(segment.getIndent() == columnInfo.indent)
    {
      if(_rows.isEmpty())
        _rows.append(RowData()).cellData.resize(_columns.size());
      RowData& rowData = _rows.back();
      CellData& cellData = rowData.cellData[column];
      if(cellData.segments.isEmpty() || newParagraph || !cellData.segments.back()->merge(segment, false))
        cellData.segments.append(&segment);
      segment._parent = this;
      return true;
    }
  }

  if(newParagraph && _indent == 0 && segment.getIndent() == 0 && !_captionSegment)
  {
    ParagraphSegment* paragraphSegment = dynamic_cast<ParagraphSegment*>(&segment);
    if(paragraphSegment && (paragraphSegment->_text.startsWith(":") ||  paragraphSegment->_text.startsWith("Table:")))
    {
      _captionSegment = paragraphSegment;
      String text = _captionSegment->_text;
      _captionSegment->_text = String();
      Parser::extractArguments(text, _arguments);
      _captionSegment->_text = text;
      return true;
    }
  }

  return false;
}

bool OutputData::TableSegment::parseArguments(const String& line, List<ColumnData>& columnData, String& error)
{
  const char* start = line;
  char separatorChar = *start;
  if (separatorChar == '+')
    _tableType = GridTable;
  const char* end = start + line.length();
  for(const char* i = start; i < end; ++i)
    if(*i == separatorChar)
    {
      ++i;
      while(i < end && String::isSpace(*i))
        ++i;
      if(i >= end)
        break;
      const char* columStart = i;
      for(; i < end && *i != separatorChar; ++i)
        if(*i == '\\' && i[1] == separatorChar)
          ++i;
      const char* columEnd = i;

      ColumnInfo& columnInfo = _columns.append(ColumnInfo(_indent + (columStart - start)));
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
          _arguments.insert(columnInfo.arguments);
          _columns.removeBack();
          columnData.removeBack();
        }
      }

      --i;
    }

  _isSeparatorLine = true;
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
        if(_tableType == PipeTable || *i != '=' || i[1] != '=' || i[2] != '=')
        {
          _isSeparatorLine = false;
          break;
        }
      }
      i += 3;
      while((*i == '-' || (_tableType == GridTable && *i == '=')) && i < end)
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
        _isSeparatorLine = false;
        break;
      }
      if(leftTick && rightTick)
        _columns[columnIndex].alignment = ColumnInfo::centerAlignment;
      else if(rightTick)
        _columns[columnIndex].alignment = ColumnInfo::rightAlignment;
      else if(leftTick)
        _columns[columnIndex].alignment = ColumnInfo::leftAlignment;
    }
    if(!_isSeparatorLine)
      break;
  }

  if(_isSeparatorLine)
    columnData.clear();
  return true;
}

bool Parser::parse(const InputData& inputData, const String& outputFile, OutputData& outputData)
{
  _outputData = &outputData;

  if (outputFile.endsWith(".htm") || outputFile.endsWith(".html"))
    outputData.format = OutputData::htmlFormat;
  else
    outputData.format = OutputData::texFormat;

  outputData.className = inputData.className;
  outputData.variables = inputData.variables;
  for(HashMap<String, InputData::Environment>::Iterator i = inputData.environments.begin(), end = inputData.environments.end(); i != end; ++i)
  {
    OutputData::EnvironmentInfo& environmentInfo = outputData.environments.append(i.key(), OutputData::EnvironmentInfo());
    environmentInfo.verbatim = i->verbatim;
    environmentInfo.command = i->command;
  }

  for(List<String>::Iterator i = inputData.headerTexFiles.begin(), end = inputData.headerTexFiles.end(); i != end; ++i)
  {
    String value = *i;
    for(HashMap<String, String>::Iterator i = outputData.variables.begin(), end = outputData.variables.end(); i != end; ++i)
      value.replace(String("%") + i.key() + "%", TexGenerator::texTranslate(*i));
    outputData.headerTexFiles.append(value);
  }

  if(outputData.className.isEmpty())
  {
    outputData.environments.append("boxed", OutputData::EnvironmentInfo()).verbatim = false;
    outputData.environments.append("plain", OutputData::EnvironmentInfo()).verbatim = true;
    outputData.environments.append("xplain", OutputData::EnvironmentInfo()).verbatim = true;
    for(usize i = 0; i < TexGenerator::_numOfDefaultListingsLanguages; ++i)
      outputData.environments.append(TexGenerator::getEnvironmentName(String::fromCString(TexGenerator::_defaultListingsLanguages[i])), OutputData::EnvironmentInfo()).verbatim = true;
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
          value.replace(String("%") + i.key() + "%", TexGenerator::texTranslate(*i));
        _outputSegments.append(new OutputData::TexSegment(value));
      }
      break;
    case InputData::Component::texTableOfContentsType:
      _outputSegments.append(new OutputData::TexSegment("\\tableofcontents"));
      break;
    case InputData::Component::texListOfFiguresType:
      _outputSegments.append(new OutputData::TexSegment("\\listoffigures"));
      break;
    case InputData::Component::texListOfTablesType:
      _outputSegments.append(new OutputData::TexSegment("\\listoftables"));
      break;
    case InputData::Component::texNewPageType:
      _outputSegments.append(new OutputData::TexSegment("\\clearpage"));
      break;
    case InputData::Component::texPartType:
      _outputSegments.append(new OutputData::TexPartSegment(component.value));
      break;
    case InputData::Component::pdfType:
      _outputSegments.append(new OutputData::PdfSegment(component.filePath));
      _outputData->hasPdfSegments = true;
      break;
    case InputData::Component::mdType:
      _segments.clear();
      if(!parseMarkdown(component.filePath, component.value))
        return false;
      break;
    }
  }
  outputData.segments.swap(_outputSegments);
  return true;
}
