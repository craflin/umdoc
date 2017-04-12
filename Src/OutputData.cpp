
#include "OutputData.h"

OutputData::BulletListSegment::~BulletListSegment()
{
  for(List<BulletListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
    delete *i;
}

OutputData::NumberedListSegment::~NumberedListSegment()
{
  for(List<NumberedListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
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

OutputData::EnvironmentSegment::~EnvironmentSegment()
{
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    delete *i;
}

OutputData::TableSegment::~TableSegment()
{
  for(List<RowData>::Iterator i = rows.begin(), end = rows.end(); i != end; ++i)
  {
    RowData& rowData = *i;
    for(Array<CellData>::Iterator i = rowData.cellData.begin(), end = rowData.cellData.end(); i != end; ++i)
    {
      CellData& cellData = *i;
      for(List<Segment*>::Iterator i = cellData.segments.begin(), end = cellData.segments.end(); i != end; ++i)
        delete *i;
    }
  }
  delete captionSegment;
}

OutputData::~OutputData()
{
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    delete *i;
}
