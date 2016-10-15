
#pragma once

#include <nstd/String.h>

class OutputData;

class Generator
{
public:
  bool_t generate(const String& engine, const OutputData& outputData, const String& outputFile);

  String getErrorString() const;

public:
  static String texEscape(char_t c);
  static String texEscape(const String& str);
};
