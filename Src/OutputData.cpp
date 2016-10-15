
#include "OutputData.h"

OutputData::ListSegment::~ListSegment()
{
  for(List<ListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
    delete *i;
}

OutputData::BlockquoteSegment::~BlockquoteSegment()
{
  for(List<BlockquoteSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
    delete *i;
}

OutputData::~OutputData()
{
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    delete *i;
}
