
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <nstd/Debug.h>
#include <nstd/String.h>

class Parser
{
private:
  static String translateHtmlEntities(const String& line);

  friend int main(int argc, char* argv[]);
};

int main(int argc, char* argv[])
{
  String test = Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b");
  ASSERT(Parser::translateHtmlEntities("a&nbsp;&nbsp;&#8364;b") == "a\302\240\302\240\342\202\254b");

  return 0;
}
