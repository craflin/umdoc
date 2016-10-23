
#pragma once

#include <nstd/String.h>

class InputData;
class OutputData;

class Parser
{
public:
  Parser();
  ~Parser();

  bool parse(const InputData& inputData, const String& outputFile, OutputData& outputData);

  String getErrorFile() const;
  int getErrorLine() const;
  String getErrorString() const;

private:
  class Private;
  Private* p;
};
