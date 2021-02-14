
#include <nstd/Debug.hpp>
#include <nstd/String.hpp>
#include <nstd/HashMap.hpp>
#include <nstd/File.hpp>
#include <nstd/Process.hpp>

#include "../Parser.hpp"
#include "../InputData.hpp"
#include "../Reader.hpp"
#include "../TexGenerator.hpp"

void test_Parser_translateHtmlEntities()
{
  String test = Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b");
  ASSERT(Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b") == "a\302\240\302\240\342\202\254b");
}

void test_Parser_replacePlaceholderVariables()
{
  HashMap<String, String> vars;
  vars.append("ab", "12");
  vars.append("c", "34");
  vars.append("de", "56");
  ASSERT(Parser::replacePlaceholderVariables("%ab%-%c%-%de%", vars, true) == "12-34-56");
  ASSERT(Parser::replacePlaceholderVariables("%ab%-%c%-", vars, true) == "12-34-");
  ASSERT(Parser::replacePlaceholderVariables("-%c%-", vars, true) == "-34-");
  ASSERT(Parser::replacePlaceholderVariables("-%c%-%f%-", vars, true) == "-34-%f%-");
  ASSERT(Parser::replacePlaceholderVariables("-\\%c% %f%-", vars, true) == "-\\%c% %f%-");
  ASSERT(Parser::replacePlaceholderVariables("-\\%c%-%f%-", vars, false) == "-\\34-%f%-");
}

void test_Parser_TableCellParsing()
{
  {
    File file;
    ASSERT(file.open("umdoc.xml", File::writeFlag));
    ASSERT(file.write("<umdoc><document><md>\n"));
    ASSERT(file.write("+-----+\n"));
    ASSERT(file.write("| * b |\n"));
    ASSERT(file.write("|   c |\n"));
    ASSERT(file.write("+-----+\n"));
    ASSERT(file.write("</md></document></umdoc>"));
  }

  {
    InputData inputData;
    OutputData outputData;
    Reader reader;
    Parser parser;
    TexGenerator generator;
    ASSERT(reader.read("umdoc.xml", inputData));
    ASSERT(parser.parse(inputData, "test.tex", outputData));
    ASSERT(generator.generate(String(), outputData, "test.tex"));
  }

  {
    String data;
    ASSERT(File::readAll("test.tex", data));
    ASSERT(data.find("\\item \nb  c \n\\end{itemize}\n"));
  }

  ASSERT(File::unlink("umdoc.xml"));
  ASSERT(File::unlink("test.tex"));
}

void test_Parser_TableCellParsing2()
{
  {
    File file;
    ASSERT(file.open("umdoc.xml", File::writeFlag));
    ASSERT(file.write("<umdoc><document><md>\n"));
    ASSERT(file.write("| * b |\n"));
    ASSERT(file.write("    c\n"));
    ASSERT(file.write("</md></document></umdoc>"));
  }

  {
    InputData inputData;
    OutputData outputData;
    Reader reader;
    Parser parser;
    TexGenerator generator;
    ASSERT(reader.read("umdoc.xml", inputData));
    ASSERT(parser.parse(inputData, "test.tex", outputData));
    ASSERT(generator.generate(String(), outputData, "test.tex"));
  }

  {
    String data;
    ASSERT(File::readAll("test.tex", data));
    ASSERT(data.find("\\item \nb  c\n\\end{itemize}\n"));
  }

  ASSERT(File::unlink("umdoc.xml"));
  ASSERT(File::unlink("test.tex"));
}

void test_umdoc_NonUtf8InputChar()
{ // test umdoc executable launching with non utf-8 char input
  {
    File file;
    ASSERT(file.open("umdoc.xml", File::writeFlag));
    ASSERT(file.write("<umdoc><document><md>Test²</md></document></umdoc>"));
  }

  {
    Process process;
    ASSERT(process.start(UMDOC_EXECUTABLE " -o test.tex"));
    uint32 exitCode;
    ASSERT(process.join(exitCode));
    ASSERT(exitCode == 0);
  }

  ASSERT(File::exists("test.tex"));
  ASSERT(File::unlink("umdoc.xml"));
  ASSERT(File::unlink("test.tex"));
}

void test_umdoc_UnclosedEnvironment()
{ // test umdoc executable launching with an unclosed environment
  {
    File file;
    ASSERT(file.open("umdoc.xml", File::writeFlag));
    ASSERT(file.write("<umdoc class=\"article\"><environment name=\"test\" verbatim=\"true\"/><document><md>a\n\n```test\n</md></document></umdoc>"));
  }

  {
    Process process;
    ASSERT(process.start(UMDOC_EXECUTABLE " -o test.tex"));
    uint32 exitCode;
    ASSERT(process.join(exitCode));
    ASSERT(exitCode == 0);
  }

  ASSERT(File::exists("test.tex"));
  ASSERT(File::unlink("umdoc.xml"));
  ASSERT(File::unlink("test.tex"));
}

int main(int argc, char* argv[])
{
  test_Parser_translateHtmlEntities();
  test_Parser_replacePlaceholderVariables();
  test_Parser_TableCellParsing();
  test_Parser_TableCellParsing2();
  test_umdoc_NonUtf8InputChar();
  test_umdoc_UnclosedEnvironment();
  return 0;
}
