
#pragma once

#include "Generator.h"

class OutputData;

class HtmlGenerator
{
public:
  bool generate(const OutputData& outputData, const String& outputFile);

  String getErrorString() const;
};
