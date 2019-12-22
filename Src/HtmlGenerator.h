
#pragma once

#include "Generator.h"

class OutputData;

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

  String escapeChar(const char c) override;
  String getSpanStart(const String& sequence) override;
  String getSpanEnd(const String& sequence) override;
  String getWordBreak(const char l, const char r) override;
};
