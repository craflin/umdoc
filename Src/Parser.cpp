
#include "Parser.hpp"

#include <nstd/File.hpp>
#include <nstd/Directory.hpp>
#include <nstd/Unicode.hpp>
#include <nstd/Memory.hpp>
#include <nstd/Process.hpp>
#include <nstd/Error.hpp>
#include <nstd/Thread.hpp>

#include "InputData.hpp"
#include "OutputData.hpp"
#include "TexGenerator.hpp"

void Parser::addSegment2(const RefCount::Ptr<OutputData::Segment>& newSegment, bool newLine, const String& data)
{
    OutputData::SeparatorSegment* isSeparator = dynamic_cast<OutputData::SeparatorSegment*>(&*newSegment);
    bool newParagraph = _newParagraphNextLine;
    _newParagraphNextLine = isSeparator != nullptr;

    bool merged = false;
    if (!_segments.isEmpty())
    {
        OutputData::Segment* lastSegment = &*_segments.back();
        OutputData::SeparatorSegment* lastSeparator = dynamic_cast<OutputData::SeparatorSegment*>(lastSegment);
        if (lastSeparator && isSeparator)
            merged = lastSegment->merge(*newSegment, false, newLine, data);
        else
        {
            bool lastIsSeparator = lastSeparator != nullptr;
            if (lastIsSeparator && _segments.size() > 1)
                lastSegment = &**--(--_segments.end());
            for (OutputData::Segment* segment = lastSegment;;)
            {
                if (segment->merge(*newSegment, newParagraph, newLine, data))
                {
                    merged = true;
                    break;
                }
                segment = segment->_parent;
                if (!segment)
                    break;
            }
        }
    }

    if (!merged)
        _outputSegments.append(&*newSegment);
    else if (!_segments.back()->isValid())
        _segments.removeBack();

    if (newSegment->isValid())
        _segments.append(newSegment);
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

bool Parser::parseMarkdownTableLine(int indent, bool newLine, const String& data)
{
    RefCount::Ptr<OutputData::TableSegment> tableSegment = new OutputData::TableSegment(indent);
    if(!tableSegment->parseArguments(data, _error.string))
      return false;
    addSegment2(tableSegment, newLine, data);
    return true;
}

bool Parser::parseMarkdownLine(const String& line, int additionalIndent)
{
  RefCount::Ptr<OutputData::Segment> segment;
  const char* p = line;
  for(; *p == ' '; ++p);
  int indent = additionalIndent + (int)(p - (const char*)line);
  String remainingLine(p, line.length() - (p - (const char*)line)); // todo: rename to segmentData
  p = remainingLine;
  bool newLine = additionalIndent == 0;

  if(_parserMode != normalMode)
  {
    OutputData::EnvironmentSegment* environmentSegment = (OutputData::EnvironmentSegment*)&*_segments.back();
    if(String::compare(p, "```", 3) == 0)
    {
      const char* i = p +3;
      while(*i == '`')
        ++i;
      int backticks = (int)(i - p);
      if(backticks >= environmentSegment->_backticks)
      {
        _parserMode = normalMode;
        return true;
      }
    }
    environmentSegment->_lines.append(line);
    return true;
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
        int titleLevel = (int)(i - ( const char*)remainingLine);
        ++i;
        String title;
        title.attach(i, remainingLine.length() - (i - (const char*)remainingLine));
        RefCount::Ptr<OutputData::TitleSegment> titleSegment = new OutputData::TitleSegment(indent, titleLevel);
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
        OutputData::ParagraphSegment* paragraphSegment;
        if(!_segments.isEmpty() && (paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(&*_segments.back())) && paragraphSegment->getIndent() == indent)
        {
          paragraphSegment->invalidate();
          RefCount::Ptr<OutputData::TitleSegment> titleSegment = new OutputData::TitleSegment(indent, 1);
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
        int childIndent = indent + (int)(i - p);
        RefCount::Ptr<OutputData::BulletListSegment> listSegment = new OutputData::BulletListSegment(indent, '*', childIndent);
        usize offset = i - (const char*)remainingLine;
        String data2(remainingLine, offset);
        remainingLine = String(i, remainingLine.length() - offset);
        addSegment2(listSegment, newLine, data2);
        return parseMarkdownLine(remainingLine, childIndent);
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
        OutputData::ParagraphSegment* paragraphSegment;
        if(!_segments.isEmpty() && (paragraphSegment = dynamic_cast<OutputData::ParagraphSegment*>(&*_segments.back())) && paragraphSegment->getIndent() == indent)
        {
          paragraphSegment->invalidate();
          RefCount::Ptr<OutputData::TitleSegment> titleSegment = new OutputData::TitleSegment(indent, 2);
          if(!titleSegment->parseArguments(paragraphSegment->_text, _error.string))
            return false;
          segment = titleSegment;
          break;
        }
      }
      i = remainingLine;
      for(; i < end && (*i == '-' || String::isSpace(*i)); ++i);
      if(i == end && remainingLine.length() >= 3)
      {
        segment = new OutputData::RuleSegment(indent);
        break;
      }
      if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int childIndent = indent + (int)(i - p);
        RefCount::Ptr<OutputData::BulletListSegment> listSegment = new OutputData::BulletListSegment(indent, '-', childIndent);
        usize offset = i - (const char*)remainingLine;
        String data2(remainingLine, offset);
        remainingLine = String(i, remainingLine.length() - offset);
        addSegment2(listSegment, newLine, data2);
        return parseMarkdownLine(remainingLine, childIndent);
      }
    }
    break;
  case '+':
    if(p[1] == '-' || p[1] == '=')
      return parseMarkdownTableLine(indent, newLine, remainingLine);
    else
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int childIndent = indent + (int)(i - p);
        RefCount::Ptr<OutputData::BulletListSegment> listSegment = new OutputData::BulletListSegment(indent, '+', childIndent);
        usize offset = i - (const char*)remainingLine;
        String data2(remainingLine, offset);
        remainingLine = String(i, remainingLine.length() - offset);
        addSegment2(listSegment, newLine, data2);
        return parseMarkdownLine(remainingLine, childIndent);
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
      RefCount::Ptr<OutputData::EnvironmentSegment> environmentSegment = new OutputData::EnvironmentSegment(indent, backticks);
      if(!environmentSegment->parseArguments(remainingLine, _outputData->environments, _error.string))
        return false;
      segment = environmentSegment;
      if(environmentSegment->_verbatim || !environmentSegment->_command.isEmpty())
        _parserMode = verbatimEnvironmentMode;
      else
        _parserMode = environmentMode;
      break;
    }
    break;
  case '>':
    if(String::isSpace(p[1]))
    {
      RefCount::Ptr<OutputData::BlockquoteSegment> blockquoteSegment = new OutputData::BlockquoteSegment(indent, indent + 2);
      p += 2;
      usize offset = p - (const char*)remainingLine;
      String data2(remainingLine, offset);
      remainingLine = String(p, remainingLine.length() - offset);
      addSegment2(blockquoteSegment, newLine, data2);
      return parseMarkdownLine(remainingLine, indent + (int)offset);
    }
    break;
  case '!':
    {
      const char* i = remainingLine;
      const char* end = i + remainingLine.length();
      String title, path, remainingLine;
      if(matchFigureImage(i, end, title, path, remainingLine))
      {
        RefCount::Ptr<OutputData::FigureSegment > figureSegment = new OutputData::FigureSegment(indent, title, path);
        if(!figureSegment->parseArguments(remainingLine, _error.string))
          return false;
        segment = figureSegment;
      }
    }
    break;
  case '|':
    return parseMarkdownTableLine(indent, newLine, remainingLine);
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
        int childIndent = indent + (int)(i - p);
        String numberStr;
        RefCount::Ptr<OutputData::NumberedListSegment> numberedListSegment = new OutputData::NumberedListSegment(indent, remainingLine.toUInt(), childIndent);
        usize offset = i - (const char*)remainingLine;
        String data2(remainingLine, offset);
        remainingLine = String(i, remainingLine.length() - offset);
        addSegment2(numberedListSegment, newLine, data2);
        return parseMarkdownLine(remainingLine, childIndent);
      }
    }
  }

  if(!segment)
    segment = new OutputData::ParagraphSegment(indent, remainingLine);

  addSegment2(segment, newLine, remainingLine);
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
        if(_parserMode != verbatimEnvironmentMode)
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

bool OutputData::ParagraphSegment::merge(Segment& segment, bool newParagraph, bool newLine, const String& line)
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

bool OutputData::SeparatorSegment::merge(Segment& segment, bool newParagraph, bool newLine, const String& line)
{
  if(dynamic_cast<SeparatorSegment*>(&segment))
  {
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

bool OutputData::BulletListSegment::merge(Segment& segment, bool newParagraph, bool newLine, const String& line)
{
  BulletListSegment* listSegment = dynamic_cast<BulletListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == _indent)
  {
    if(_parent)
    {
      BulletListSegment* parentListSegment = dynamic_cast<BulletListSegment*>(_parent);
      if(parentListSegment && parentListSegment->getIndent() == _indent)
        return parentListSegment->merge(segment, newParagraph, newLine, line);
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

bool OutputData::NumberedListSegment::merge(Segment& segment, bool newParagraph, bool newLine, const String& line)
{
  NumberedListSegment* listSegment = dynamic_cast<NumberedListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == _indent)
  {
    if(_parent)
    {
      NumberedListSegment* parentListSegment = dynamic_cast<NumberedListSegment*>(_parent);
      if(parentListSegment && parentListSegment->getIndent() == _indent)
        return parentListSegment->merge(segment, newParagraph, newLine, line);
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

bool OutputData::BlockquoteSegment::merge(Segment& segment, bool newParagraph, bool newLine, const String& line)
{
  BlockquoteSegment* blockSegment = dynamic_cast<BlockquoteSegment*>(&segment);
  if(blockSegment && blockSegment->getIndent() == _indent)
  {
    if(_parent)
    {
      BlockquoteSegment* parentBlockquoteSegment = dynamic_cast<BlockquoteSegment*>(_parent);
      if(parentBlockquoteSegment && parentBlockquoteSegment->getIndent() == _indent)
        return parentBlockquoteSegment->merge(segment, newParagraph, newLine, line);
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
  if(!_command.isEmpty())
  {
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
    process.close(Process::stdinStream);
    if(readerThread.join() != 0)
      return error = "Could not join reader thread", false;
  }

  if(!_verbatim)
  {
    Parser parser;
    String fileContent;
    fileContent.join(_lines, '\n');
    if (!parser.parseMarkdown(_command, fileContent) ||
        !parser.process(format_))
      return error = parser.getErrorString(), false;
    _segments.swap(parser._outputSegments);
    _allocatedSegments.swap(parser._segments);
  }

  return true;
}

bool OutputData::TableSegment::merge(Segment& segment, bool newParagraph, bool newLine, const String& line)
{
  TableSegment* tableSegment = dynamic_cast<TableSegment*>(&segment);
  if(tableSegment && tableSegment->getIndent() == _indent && !newParagraph)
  {
    tableSegment->invalidate();

    // increase column count if necessary
    if(tableSegment->_columns.size() > _columns.size())
    {
      usize newColumnCount = tableSegment->_columns.size();
      for(List<RowData>::Iterator i = _rows.begin(), end = _rows.end(); i != end; ++i)
        i->cellData.resize(newColumnCount);
      for (usize i = _columns.size(); i < newColumnCount; ++i)
          _columns.append(tableSegment->_columns[i]);
    }

    // adopt additional alignment configuration and column arguments
    for(usize i = 0; i < tableSegment->_columns.size(); ++i)
    {
      ColumnInfo& columnInfo = _columns[i];
      ColumnInfo& additionalColumnInfo = tableSegment->_columns[i];
      if(columnInfo.alignment ==  ColumnInfo::undefinedAlignment && additionalColumnInfo.alignment != ColumnInfo::undefinedAlignment)
          columnInfo.alignment = additionalColumnInfo.alignment;
      columnInfo.indent = additionalColumnInfo.indent;
      columnInfo.arguments.insert(additionalColumnInfo.arguments);
    }

    // add cell data
    if (!tableSegment->_rows.isEmpty())
    {
      if (_rows.isEmpty() || _forceNewRowNextMerge || _tableType != TableSegment::GridTable)
      {
        RowData& newRow = _rows.append(RowData());
        newRow.cellData.resize(_columns.size());
        _forceNewRowNextMerge = false;
      }

      RowData& lastRow = _rows.back();
      RowData& newRowData = tableSegment->_rows.front();
      for (usize i = 0; i < newRowData.cellData.size(); ++i)
        lastRow.cellData[i].lines.append(newRowData.cellData[i].lines);
    }

    if (tableSegment->_isSeparatorLine)
      _forceNewRowNextMerge = true;

    return true;
  }

  usize column = 0;
  for(Array<ColumnInfo>::Iterator i = _columns.begin(), end = _columns.end(); i != end; ++i, ++column)
  {
    const ColumnInfo& columnInfo = *i;
    if(segment.getIndent() >= columnInfo.indent && (column == _columns.size() - 1 || segment.getIndent() < ((ColumnInfo*)_columns)[column + 1].indent))
    {
      if (_rows.isEmpty() || _forceNewRowNextMerge)
      {
        _forceNewRowNextMerge = false;
        _rows.append(RowData()).cellData.resize(_columns.size());
      }
      RowData& rowData = _rows.back();
      CellData& cellData = rowData.cellData[column];
      if (newParagraph && !cellData.lines.isEmpty())
        cellData.lines.append(String());
      if (newLine || cellData.lines.isEmpty())
        cellData.lines.append(String(segment.getIndent() - columnInfo.indent, ' ') + line);
      else 
        cellData.lines.back().append(line);
      segment.invalidate();
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

bool OutputData::TableSegment::parseArguments(const String& line, String& error)
{
  RowData& row = _rows.append(RowData());
  const char* start = line;
  char separatorChar = *start;
  if (separatorChar == '+')
    _tableType = GridTable;
  const char* end = start + line.length();
  for(const char* i = start; i < end; ++i)
    if(*i == separatorChar)
    {
      ++i;
      const char* columStart = i;
      while(i < end && String::isSpace(*i))
        ++i;
      if(i >= end)
        break;
      for(; i < end && *i != separatorChar; ++i)
        if(*i == '\\' && i[1] == separatorChar)
          ++i;
      const char* columEnd = i;

      ColumnInfo& columnInfo = _columns.append(ColumnInfo(_indent + (int)(columStart - start)));
      String text;
      text.attach(columStart, columEnd - columStart);
      Parser::extractArguments(text, columnInfo.arguments);

      if(columEnd == end)
      {
        text.trim();
        if(text.isEmpty())
        {
          _arguments.insert(columnInfo.arguments);
          _columns.removeBack();
          break;
        }
      }
      CellData& cellData = row.cellData.append(CellData());
      cellData.lines.append(text);

      --i;
    }

  _isSeparatorLine = true;
  bool foundSeparatorLineChar = false;
  for(Array<CellData>::Iterator i = row.cellData.begin(), end = row.cellData.end(); i != end; ++i)
  {
    const CellData& cellData = *i;
    usize index = &*i - &*row.cellData.begin();
    const String& text = cellData.lines.back();
    for(const char* i = text, * end = i + text.length(); i < end; ++i)
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
      while((*i == '-' || (_tableType == GridTable && *i == '=')) && i < end)
      {
        foundSeparatorLineChar = true;
        ++i;
      }
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
          _columns[index].alignment = ColumnInfo::centerAlignment;
      else if(rightTick)
          _columns[index].alignment = ColumnInfo::rightAlignment;
      else if(leftTick)
          _columns[index].alignment = ColumnInfo::leftAlignment;
    }
    if(!_isSeparatorLine)
      break;
  }
  if (!foundSeparatorLineChar)
    _isSeparatorLine = false;
  if (_isSeparatorLine)
    _rows.clear();
  return true;
}

bool OutputData::TableSegment::process(OutputData::OutputFormat format, String& error)
{
  for (List<RowData>::Iterator i = _rows.begin(), end = _rows.end(); i != end; ++i)
  {
    RowData& row = *i;
    for (Array<CellData>::Iterator i = row.cellData.begin(), end = row.cellData.end(); i != end; ++i)
    {
      CellData& cellData = *i;
      Parser parser;
      String fileContent;
      fileContent.join(cellData.lines, '\n');
      if (!parser.parseMarkdown(String(), fileContent) ||
          !parser.process(format))
        return error = parser.getErrorString(), false;
      cellData.outputSegments2.swap(parser._outputSegments);
      cellData.allocatedSegments.append(parser._segments);
    }
  }
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
  for(HashMap<String, InputData::Environment>::Iterator i = inputData.environments.begin(), end = inputData.environments.end(); i != end; ++i)
  {
    OutputData::EnvironmentInfo& environmentInfo = outputData.environments.append(i.key(), OutputData::EnvironmentInfo());
    environmentInfo.verbatim = i->verbatim;
    environmentInfo.command = i->command;
  }

  for(List<String>::Iterator i = inputData.headerTexFiles.begin(), end = inputData.headerTexFiles.end(); i != end; ++i)
    outputData.headerTexFiles.append(replacePlaceholderVariables(*i, inputData.variables, false));

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
    RefCount::Ptr<OutputData::Segment> segment;
    switch(component.type)
    {
    case InputData::Component::texType:
        segment = new OutputData::TexSegment(replacePlaceholderVariables(component.value, inputData.variables, false));
      break;
    case InputData::Component::texTableOfContentsType:
        segment = new OutputData::TexSegment("\\tableofcontents");
      break;
    case InputData::Component::texListOfFiguresType:
        segment = new OutputData::TexSegment("\\listoffigures");
      break;
    case InputData::Component::texListOfTablesType:
        segment = new OutputData::TexSegment("\\listoftables");
      break;
    case InputData::Component::texNewPageType:
        segment = new OutputData::TexSegment("\\clearpage");
      break;
    case InputData::Component::texPartType:
        segment = new OutputData::TexPartSegment(component.value);
      break;
    case InputData::Component::pdfType:
        segment = new OutputData::PdfSegment(component.filePath);
      _outputData->hasPdfSegments = true;
      break;
    case InputData::Component::mdType:
      _segments.append(new OutputData::SeparatorSegment(0));
      _segments.append(new OutputData::SeparatorSegment(0));
      if(!parseMarkdown(component.filePath, replacePlaceholderVariables(component.value, inputData.variables, true)))
        return false;
      break;
    }
    if (segment)
    {
        _outputSegments.append(&*segment);
        _segments.append(segment);
    }
  }

  if (!process(outputData.format))
    return false;

  outputData.segments.swap(_outputSegments);
  outputData.allocatedSegments.swap(_segments);
  return true;
}

bool Parser::process(OutputData::OutputFormat format)
{
  for (List<OutputData::Segment*>::Iterator i = _outputSegments.begin(), end = _outputSegments.end(); i != end; ++i)
  {
    OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    if(!segment->process(format, _error.string))
      return false;
  }
  return true;
}


String Parser::replacePlaceholderVariables(const String& data, const HashMap<String, String>& variables, bool allowEscaping)
{
  String result;
  const char* start = data;
  const char* blockStart = start;
  const char* i = start;
  for(const char* end = i + data.length(); i < end; ++i)
  {
    switch(*i)
    {
    case '\\':
      if(allowEscaping && i + 1 < end && i[1] == '%')
      {
        ++i;
        continue;
      }
      break;
    case '%':
      {
        const char* varStart = ++i, *varEnd;
        for(;; ++i)
        {
          if(!*i || String::isSpace(*i))
            goto processNextChar;
          if(*i == '%')
          {
            varEnd = i;
            break;
          }
        }
        if(varStart != varEnd)
        {
          String variableName(varStart, varEnd - varStart);
          result.append(blockStart, (varStart - 1) - blockStart);
          blockStart = varEnd + 1;
          HashMap<String, String>::Iterator it = variables.find(variableName);
          if (it == variables.end())
            result.append(String("%") + variableName + "%");
          else
            result.append(*it);
        }
      }
    }
processNextChar:;
  }
  if(blockStart == start)
    return data;
  result.append(blockStart, i - blockStart);
  return result;
}

