
#pragma once

#include <nstd/String.h>

class OutputData;

class Generator
{
public:
  bool_t generate(const String& engine, const OutputData& outputData, const String& outputFile);

  String getErrorString() const;

public:
  static String texEscapeChar(char_t c);
  static String texEscape(const String& str, const OutputData& outputData);

private:
  const OutputData* outputData;

private:
  //static String mardownUnescape(const String& str);
  static bool matchInlineLink(const char_t* s, const char_t* end, const OutputData& outputData, const char_t*& pos, String& result);
  static bool matchInlineImage(const char_t* s, const char_t* end, const OutputData& outputData, const char_t*& pos, String& result);
};
