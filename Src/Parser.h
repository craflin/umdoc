
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

  String getErrorFile() const {return String();}
  int_t getErrorLine() const {return 0;}
  int_t getErrorColumn() const {return 0;}
  String getErrorString() const {return String();}

private:
  class Private;
  Private* p;
};
