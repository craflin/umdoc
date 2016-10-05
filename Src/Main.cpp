
#include <nstd/Process.h>
#include <nstd/Console.h>

#include "InputData.h"
#include "OutputData.h"
#include "Reader.h"
#include "Parser.h"
#include "Generator.h"

int_t main(int_t argc, char_t* argv[])
{
  String configFile("md2tex.xml");
  String outputFile;

  {
    Process::Option options[] = {
        {'o', "output", Process::argumentFlag},
    };
    Process::Arguments arguments(argc, argv, options);
    int_t character;
    String argument;
    while(arguments.read(character, argument))
      switch(character)
      {
      case 'o':
        outputFile = argument;
        break;
      case '?':
        Console::errorf("Unknown option: %s.\n", (const char_t*)argument);
        return 1;
      case ':':
        Console::errorf("Option %s required an argument.\n", (const char_t*)argument);
        return 1;
      default:
        Console::errorf("Usage: %s [<config file>] [-o <output file>]\n", argv[0]);
        return 1;
      }
  }

  if(outputFile.isEmpty())
  {
    const tchar_t* end = configFile.findOneOf("\\/.");
    if(*end == '.')
      outputFile = configFile.substr(0, end - configFile) + ".tex";
    else
      outputFile =configFile + ".tex";
  }

  InputData inputData;
  OutputData outputData;
  {
    Reader reader;
    if(!reader.read(configFile, inputData))
      return 1;
  }
  {
    Parser parser;
    if(!parser.parse(inputData, outputData))
      return 1;
  }
  {
    Generator generator;
    if(!generator.generate(outputData, outputFile))
      return 1;
  }

  return 0;
}
