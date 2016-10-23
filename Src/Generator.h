
#pragma once

#include <nstd/String.h>

class OutputData;

class Generator
{
public:
  bool generate(const String& engine, const OutputData& outputData, const String& outputFile);

  String getErrorString() const;

public:
  static String texEscapeChar(char c);
  static String texEscape(const String& str, const OutputData& outputData);

private:
  const OutputData* outputData;

private:
  //static String mardownUnescape(const String& str);
  static bool matchInlineLink(const char* s, const char* end, const OutputData& outputData, const char*& pos, String& result);
  static bool matchInlineImage(const char* s, const char* end, const OutputData& outputData, const char*& pos, String& result);
};
