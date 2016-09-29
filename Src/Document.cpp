
#include "Document.h"

bool_t Document::addLine(const String& line, size_t offset)
{
begin:
  int_t indent = 0;
  Segment* segment = 0;
  const char_t* p = line;
  for(p += offset; *p == ' '; ++p);
  indent = p - (const char_t*)line;
  String remainingLine;
  remainingLine.attach(p, line.length() - (p - (const char_t*)line));
  switch(*p)
  {
  case '#':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; *i == '#'; ++i)
      if(i < end && String::isSpace(*i))
        segment = new TitleSegment(indent, remainingLine);
    }
    break;
  case '=':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; i < end && (String::isSpace(*i) || *i == '='); ++i);
      if(i == end)
        segment = new RuleSegment(indent);
    }
    break;
  case '*':
    {
      const char_t* i = remainingLine;
      const char_t* end = i + remainingLine.length();
      for(; i < end && (String::isSpace(*i) || *i == '*'); ++i);
      if(i == end)
        segment = new RuleSegment(indent);
      else if(p + 1 < end && String::isSpace(*(p + 1)))
      {
        for(i = p + 2; i < end && String::isSpace(*i); ++i);
        int_t childIndent = i - (const char_t*)line;
        segment = new ListSegment(indent, childIndent);
        addSegment(*segment);
        offset = i - (const char_t*)line;
        goto begin;
      }
    }
    break;
  case '\r':
  case '\n':
  case '\0':
    segment = new SeparatorSegment(indent);
    break;
  default: ;
  }

  if(!segment)
    segment = new ParagraphSegment(indent, remainingLine);

  addSegment(*segment);
  return true;
}

void_t Document::addSegment(Segment& segment)
{
  if(segments.isEmpty() || !segments.back()->merge(segment))
    segments.append(&segment);
}

void_t Document::ParagraphSegment::print()
{
  // todo 
}

bool_t Document::ParagraphSegment::merge(Segment& segment)
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

void_t Document::SeparatorSegment::print()
{
  // todo
}

bool_t Document::SeparatorSegment::merge(Segment& segment)
{
  if(dynamic_cast<SeparatorSegment*>(&segment))
  {
    delete &segment;
    return true;
  }
  return false;
}

Document::TitleSegment::TitleSegment(int_t indent, const String& line) : Segment(indent), level(1)
{
  const char_t* i = (const char_t* )line + 1;
  for(; *i == '#'; ++i, ++level);
  for(const char_t* end = (const char_t*)line + line.length(); i < end && String::isSpace(*i); ++i);
  title.attach(i, line.length() - (i - (const char_t* )line));
}

void_t Document::TitleSegment::print()
{
  // todo
}

void_t Document::RuleSegment::print()
{
  // todo
}

Document::ListSegment::~ListSegment()
{
  for(List<ListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
    delete *i;
}

void_t Document::ListSegment::print()
{
  // todo
}

bool_t Document::ListSegment::merge(Segment& segment)
{
  ListSegment* listSegment = dynamic_cast<ListSegment*>(&segment);
  if(listSegment && listSegment->getIndent() == indent)
  {
    if(parent)
      segment.setParent(*parent);
    siblingSegments.append(listSegment);
    return true;
  }
  if(segment.getIndent() == childIndent)
  {
    segment.setParent(*this);
    childSegments.append(&segment);
    return true;
  }
  if(parent)
    return parent->merge(segment);
  return false;
}

Document::~Document()
{
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    delete *i;
}

String Document::generate() const
{
  // todo
  return String();
}
