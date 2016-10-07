
#pragma once

#include <nstd/String.h>

class OutputData;

class Generator
{
public:
  bool_t generate(const OutputData& outputData, const String& outputFile);

  String getErrorString() const;

public:
  static String texEscape(const String& str);
};
