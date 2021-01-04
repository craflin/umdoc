
#include <nstd/Process.hpp>
#include <nstd/Console.hpp>
#include <nstd/Directory.hpp>
#include <nstd/Error.hpp>
#include <nstd/File.hpp>

#include "InputData.hpp"
#include "OutputData.hpp"
#include "Reader.hpp"
#include "Parser.hpp"
#include "TexGenerator.hpp"
#include "HtmlGenerator.hpp"

static bool latex2pdf(const String& texFile, const String& engine, const String& auxDirectory)
{
  String tocFile;
  String auxFile;
  {
    const char* end = texFile.findLastOf("\\/.");
    if(end)
    {
      tocFile = texFile.substr(0, end - (const char*)texFile) + ".toc";
      auxFile = texFile.substr(0, end - (const char*)texFile) + ".aux";
    }
    else
    {
      tocFile = texFile + ".toc";
      auxFile = texFile + ".aux";
    }
  }

  for(int run = 0; run < 5; ++run)
  {
    bool tocFileExists = File::exists(tocFile);
    bool auxFileExists = File::exists(auxFile);
    bool rerun = false;

    Process process;
    if(process.open(engine + " --interaction=scrollmode --halt-on-error --file-line-error --aux-directory=\"" + auxDirectory + "\" --output-directory=\"" + auxDirectory + "\" \"" + texFile + "\"") == 0)
      return false;

    // Well I tried to set textinfo.max_print_line in an lua init script. This does not work since it is overwritten from kpathsea when it is reading its config file (texmf.cnf).
    // I tried to create a texmf.cnf file to overwrite max_print_line just for this build, but this does not seem to work with MiKTeX. After inspecting some MiKTeX I came to the
    // conclusion that there is currently no way to set max_print_line without editing the global texmfapp.ini or hacking into MiKTeX's file IO operations.

    // Why the fuck are TeX engines still wrapping log messages after 79 characters? We have terminal emulators and text editors that will do this for you.

    // Now, lets try to unfuck lualatex's output:

    char buffer[32 * 1024];
    String unhandledData;
    String bufferedLine;
    for(ssize i;;)
      switch((i = process.read(buffer, sizeof(buffer))))
      {
      case -1:
        return false;
      case 0:
        if(!bufferedLine.isEmpty() || !unhandledData.isEmpty())
          Console::print(bufferedLine + unhandledData + "\n");
        goto done;
      default:
        unhandledData.append(buffer, i);

        for(;;)
        {
          const char* lineEnd = unhandledData.findOneOf("\r\n");
          if(lineEnd)
          {
            usize lineLen = lineEnd - (const char*)unhandledData;
#ifdef _WIN32
            if(lineLen == 79)
#else
            if(lineLen == 80)
#endif
              bufferedLine.append(unhandledData.substr(0, lineLen));
            else
            {
              String line = bufferedLine + unhandledData.substr(0, lineLen);
              if(line.find("Rerun to get") || line.find("run LaTeX again"))
                rerun = true;

              Console::print(line + "\n");
              bufferedLine.clear();
            }
            if(*lineEnd == '\r' && lineEnd[1] == '\n')
              ++lineLen;
            unhandledData = unhandledData.substr(lineLen + 1);
          }
          else
            break;
        }
        break;
      }

  done: ;

    uint32 exitCode;
    if(!process.join(exitCode))
      return false;
    if(exitCode != 0)
      return false;

    if((!tocFileExists && File::exists(tocFile)) ||
       (!auxFileExists && File::exists(auxFile)))
       rerun = true;

    if(!rerun)
       break;

  }

  return true;
}

int main(int argc, char* argv[])
{
  String inputFile("umdoc.xml");
  String outputFile;
  String engine("xelatex");
  String auxDirectory;
  HashMap<String, String> variables;

  {
    Process::Option options[] = {
        {'o', "output", Process::argumentFlag},
        {'e', "engine", Process::argumentFlag},
        {'a', "aux-directory", Process::argumentFlag},
        {'h', "help", Process::optionFlag},
        {1000, "version", Process::optionFlag},
    };
    Process::Arguments arguments(argc, argv, options);
    int character;
    String argument;
    while(arguments.read(character, argument))
      switch(character)
      {
      case 'o':
        outputFile = argument;
        break;
       case 'e':
        engine = argument;
        break;
       case 'a':
        auxDirectory = argument;
        break;
      case '?':
        if(argument.startsWith("--"))
        {
          const char* i = argument.find('=');
          if(i)
          {
            variables.append(argument.substr(2, i - (const char*)argument - 2), argument.substr(i  - (const char*)argument + 1));
            continue;
          }
        }
        Console::errorf("Unknown option: %s.\n", (const char*)argument);
        return 1;
      case ':':
        Console::errorf("Option %s required an argument.\n", (const char*)argument);
        return 1;
      case 1000:
        Console::errorf("umdoc %s\n", VERSION);
        return 0;
      case '\0':
        inputFile = argument;
        break;
      default:
        Console::errorf("Usage: %s [<input-file>] [-a <aux-directory>] [-e <latex-engine>]\n             [-o <output-file>] [--<variable>=<value>]\n", argv[0]);
        return 1;
      }
  }

  if(outputFile.isEmpty())
  {
    const char* end = inputFile.findLastOf("\\/.");
    if(end && *end == '.')
      outputFile = inputFile.substr(0, end - (const char*)inputFile) + ".pdf";
    else
      outputFile = inputFile + ".pdf";
  }

  if(auxDirectory.isEmpty())
  {
    const char* end = outputFile.findLastOf("\\/.");
    if(end && *end == '.')
      auxDirectory = outputFile.substr(0, end - (const char*)outputFile);
    else
      auxDirectory = outputFile;
  }

  String tmpTexFile;
  String tmpPdfFile;
  {
    String inputFileBasename = File::basename(inputFile);
    const char* end = inputFileBasename.findLast('.');
    if(end)
      inputFileBasename = inputFileBasename.substr(0, end - (const char*)inputFileBasename);
    tmpTexFile = auxDirectory + "/" + inputFileBasename + ".tex";
    tmpPdfFile = auxDirectory + "/" + inputFileBasename + ".pdf";
  }

  // change working directory to the directory of the input fle
  String inputFileDir = File::dirname(inputFile);
  if(inputFileDir != ".")
  {
    // convert input and output files to absolute paths
    inputFile = File::getAbsolutePath(inputFile);
    outputFile = File::getAbsolutePath(outputFile);
    auxDirectory = File::getAbsolutePath(auxDirectory);
    tmpTexFile = File::getAbsolutePath(tmpTexFile);
    tmpPdfFile = File::getAbsolutePath(tmpPdfFile);

    if(!Directory::change(inputFileDir))
    {
      Console::errorf("%s: error: %s\n", (const char*)inputFile, (const char*)Error::getErrorString());
      return 1;
    }
  }

  //
  InputData inputData;
  OutputData outputData;

  // read input file
  {
    Reader reader;
    if(!reader.read(inputFile, inputData))
    {
      Console::errorf("%s:%d:%d: error: %s\n", (const char*)inputFile, reader.getErrorLine(), reader.getErrorColumn(), (const char*)reader.getErrorString());
      return 1;
    }
  }

  // set variables
  for(HashMap<String, String>::Iterator i = variables.begin(), end = variables.end(); i != end; ++i)
  {
    if(inputData.variables.find(i.key()) == inputData.variables.end())
    {
      Console::errorf("Unknown option: %s.\n", (const char*)i.key());
      return 1;
    }
    inputData.variables.append(i.key(), *i);
  }

  // parse input data
  {
    Parser parser(outputData.format);
    if(!parser.parse(inputData, outputFile, outputData))
    {
      Console::errorf("%s:%d: error: %s\n", (const char*)parser.getErrorFile(), parser.getErrorLine(), (const char*)parser.getErrorString());
      return 1;
    }
  }

  // generate html
  if(outputData.format == OutputData::htmlFormat)
  {
    HtmlGenerator generator;
    if(!generator.generate(outputData, outputFile))
    {
      Console::errorf("%s: error: %s\n", (const char*)tmpTexFile, (const char*)generator.getErrorString());
      return 1;
    }
    return 0;
  }

  // generate tmp tex file
  if(!Directory::exists(auxDirectory) && !Directory::create(auxDirectory))
  {
    Console::errorf("%s: error: %s\n", (const char*)auxDirectory, (const char*)Error::getErrorString());
    return 1;
  }
  {
    TexGenerator generator;
    if(!generator.generate(engine, outputData, outputFile.endsWith(".tex") ? outputFile : tmpTexFile))
    {
      Console::errorf("%s: error: %s\n", (const char*)tmpTexFile, (const char*)generator.getErrorString());
      return 1;
    }
  }
  if(outputFile.endsWith(".tex"))
    return 0;

  // covnert tmp tex file to tmp pdf
  if(!latex2pdf(tmpTexFile, engine, auxDirectory))
    return 1;

  // copy tmp pdf to output file
  if(tmpPdfFile != outputFile)
  {
    String pdfData;
    File pdfFile;
    if(!pdfFile.open(tmpPdfFile) ||
       !pdfFile.readAll(pdfData))
    {
      Console::errorf("%s: error: %s\n", (const char*)tmpPdfFile, (const char*)Error::getErrorString());
      return 1;
    }
    File file;
    if(!file.open(outputFile, File::writeFlag) ||
       !file.write(pdfData))
    {
      Console::errorf("%s: error: %s\n", (const char*)outputFile, (const char*)Error::getErrorString());
      return 1;
    }
  }

  return 0;
}
