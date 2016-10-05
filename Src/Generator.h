
#pragma once

#include <nstd/String.h>

class OutputData;

class Generator
{
public:
  bool_t generate(const OutputData& outputData, const String& outputFile);

private:
  static String texEscape(const String& str);
};
