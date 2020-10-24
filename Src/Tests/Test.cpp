
#include <nstd/Debug.hpp>
#include <nstd/String.hpp>
#include <nstd/HashMap.hpp>
#include <nstd/File.hpp>
#include <nstd/Process.hpp>

class Parser
{
private:
  static String translateHtmlEntities(const String& line);
  static String replacePlaceholderVariables(const String& data, const HashMap<String, String>& variables, bool allowEscaping);

  friend int main(int argc, char* argv[]);
};

int main(int argc, char* argv[])
{
  // test Parser::translateHtmlEntities
  {
    String test = Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b");
    ASSERT(Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b") == "a\302\240\302\240\342\202\254b");
  }

  // test Parser::replacePlaceholderVariables
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

  // test umdoc executable launching with non utf-8 char input
  {
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

  return 0;
}
