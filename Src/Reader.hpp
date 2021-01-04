
#pragma once

#include <nstd/String.hpp>

struct InputData;

class Reader
{
public:
  Reader() : _errorLine(0), _errorColumn(0) {}

  bool read(const String& inputFile, InputData& inputData);

  int getErrorLine() const {return _errorLine;}
  int getErrorColumn() const {return _errorColumn;}
  const String& getErrorString() const {return _errorString;}

private:
  int _errorLine;
  int _errorColumn;
  String _errorString;
};
