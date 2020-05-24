
#pragma once

#include <nstd/String.hpp>

#include "OutputData.h"

#pragma once

class Generator
{
public:
  virtual String generate(const OutputData::ParagraphSegment& segment) = 0;
  virtual String generate(const OutputData::TitleSegment& segment) = 0;
  virtual String generate(const OutputData::SeparatorSegment& segment) = 0;
  virtual String generate(const OutputData::FigureSegment& segment) = 0;
  virtual String generate(const OutputData::RuleSegment& segment) = 0;
  virtual String generate(const OutputData::BulletListSegment& segment) = 0;
  virtual String generate(const OutputData::NumberedListSegment& segment) = 0;
  virtual String generate(const OutputData::BlockquoteSegment& segment) = 0;
  virtual String generate(const OutputData::EnvironmentSegment& segment) = 0;
  virtual String generate(const OutputData::TableSegment& segment) = 0;
  virtual String generate(const OutputData::TexSegment& segment) = 0;
  virtual String generate(const OutputData::TexPartSegment& segment) = 0;
  virtual String generate(const OutputData::PdfSegment& segment) = 0;

  virtual String escapeChar(uint32 c) = 0;
  virtual String getSpanStart(const String& sequence) = 0;
  virtual String getSpanEnd(const String& sequence) = 0;
  virtual String getWordBreak(const char l, const char r) = 0;
  virtual String getLink(const String& link, const String& name) = 0;
  virtual String getLineBreak() = 0;
  virtual String getInlineImage(const String& path) = 0;
  virtual String getFootnote(const String& text) = 0;
  virtual String getLatexFormula(const String& formula) = 0;

private:
  static bool matchInlineLink(Generator& generator, const char* s, const char* end, const char*& pos, String& result);
  static bool matchInlineImage(Generator& generator, const char* s, const char* end, const char*& pos, String& result);
  static bool matchLineBreak(Generator& generator, const char* s, const char* end, const char*& pos, String& result);
  static bool matchInlineFootnote(Generator& generator, const char* s, const char* end, const char*& pos, String& result);
  static bool matchInlineLatexFormula(Generator& generator, const char* s, const char* end, const char*& pos, String& result);

protected:
  static String generate(Generator& generator, const OutputData& data);
  static String translate(Generator& generator, const String& str);
};
