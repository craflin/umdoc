
#pragma once

#include "Generator.h"

class PlainGenerator : public Generator
{
public:
  String generate(const OutputData::ParagraphSegment& segment) override {return String();}
  String generate(const OutputData::TitleSegment& segment) override {return String();}
  String generate(const OutputData::SeparatorSegment& segment) override {return String();}
  String generate(const OutputData::FigureSegment& segment) override {return String();}
  String generate(const OutputData::RuleSegment& segment) override {return String();}
  String generate(const OutputData::BulletListSegment& segment) override {return String();}
  String generate(const OutputData::NumberedListSegment& segment) override {return String();}
  String generate(const OutputData::BlockquoteSegment& segment) override {return String();}
  String generate(const OutputData::EnvironmentSegment& segment) override {return String();}
  String generate(const OutputData::TableSegment& segment) override {return String();}
  String generate(const OutputData::TexSegment& segment) override {return String();}
  String generate(const OutputData::TexPartSegment& segment) override {return String();}
  String generate(const OutputData::PdfSegment& segment) override {return String();}

  String escapeChar(const char c) override {return String(&c, 1);}
  String getSpanStart(const String& sequence) override {return String();}
  String getSpanEnd(const String& sequence) override {return String();}
  String getWordBreak(const char l, const char r) override {return String();}
  String getLink(const String& link, const String& name) override {return String();}
  String getLineBreak() override {return String();}
  String getInlineImage(const String& path) override {return String();}

};
