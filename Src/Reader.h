
#pragma once


#include <nstd/String.h>

class InputData;

class Reader
{
public:
  bool_t read(const String& inputFile, InputData& inputData);
};
