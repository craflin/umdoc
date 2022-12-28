
#pragma once

#include "Generator.hpp"

#include <nstd/HashSet.hpp>

struct OutputData;

class HtmlGenerator : public Generator
{
public:
  bool generate(const OutputData& outputData, const String& outputFile);

  String getErrorString() const;

public:
  String generate(const OutputData::ParagraphSegment& segment) override;
  String generate(const OutputData::TitleSegment& segment) override;
  String generate(const OutputData::SeparatorSegment& segment) override;
  String generate(const OutputData::FigureSegment& segment) override;
  String generate(const OutputData::RuleSegment& segment) override;
  String generate(const OutputData::BulletListSegment& segment) override;
  String generate(const OutputData::NumberedListSegment& segment) override;
  String generate(const OutputData::BlockquoteSegment& segment) override;
  String generate(const OutputData::EnvironmentSegment& segment) override;
  String generate(const OutputData::TableSegment& segment) override;
  String generate(const OutputData::TexSegment& segment) override;
  String generate(const OutputData::TexPartSegment& segment) override;
  String generate(const OutputData::PdfSegment& segment) override;

  String escapeChar(uint32 c) override;
  String getSpanStart(const String& sequence) override;
  String getSpanEnd(const String& sequence) override;
  String getWordBreak(const char l, const char r) override;
  String getLink(const String& link, const String& name) override;
  String getLineBreak() override;
  String getInlineImage(const String& path) override;
  String getFootnote(const String& text) override;
  String getLatexFormula(const String& formula) override {return String();};

private:
  class Number
  {
  public:
    Number() {}
    Number(const Array<uint>& number) : _number(number) {}
    Number(uint number) {_number.append(number);}

    String toString() const;

  private:
    Array<uint> _number;
  };

  struct LastNumbers
  {
    Array<uint> title;
    uint figure;
    uint table;

    LastNumbers() : figure(), table() {}
  };

private:
  String _outputDir;

  HashMap<const OutputData::TitleSegment*, Number> _titleNumbers;
  HashMap<const OutputData::FigureSegment*, Number> _figureNumbers;
  HashMap<const OutputData::TableSegment*, Number> _tableNumbers;
  HashMap<String, Number> _numbers;

  HashMap<const OutputData::Segment*, String> _elementIds;
  HashSet<String> _usedElementIds;

  List<String> _footnotes;

private:
  String stripFormattingAndTranslate(const String& str);
  String escape(const String& str);

  void findNumbers(const List<OutputData::SegmentPtr>& segments, LastNumbers& lastNumbers);
  String getElementId(const OutputData::Segment& segment, const String& title, const Map<String, Variant>& arguments);
};
