
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

private:
  class Private;
  Private* p;
};
