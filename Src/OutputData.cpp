
#include "OutputData.h"

OutputData::TitleSegment::TitleSegment(int_t indent, const String& line) : Segment(indent), level(1)
{
  const char_t* i = (const char_t* )line + 1;
  for(; *i == '#'; ++i, ++level);
  for(const char_t* end = (const char_t*)line + line.length(); i < end && String::isSpace(*i); ++i);
  title.attach(i, line.length() - (i - (const char_t* )line));
}

OutputData::TitleSegment::TitleSegment(int_t indent, int_t level, const String& title) : Segment(indent), level(level), title(title) {}

OutputData::ListSegment::~ListSegment()
{
  for(List<ListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
    delete *i;
}

OutputData::~OutputData()
{
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    delete *i;
}
