
#include "HtmlGenerator.h"
#include "OutputData.h"
#include "PlainGenerator.h"

#include <nstd/Error.hpp>
#include <nstd/File.hpp>
#include <nstd/Console.hpp>
#include <nstd/Unicode.hpp>

void HtmlGenerator::findNumbers(const List<OutputData::Segment*>& segments, LastNumbers& lastNumbers)
{
  for(List<OutputData::Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    const OutputData::TitleSegment* titleSegment = dynamic_cast<const OutputData::TitleSegment*>(segment);
    if(titleSegment)
    {
      if(titleSegment->_level >= 5 ||
         titleSegment->_arguments.contains(".unnumbered"))
        continue;
      if((usize)titleSegment->_level > lastNumbers.title.size())
        lastNumbers.title.resize(titleSegment->_level, 1);
      else
      {
        lastNumbers.title.resize(titleSegment->_level);
        ++lastNumbers.title.back();
      }
      _titleNumbers.append(titleSegment, lastNumbers.title);
      String label = titleSegment->_arguments.find("#")->toString();
      if(!label.isEmpty())
        _numbers.append(label, lastNumbers.title);
    }
    const OutputData::FigureSegment* figureSegment = dynamic_cast<const OutputData::FigureSegment*>(segment);
    if(figureSegment)
    {
      _figureNumbers.append(figureSegment, ++lastNumbers.figure);
      String label = figureSegment->_arguments.find("#")->toString();
      if(!label.isEmpty())
        _numbers.append(label, lastNumbers.figure);
    }
    const OutputData::TableSegment* tableSegment = dynamic_cast<const OutputData::TableSegment*>(segment);
    if(tableSegment && tableSegment->_captionSegment)
    {
      _tableNumbers.append(tableSegment, ++lastNumbers.table);
      String label = tableSegment->_arguments.find("#")->toString();
      if(!label.isEmpty())
        _numbers.append(label, lastNumbers.table);
    }
    const OutputData::EnvironmentSegment* environmentSegment = dynamic_cast<const OutputData::EnvironmentSegment*>(segment);
    if(environmentSegment)
      findNumbers(environmentSegment->_segments, lastNumbers);
  }
}

bool HtmlGenerator::generate(const OutputData& outputData, const String& outputFile)
{
  _outputDir = File::dirname(outputFile);

  {
    LastNumbers lastNumbers;
    findNumbers(outputData.segments, lastNumbers);
  }

  File file;
  if(!file.open(outputFile, File::writeFlag))
    return false;

  if(!file.write(String("<html>\n")))
    return false;

  if(!file.write(String("<head>\n")) ||
     !file.write(String("<meta charset=\"UTF-8\">\n")) ||
     !file.write(String("<style>\n")) ||
     !file.write(String("code {background-color:whitesmoke;border:1px solid grey;padding:0px 2px 0px 2px;}\n")) ||
     !file.write(String(".environment {background-color:whitesmoke;border:1px solid grey;padding:0em 1em 0em 1em;}\n")) ||
     !file.write(String(".figure {text-align:center;}\n")) ||
     !file.write(String(".table {margin-left:auto;margin-right:auto;border-collapse:collapse;border-top:1px solid black;border-bottom:1px solid black;}\n")) ||
     !file.write(String(".th {border-bottom:1px solid black;padding-left:0.5em;padding-right:0.5em;font-weight: normal;}\n")) ||
     !file.write(String(".td {padding-left:0.5em;padding-right:0.5em;}\n")) ||
     !file.write(String(".table_grid {margin-left:auto;margin-right:auto;border-collapse:collapse;border:1px solid black;}\n")) ||
     !file.write(String(".th_grid {border:1px solid black;padding-left:0.5em;padding-right:0.5em;font-weight: normal;}\n")) ||
     !file.write(String(".td_grid {border:1px solid black;padding-left:0.5em;padding-right:0.5em;}\n")) ||
     !file.write(String("</style>\n")) ||
     !file.write(String("</head>\n")))
    return false;

  if(!file.write(String("<body>\n")) ||
     !file.write(Generator::generate(*this, outputData)))
     return false;

  if(!_footnotes.isEmpty())
  {
    if (!file.write("<hr/><p>\n"))
        return false;
    uint number = 1;
    for(List<String>::Iterator i = _footnotes.begin(), end = _footnotes.end(); i != end; ++i, ++number)
    {
      if(i != _footnotes.begin())
        if(!file.write("<br/>"))
          return false;
      if(!file.write(String("<sup>") + String::fromUInt(number) + "</sup> " + translate(*this, *i) + "\n"))
        return false;
    }
    if (!file.write("</p>\n"))
        return false;
  }

  if(!file.write(String("</body>\n")) ||
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
  return String("<p>") + Generator::translate(*this, segment._text) + "</p>\n";
}

String HtmlGenerator::generate(const OutputData::TitleSegment& segment)
{
  const Number& numberData = *_titleNumbers.find(&segment);
  String number = numberData.toString();
  String id = getElementId(segment, segment._title, segment._arguments);
  if(segment._level >= 1 && segment._level <= 6)
  {
    String levelStr = String::fromInt(segment._level);
    return String("<h") + levelStr + " id=\"" + id + "\">" + number + " " + Generator::translate(*this, segment._title) + "</h" + levelStr + ">\n";
  }
  return String("<p id=\"") + id + "\"><strong>" + Generator::translate(*this, segment._title) + "</strong></p>\n";
}

String HtmlGenerator::generate(const OutputData::SeparatorSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::FigureSegment& segment)
{
  String number = _figureNumbers.find(&segment)->toString();
  String id = getElementId(segment, "", segment._arguments);
  String basename = File::basename(segment._path);
  String outputImageFile = _outputDir + "/" + basename;
  if(!File::copy(segment._path, outputImageFile, false))
      Console::errorf("error: Could not copy file '%s' to '%s': %s\n", (const char*)segment._path, (const char*)outputImageFile, (const char*)Error::getErrorString());
  List<String> styleList;
  String width = segment._arguments.find("width")->toString();
  if(!width.isEmpty())
    styleList.append(String("width:") + width);
  String height = segment._arguments.find("height")->toString();
  if(!height.isEmpty())
    styleList.append(String("height:") + width);
  String style;
  style.join(styleList, ';');
  String result;
  result.append(String("<p class=\"figure\" id=\"") + id + "\">" + "<img src=\"" + basename + "\" alt=\"" + stripFormattingAndTranslate(segment._title) + "\" style=\"" + style + "\"/></p>");
  result.append(String("<p class=\"figure\">Figure ") + number + ": " + translate(*this, segment._title) + "</p>\n");
  return result;

}

String HtmlGenerator::generate(const OutputData::RuleSegment& segment)
{
  return String("<hr/>\n");
}

String HtmlGenerator::generate(const OutputData::BulletListSegment& segment)
{
  String result;
  result.append("<ul><li>");
  for(List<OutputData::Segment*>::Iterator i = segment._childSegments.begin(), end = segment._childSegments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append(segment->generate(*this));
  }
  result.append("</li>");
  for(List<OutputData::BulletListSegment*>::Iterator i = segment._siblingSegments.begin(), end = segment._siblingSegments.end(); i != end; ++i)
  {
    OutputData::BulletListSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    result.append("<li>");
    for(List<OutputData::Segment*>::Iterator i = siblingSegment->_childSegments.begin(), end = siblingSegment->_childSegments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
    result.append("</li>");
  }
  result.append("</ul>\n");
  return result;
}

String HtmlGenerator::generate(const OutputData::NumberedListSegment& segment)
{
  String result;
  result.append("<ol><li>");
  for(List<OutputData::Segment*>::Iterator i = segment._childSegments.begin(), end = segment._childSegments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append(segment->generate(*this));
  }
  result.append("</li>");
  for(List<OutputData::NumberedListSegment*>::Iterator i = segment._siblingSegments.begin(), end = segment._siblingSegments.end(); i != end; ++i)
  {
    OutputData::NumberedListSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    result.append("<li>");
    for(List<OutputData::Segment*>::Iterator i = siblingSegment->_childSegments.begin(), end = siblingSegment->_childSegments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
    result.append("</li>");
  }
  result.append("</ol>\n");
  return result;
}

String HtmlGenerator::generate(const OutputData::BlockquoteSegment& segment)
{
  String result;
  result.append("<blockquote>");
  for(List<OutputData::Segment*>::Iterator i = segment._childSegments.begin(), end = segment._childSegments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate(*this));
  }
  for(List<OutputData::BlockquoteSegment*>::Iterator i = segment._siblingSegments.begin(), end = segment._siblingSegments.end(); i != end; ++i)
  {
    OutputData::BlockquoteSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    for(List<OutputData::Segment*>::Iterator i = siblingSegment->_childSegments.begin(), end = siblingSegment->_childSegments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
  }
  result.append("</blockquote>\n");
  return result;
}

String HtmlGenerator::generate(const OutputData::EnvironmentSegment& segment)
{
  String result;
  result.append("<div class=\"environment\">");
  if(segment._verbatim)
  {
    if(segment._command.isEmpty())
      result.append("<pre>\n");
    for(List<String>::Iterator i = segment._lines.begin(), end = segment._lines.end(); i != end; ++i)
    {
      result.append(escape(*i));
      result.append("\n");
    }
    if(segment._command.isEmpty())
      result.append("</pre>");
  }
  else
  {
    for(List<OutputData::Segment*>::Iterator i = segment._segments.begin(), end = segment._segments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
  }
  result.append("</div>\n");
  return result;
}

String HtmlGenerator::generate(const OutputData::TableSegment& segment)
{
  String result;
  String id = getElementId(segment, "", segment._arguments);
  bool xtabGridStyle = segment._arguments.contains(".xtabgrid");
  bool gridStyle = segment._arguments.contains(".grid") || xtabGridStyle;
  String tableStyleSuffix;
  if(gridStyle)
    tableStyleSuffix = "_grid";
  result.append(String("<p id=\"") + id + "\"><table class=\"table" + tableStyleSuffix + "\">");
  for(List<OutputData::TableSegment::RowData>::Iterator i = segment._rows.begin(), end = segment._rows.end(); i != end; ++i)
  {
    result.append("<tr>");
    OutputData::TableSegment::RowData& rowData = *i;
    usize columnIndex = 0;
    bool firstRow = i == segment._rows.begin();
    for(Array<OutputData::TableSegment::CellData>::Iterator i = rowData.cellData.begin(), end = rowData.cellData.end(); i != end; ++i, ++columnIndex)
    {
      const OutputData::TableSegment::ColumnInfo& columnInfo = segment._columns[columnIndex];
      List<String> styleList;
      switch(columnInfo.alignment)
      {
      case OutputData::TableSegment::ColumnInfo::rightAlignment:
        styleList.append("text-align:right");
        break;
      case OutputData::TableSegment::ColumnInfo::centerAlignment:
        styleList.append("text-align:center");
        break;
      default:
        break;
      }
      String width = columnInfo.arguments.find("width")->toString();
      if(!width.isEmpty())
          styleList.append(String("width:") + width);
      String style;
      style.join(styleList, ';');
      
      OutputData::TableSegment::CellData& cellData = *i;
      String class_ = firstRow ? String("th") : String("td");
      result.append(String("<") + class_ + " class=\"" + class_ + tableStyleSuffix + "\" style=\"" + style + "\">");
      for(List<OutputData::TableSegment::Segment*>::Iterator i = cellData.outputSegments.begin(), end = cellData.outputSegments.end(); i != end; ++i)
      {
        OutputData::TableSegment::Segment* segment = *i;
        if(!segment->isValid())
          continue;
        result.append(segment->generate(*this));
      }
      if (firstRow)
        result.append("</th>\n");
      else
        result.append("</td>\n");
    }
    result.append("</tr>\n");
  }
  result.append("</table></p>\n");
  if(segment._captionSegment)
  {
    String caption = segment._captionSegment->_text;
    if(caption.startsWith(":"))
      caption = caption.substr(1);
    else // Table:
      caption = caption.substr(6);

    String number = _tableNumbers.find(&segment)->toString();
    result.append(String("<p class=\"figure\">Table ") + number + ": ");
    result.append(translate(*this, caption));
    result.append("</p>\n");
  }
  return result;
}

String HtmlGenerator::generate(const OutputData::TexSegment& segment)
{
  String result;
  if(segment._content == "\\tableofcontents")
  {
    result.append("<h1>Contents</h1>\n");
    result.append("<p>");
    result.append("<table>");
    int lastLevel = 1;
    for(HashMap<const OutputData::TitleSegment*, Number>::Iterator i = _titleNumbers.begin(), end = _titleNumbers.end(); i != end; ++i)
    {
      const OutputData::TitleSegment* segment = i.key();
      const Number& number = *i;
      if (segment->_level > 3)
        continue;
      for(; lastLevel < segment->_level; ++lastLevel)
        result.append("<tr><td></td><td><table>");
      for(; lastLevel > segment->_level; --lastLevel)
        result.append("</table></td></tr>\n");
      String id = getElementId(*segment, segment->_title, segment->_arguments);
      result.append(String("<tr><td style=\"width:") + String::fromInt(segment->_level) + "em\">");
      result.append(number.toString());
      result.append("</td><td>");
      result.append(String("<a href=\"#") + id + "\">");
      result.append(Generator::translate(*this, segment->_title));
      result.append("</a></td></tr>\n");
    }
    for(; lastLevel > 1; --lastLevel)
      result.append("</table></td></tr>\n");
    result.append("</table>\n");
    result.append("</p>\n");
  }
  return result;
}

String HtmlGenerator::generate(const OutputData::TexPartSegment& segment)
{
  return String();
}

String HtmlGenerator::generate(const OutputData::PdfSegment& segment)
{
  return String();
}

String HtmlGenerator::escapeChar(uint32 c)
{
  switch (c)
  {
  case '"':
    return String("&quot;");
  case '&':
    return String("&amp;");
  case '\'':
    return String("&apos;");
  case '<':
    return String("&lt;");
  case '>':
    return String("&gt;");
  default:
    return Unicode::toString(c);
  }
}

String HtmlGenerator::getSpanStart(const String& sequence)
{
  if(sequence.startsWith("`"))
    return String("<code>");
  if(sequence == "**" || sequence == "__")
    return String("<strong>");
  return String("<em>");
}

String HtmlGenerator::getSpanEnd(const String& sequence)
{
  if(sequence.startsWith("`"))
    return String("</code>");
  if(sequence == "**" || sequence == "__")
    return String("</strong>");
  return String("</em>");
}

String HtmlGenerator::getWordBreak(const char l, const char r)
{
  return String();
}

String HtmlGenerator::getLink(const String& link, const String& name_)
{
  String name = name_;
  if(name.isEmpty())
    name = link;
  else if(link.startsWith("#"))
  {
    if (name.endsWith("#"))
        name  = name.substr(0, name.length() - 1) + _numbers.find(link.substr(1))->toString();
  }
  return String("<a href=\"") + link + "\">" + Generator::translate(*this, name) + "</a>";
}

String HtmlGenerator::getLineBreak()
{
    return String("<br/>");
}

String HtmlGenerator::getInlineImage(const String& path)
{
  String basename = File::basename(path);
  String outputImageFile = _outputDir + "/" + basename;
  if(!File::copy(path, outputImageFile, false))
      Console::errorf("error: Could not copy file '%s' to '%s': %s\n", (const char*)path, (const char*)outputImageFile, (const char*)Error::getErrorString());
  return String("<img src=\"") + basename + "\" style=\"height:1em;\">";
}

String HtmlGenerator::getFootnote(const String& text)
{
    _footnotes.append(text);
    return String("<sup>") + String::fromInt((int)_footnotes.size()) + "</sup>";
}

String HtmlGenerator::Number::toString() const
{
  String result;
  for(Array<uint>::Iterator i = _number.begin(), end = _number.end(); i != end; ++i)
  {
    if(!result.isEmpty())
      result.append('.');
    result.append(String::fromUInt(*i));
  }
  return result;
}

String HtmlGenerator::stripFormattingAndTranslate(const String& str)
{
  PlainGenerator plainGenerator;
  return escape(Generator::translate(plainGenerator, str));
}

String HtmlGenerator::escape(const String& str)
{
  String result;
  result.reserve(str.length());
  for(const char* i = str; *i; ++i)
    result.append(escapeChar(*i));
  return result;
}

String HtmlGenerator::getElementId(const OutputData::Segment& segment, const String& title, const Map<String, Variant>& arguments)
{
  HashMap<const OutputData::Segment*, String>::Iterator it = _elementIds.find(&segment);
  if(it != _elementIds.end())
      return *it;
  String id = arguments.find("#")->toString();
  if(id.isEmpty())
  {
    id = title;
    id.toLowerCase();
    id = stripFormattingAndTranslate(id);
    id.replace(" ", "-");
  }
  String base = id;
  int index = 1;
  while(_usedElementIds.contains(id))
      id = base + "-" + String::fromInt(++index);
  _elementIds.append(&segment, id);
  return id;
}
