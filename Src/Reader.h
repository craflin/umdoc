
#pragma once


#include <nstd/String.h>

class InputData;

class Reader
{
public:
  Reader() : errorLine(0), errorColumn(0) {}

  bool_t read(const String& inputFile, InputData& inputData);

  int_t getErrorLine() const {return errorLine;}
  int_t getErrorColumn() const {return errorColumn;}
  const String& getErrorString() const {return errorString;}

private:
  int_t errorLine;
  int_t errorColumn;
  String errorString;
};
