
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
     !file.write(Generator::generate(*this, outputData)) ||
     !file.write(String("</body>\n")) ||
     !file.write(String("</html>\n")))
    return false;

  return true;
}

String HtmlGenerator::getErrorString() const
{
  return Error::getErrorString();
}

String HtmlGenerator::generate(const OutputData::ParagraphSegment& segment)
{
  return String("<p>") + Generator::escape(*this, segment.text) + "</p>\n";
}

String HtmlGenerator::generate(const OutputData::TitleSegment& segment)
{
  if (segment.level >= 1 && segment.level <= 6)
  {
    String levelStr = String::fromInt(segment.level);
    return String("<h") + levelStr + ">" + Generator::escape(*this, segment.title) + "</h" + levelStr + ">\n";
  }
  return String("<p><strong>") + Generator::escape(*this, segment.title) + "</strong></p>\n";
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

String HtmlGenerator::escapeChar(const char c)
{
  switch (c)
  {
  case '"':
    return "&quot;";
  case '&':
    return "&amp;";
  case '\'':
    return "&apos;";
  case '<':
    return "&lt;";
  case '>':
    return "&gt;";
  default:
    return String(&c, 1);
  }
}

String HtmlGenerator::getSpanStart(const String& sequence)
{
  if(sequence.startsWith("`"))
    return "<code>";
  if(sequence == "**" || sequence == "__")
    return "<strong>";
  return "<em>";
}

String HtmlGenerator::getSpanEnd(const String& sequence)
{
  if(sequence.startsWith("`"))
    return "</code>";
  if(sequence == "**" || sequence == "__")
    return "</strong>";
  return "</em>";
}

String HtmlGenerator::getWordBreak(const char l, const char r)
{
  return String();
}
