
#pragma once

#include <nstd/String.h>

class InputData;

class Reader
{
public:
  Reader() : errorLine(0), errorColumn(0) {}

  bool read(const String& inputFile, InputData& inputData);

  int getErrorLine() const {return errorLine;}
  int getErrorColumn() const {return errorColumn;}
  const String& getErrorString() const {return errorString;}

private:
  int errorLine;
  int errorColumn;
  String errorString;
};
