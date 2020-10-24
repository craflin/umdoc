
#include <nstd/Debug.hpp>
#include <nstd/String.hpp>
#include <nstd/HashMap.hpp>

class Parser
{
private:
  static String translateHtmlEntities(const String& line);
  static String replacePlaceholderVariables(const String& data, const HashMap<String, String>& variables, bool allowEscaping);

  friend int main(int argc, char* argv[]);
};

int main(int argc, char* argv[])
{
  {
    String test = Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b");
    ASSERT(Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b") == "a\302\240\302\240\342\202\254b");
  }

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

  return 0;
}
