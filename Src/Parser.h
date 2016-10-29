
#pragma once

#include <nstd/String.h>
#include <nstd/Map.h>
#include <nstd/Variant.h>

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

public:
  static void extractArguments(String& line, Map<String, Variant>& args);

private:
  class Private;
  Private* p;
};
