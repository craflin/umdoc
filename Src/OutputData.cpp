
#include "OutputData.h"
#include "Generator.h"

OutputData::BulletListSegment::~BulletListSegment()
{
  for(List<BulletListSegment*>::Iterator i = _siblingSegments.begin(), end = _siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = _childSegments.begin(), end = _childSegments.end(); i != end; ++i)
    delete *i;
}

OutputData::NumberedListSegment::~NumberedListSegment()
{
  for(List<NumberedListSegment*>::Iterator i = _siblingSegments.begin(), end = _siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = _childSegments.begin(), end = _childSegments.end(); i != end; ++i)
    delete *i;
}

OutputData::BlockquoteSegment::~BlockquoteSegment()
{
  for(List<BlockquoteSegment*>::Iterator i = _siblingSegments.begin(), end = _siblingSegments.end(); i != end; ++i)
    delete *i;
  for(List<Segment*>::Iterator i = _childSegments.begin(), end = _childSegments.end(); i != end; ++i)
    delete *i;
}

OutputData::EnvironmentSegment::~EnvironmentSegment()
{
  for(List<Segment*>::Iterator i = _segments.begin(), end = _segments.end(); i != end; ++i)
    delete *i;
}

OutputData::TableSegment::~TableSegment()
{
  for(List<RowData>::Iterator i = _rows.begin(), end = _rows.end(); i != end; ++i)
  {
    RowData& rowData = *i;
    for(Array<CellData>::Iterator i = rowData.cellData.begin(), end = rowData.cellData.end(); i != end; ++i)
    {
      CellData& cellData = *i;
      for(List<Segment*>::Iterator i = cellData.segments.begin(), end = cellData.segments.end(); i != end; ++i)
        delete *i;
    }
  }
  delete _captionSegment;
}

OutputData::~OutputData()
{
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    delete *i;
}

String OutputData::ParagraphSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TitleSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::SeparatorSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::FigureSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::RuleSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::BulletListSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::NumberedListSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::BlockquoteSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::EnvironmentSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TableSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TexSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TexPartSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::PdfSegment::generate(Generator& generator) const {return generator.generate(*this);}
