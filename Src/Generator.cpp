
#include "Generator.h"

#include <nstd/Unicode.hpp>

String Generator::translate(Generator& generator, const String& str)
{
  String result(str.length());
  char c;
  String endSequence;
  List<String> endSequenceStack;
  bool ignoreSingleBacktick = false;
  for(const char* start = str, * i = start, * end = start + str.length(); i < end;)
  {
    switch(c = *i)
    {
    case '\\':
      if(i + 1 < end && String::find("\\`*_{}[]()#+-.!$%", *(i + 1)))
        ++i;
      result.append(generator.escapeChar(*i));
      ++i;
      break;
    default:
      if(!endSequence.isEmpty() && String::compare(i, endSequence, endSequence.length()) == 0)
      {
        if(*(const char*)endSequence == '*' || *(const char*)endSequence == '_')
        {
          if((String::find(" \t", i[endSequence.length()]) && (i == start || String::find(" \t", i[-1]))) ||
            (*(const char*)endSequence == '_' && String::isAlphanumeric(i[endSequence.length()])))
          { // "[...] if you surround an * or _ with spaces, it will be treated as a literal asterisk or underscore."
            for(usize j = 0; j < endSequence.length(); ++j)
              result.append(generator.escapeChar(*(const char*)endSequence));
            i += endSequence.length();
            continue;
          }
        }

        if(*(const char*)endSequence == '`')
          while(!result.isEmpty() && String::find(" \t", ((const char*)result)[result.length() - 1]))
            result.resize(result.length() - 1);

        result.append(generator.getSpanEnd(endSequence));
        i += endSequence.length();
        if(endSequence == "``")
          ignoreSingleBacktick = false;
        if(endSequenceStack.isEmpty())
          endSequence.clear();
        else
          endSequence = endSequenceStack.back(), endSequenceStack.removeBack();
        continue;
      }
      if(c == '*' || c == '_' || c == '`')
      {
        String sequence;
        sequence.attach(i, i[1] == c ? 2 : 1);

        if(c == '*' || c == '_')
        {
          if((String::find(" \t", i[sequence.length()]) && (i == start || String::find(" \t", i[-1]))) ||
            (c == '_' && i != start && String::isAlphanumeric(i[-1])))
          { // "[...] if you surround an * or _ with spaces, it will be treated as a literal asterisk or underscore."
            for(usize j = 0; j < sequence.length(); ++j)
              result.append(generator.escapeChar(c));
            i += sequence.length();
            continue;
          }
        }

        if(c == '`' && ignoreSingleBacktick)
        {
          result.append(generator.escapeChar(c));
          ++i;
          continue;
        }

        i += sequence.length();
        if(c == '`')
        {
          result.append(generator.getSpanStart(sequence));
          if(sequence.length() > 1)
            ignoreSingleBacktick = true;
          while(String::find(" \t", *i))
            ++i;
        }
        else
          result.append(generator.getSpanStart(sequence));
        if(!endSequence.isEmpty())
          endSequenceStack.append(endSequence);
        endSequence = sequence;
        continue;
      }
      if(matchInlineLink(generator, i, end, i, result))
        continue;
      if(matchInlineImage(generator, i, end, i, result))
        continue;
      if(matchLineBreak(generator, i, end, i, result))
        continue;
      if(matchInlineFootnote(generator, i, end, i, result))
        continue;
      if(matchInlineLatexFormula(generator, i, end, i, result))
        continue;
      if(c == ':' && String::isAlpha(i[1]) && i > start && !String::isAlphanumeric(i[-1]))
        result.append(generator.escapeChar(c) + generator.getWordBreak(c, i[1]));  // allow line breaks after e.g. "::"
      else if(String::isAlpha(c) && String::isLowerCase(c) && String::isAlpha(i[1]) && String::isUpperCase(i[1]))
        result.append(generator.escapeChar(c) + generator.getWordBreak(c, i[1]));  // allow line breaks in camel case
      else if(String::find("<({[", c) && i > start && String::isAlphanumeric(i[-1]))
        result.append(generator.getWordBreak(i[-1], c) + generator.escapeChar(c));  // allow line breaks before <, (, { or [
      else
      {
        uint32 c = Unicode::fromString(i, end - i);
        result.append(generator.escapeChar(c));
        i += Unicode::length(*i);
        continue;
      }
      ++i;
      break;
    }
  }
  while(!endSequenceStack.isEmpty())
  {
    result.append(generator.getSpanEnd(endSequenceStack.back()));
    endSequenceStack.removeBack();
  }
  if(!endSequence.isEmpty())
    result.append(generator.getSpanEnd(endSequence));
  return result;
}

String Generator::generate(Generator& generator, const OutputData& data)
{
  String result(data.segments.size() * 256);
  for(List<OutputData::Segment*>::Iterator i = data.segments.begin(), end = data.segments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append(segment->generate(generator));
  }
  return result;
}

bool Generator::matchInlineLink(Generator& generator, const char* s, const char* end, const char*& pos, String& result)
{
  if(*s != '[')
    return false;
  const char* nameStart = ++s;
  while(*s != ']')
    if(++s >= end)
      return false;
  const char* nameEnd = s++;
  if(*s != '(')
    return false;
  const char* linkStart = ++s;
  const char* linkEnd = 0;
  while(*s != ')')
  {
    if(*s == ' ' && !linkEnd)
      linkEnd = s;
    if(++s >= end)
      return false;
  }
  if(!linkEnd)
    linkEnd = s;
  ++s;
  String link;
  link.attach(linkStart, linkEnd - linkStart);
  String name;
  name.attach(nameStart, nameEnd - nameStart);
  result.append(generator.getLink(link, name));
  pos = s;
  return true;
}

bool Generator::matchInlineImage(Generator& generator, const char* s, const char* end, const char*& pos, String& result)
{
  if(*s != '!')
    return false;
  if(*(++s) != '[')
    return false;
  ++s;
  while(*s != ']')
    if(++s >= end)
      return false;
  if(*(++s) != '(')
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
  ++s;
  String path;
  path.attach(pathStart, pathEnd - pathStart);
  result.append(generator.getInlineImage(path));
  pos = s;
  return true;
}

bool Generator::matchLineBreak(Generator& generator, const char* s, const char* end, const char*& pos, String& result)
{
  if(*(s++) != '<')
    return false;
  while(String::isSpace(*s) && s < end)
    ++s;
  if(*(s++) != 'b')
    return false;
  if(*(s++) != 'r')
    return false;
  while(String::isSpace(*s) && s < end)
    ++s;
  if(*(s++) != '/')
    return false;
  if(*(s++) != '>')
    return false;
  pos = s;
  result.append(generator.getLineBreak());
  return true;
}

bool Generator::matchInlineFootnote(Generator& generator, const char* s, const char* end, const char*& pos, String& result)
{
  if(*(s++) != '[')
    return false;
  if(*(s++) != '^')
    return false;
  const char* textStart = s;
  while(*s != ']')
  {
    if(++s >= end)
      return false;
  }
  const char* textEnd = s++;
  String text;
  text.attach(textStart, textEnd - textStart);
  pos = s;
  result.append(generator.getFootnote(text));
  return true;
}

bool Generator::matchInlineLatexFormula(Generator& generator, const char* s, const char* end, const char*& pos, String& result)
{
  if(*(s++) != '$')
    return false;
  const char* formulaStart = s;
  while(*s != '$')
  {
    if(++s >= end)
      return false;
  }
  const char* formulaEnd = s++;
  String formula;
  formula.attach(formulaStart, formulaEnd - formulaStart);
  pos = s;
  result.append(generator.getLatexFormula(formula));
  return true;
}
