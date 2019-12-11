
#include "HtmlGenerator.h"
#include "OutputData.h"

#include <nstd/Error.h>
#include <nstd/File.h>

bool HtmlGenerator::generate(const OutputData& outputData, const String& outputFile)
{
  File file;
  if(!file.open(outputFile, File::writeFlag))
    return false;

  if(!file.write(String("<html>\n")))
    return false;

  if(!file.write(String("<body>\n")) ||
     !file.write(generate(outputData)) ||
     !file.write(String("</body>\n")) ||
     !file.write(String("</html>\n")))
    return false;

  return true;
}

String HtmlGenerator::getErrorString() const
{
  return Error::getErrorString();
}

String HtmlGenerator::generate(const OutputData& data)
{
  String result(data.segments.size() * 256);
  for(List<OutputData::Segment*>::Iterator i = data.segments.begin(), end = data.segments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append(segment->generate(*this));
  }
  return result;
}

String HtmlGenerator::generate(const OutputData::ParagraphSegment& segment)
{
  return String("<p>") + segment.text + "</p>\n";
}

String HtmlGenerator::generate(const OutputData::TitleSegment& segment)
{
  if (segment.level >= 1 && segment.level <= 6)
  {
    String levelStr = String::fromInt(segment.level);
    return String("<h") + levelStr + ">" + segment.title + "</h" + levelStr + ">\n";
  }
  return String("<p><b>") + segment.title + "</b></p>\n";
}

String HtmlGenerator::generate(const OutputData::SeparatorSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::FigureSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::RuleSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::BulletListSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::NumberedListSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::BlockquoteSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::EnvironmentSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::TableSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::TexSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::TexPartSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::PdfSegment& segment)
{
  return String();
}
