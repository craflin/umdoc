
#include "OutputData.h"


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
    delete &segment;
    return true;
  }
  return false;
}

OutputData::TitleSegment::TitleSegment(int_t indent, const String& line) : Segment(indent), level(1)
{
  const char_t* i = (const char_t* )line + 1;
  for(; *i == '#'; ++i, ++level);
  for(const char_t* end = (const char_t*)line + line.length(); i < end && String::isSpace(*i); ++i);
  title.attach(i, line.length() - (i - (const char_t* )line));
}

OutputData::ListSegment::~ListSegment()
{
  for(List<ListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
    delete *i;
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

OutputData::~OutputData()
{
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    delete *i;
}
