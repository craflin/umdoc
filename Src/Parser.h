
#pragma once

#include <nstd/String.h>

class InputData;
class OutputData;

class Parser
{
public:
  Parser();
  ~Parser();

  bool_t parse(const InputData& inputData, OutputData& outputData);

  String getErrorFile() const;
  int_t getErrorLine() const;
  String getErrorString() const;

private:
  class Private;
  Private* p;
};
