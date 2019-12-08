
#pragma once

#include "Generator.h"

class OutputData;

class TexGenerator : public Generator
{
public:
  bool generate(const String& engine, const OutputData& outputData, const String& outputFile);

  String getErrorString() const;

public:
  static const char* defaultListingsLanguages[];
  static const usize numOfDefaultListingsLanguages;

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

public:
  static String texEscapeChar(char c);
  static String texEscape(const String& str);
  static String getEnvironmentName(const String& language);
  static String getTexSize(const String& size, bool width = true);

//private:
//  const OutputData* outputData;

private:
  //static String mardownUnescape(const String& str);
  static bool matchInlineLink(const char* s, const char* end, const char*& pos, String& result);
  static bool matchInlineImage(const char* s, const char* end, const char*& pos, String& result);
  static bool matchLineBreak(const char* s, const char* end, const char*& pos, String& result);
  static bool matchInlineFootnote(const char* s, const char* end, const char*& pos, String& result);

  String generate(const OutputData& data);
};
