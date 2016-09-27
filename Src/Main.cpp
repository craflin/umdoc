
#include <nstd/Process.h>
#include <nstd/Console.h>
#include <nstd/File.h>
#include <nstd/Error.h>
#include <nstd/Document/XML.h>

class InputData
{
public:
  class Component
  {
  public:
    enum Type
    {
      texType,
      texTocType,
      texPartType,
      pdfType,
      mdType,
    };

  public:
    Type type;
    String filePath;
    String content;
  };

public:
  String className;
  List<String> headerTexFiles;
  List<Component> document;
};

static bool_t loadInputFile(const String& inputFile, InputData& inputData)
{
  XML::Parser xmlParser;
  XML::Element xmlFile;
  if(!xmlParser.load(inputFile, xmlFile))
  {
    Console::errorf("%s:%d:%d: error: %s", (const char_t*)inputFile, xmlParser.getErrorLine(), xmlParser.getErrorColumn(), xmlParser.getErrorString());
    return false;
  }

  if(xmlFile.type != "md2tex")
  {
    Console::errorf("%s:%d: error: Expected element 'md2tex'", (const char_t*)inputFile, xmlFile.line);
    return false;
  }
  inputData.className = *xmlFile.attributes.find("class");

  bool documentRead = false;
  for(List<XML::Variant>::Iterator i = xmlFile.content.begin(), end = xmlFile.content.end(); i != end; ++i)
  {
    const XML::Variant& variant = *i;
    if(!variant.isElement())
      continue;
    const XML::Element& element = variant.toElement();
    if(documentRead)
    {
      Console::errorf("%s:%d:%d: error: Unexpected element '%s'", (const char_t*)inputFile, element.line, element.column, element.type);
      return false;
    }
    if(element.type == "tex")
    {
      String filePath = *element.attributes.find("file");
      File file;
      String data;
      if(!file.open(filePath))
      {
        Console::errorf("%s:%d:%d: error: Could not open file '%s': %s", (const char_t*)inputFile, element.line, element.column, (const char_t*)filePath, (const char_t*)Error::getErrorString());
        return false;
      }
      if(!file.readAll(data))
      {
        Console::errorf("%s:%d:%d: error: Could not read file '%s': %s", (const char_t*)inputFile, element.line, element.column, (const char_t*)filePath, (const char_t*)Error::getErrorString());
        return false;
      }
      inputData.headerTexFiles.append(data);

    }
    else if(element.type == "document")
    {
      for(List<XML::Variant>::Iterator i = element.content.begin(), end = element.content.end(); i != end; ++i)
      {
        const XML::Variant& variant = *i;
        if(!variant.isElement())
          continue;
        const XML::Element& element = variant.toElement();
        if(element.type == "tex" || element.type == "md")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = element.type == "md" ? InputData::Component::mdType : InputData::Component::texType;
          component.filePath = *element.attributes.find("file");
          File file;
          if(!file.open(component.filePath))
          {
            Console::errorf("%s:%d:%d: error: Could not open file '%s': %s", (const char_t*)inputFile, element.line, element.column, (const char_t*)component.filePath, (const char_t*)Error::getErrorString());
            return false;
          }
          if(!file.readAll(component.content))
          {
            Console::errorf("%s:%d:%d: error: Could not read file '%s': %s", (const char_t*)inputFile, element.line, element.column, (const char_t*)component.filePath, (const char_t*)Error::getErrorString());
            return false;
          }
        }
        else if(element.type == "toc")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::texTocType;
        }
        else if(element.type == "pdf")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::pdfType;
          component.filePath = *element.attributes.find("file");
        }
        else if(element.type == "part")
        {
          InputData::Component& component = inputData.document.append(InputData::Component());
          component.type = InputData::Component::pdfType;
          component.content = *element.attributes.find("title");
        }
        else
        {
          Console::errorf("%s:%d: error: Unexpected element '%s'", (const char_t*)inputFile, element.line, element.type);
          return false;
        }
      }
      documentRead = true;
    }
    else
    {
      Console::errorf("%s:%d: error: Unexpected element '%s'", (const char_t*)inputFile, element.line, element.type);
      return false;
    }
  }

  return true;
}

String texEscape(const String& str)
{
  // todo
  return str;
}

class Document
{
public:
  class Segment
  {
  public:
    Segment(int_t indent) : indent(indent) {}

  public:
    virtual ~Segment() {};
    virtual void_t print() = 0;
    virtual bool_t merge(Segment& segment) = 0;

  protected:
    int_t indent;
  };

  class TitleSegment : public Segment
  {
  public:
    virtual void_t print() {}
  };

  class ParagraphSegment : public Segment
  {
  public:
    ParagraphSegment(int_t indent, const String& line) : Segment(indent) {text.attach((const char_t*)line + indent, line.length() - indent);}

  public:
    virtual void_t print() {}
    virtual bool_t merge(Segment& segment)
    {
      ParagraphSegment* paragraphSegment = dynamic_cast<ParagraphSegment*>(&segment);
      if(paragraphSegment && paragraphSegment->indent == indent)
      {
        text.append(' ');
        text.append(paragraphSegment->text);
        delete paragraphSegment;
        return true;
      }
      return false;
    }

  private:
    String text;
  };

  class SeparatorSegment : public Segment
  {
  public:
    SeparatorSegment(int_t indent) : Segment(indent) {}
  public:
    virtual void_t print() {}
    virtual bool_t merge(Segment& segment)
    {
      if(dynamic_cast<SeparatorSegment*>(&segment))
      {
        delete &segment;
        return true;
      }
      return false;
    }
  };

public:
  const String& getErrorString() const {return lastError;}
  int_t getErrorColumn() const {return errorColumn;}

  bool_t addLine(const String& line);
  String generate() const
  {
    return String();
  }

private:
  List<Segment*> segments;
  String lastError;
  int_t errorColumn;
};

bool_t Document::addLine(const String& line)
{
  int_t indent = 0;
  Segment* segment = 0;
  const char_t* p = line;
  for(; *p == ' '; ++p)
    ++indent;
  switch(*p)
  {
  case '\r':
  case '\n':
  case '\0':
    segment = new SeparatorSegment(indent);
    break;
  default:
    segment = new ParagraphSegment(indent, line);
  }

  if(segments.isEmpty() || !segments.back()->merge(*segment))
    segments.append(segment);
  return true;
}

static bool_t markdown2Tex(const String& filePath, const String& fileContent, String& output)
{
  Document doc;

  int_t line = 1;
  String lineStr;
  for(const char_t* p = fileContent, * end; *p; (p = end), ++line)
  {
    end = String::findOneOf(p, "\r\n");
    if(!end)
      end = p + String::length(p);
    
    lineStr.attach(p, end - p);
    if(!doc.addLine(lineStr))
    {
      Console::errorf("%s:%d:%d: %s", (const char_t*)filePath, line, doc.getErrorColumn(), (const char_t*)doc.getErrorString());
      return false;
    }

    if(*end == '\r' && end[1] == '\n')
      ++end;
    if(*end)
      ++end;
  }

  output = doc.generate();
  return true;
}

static bool_t createOutputFile(const InputData& inputData, const String& outputFile)
{
  File file;
  if(!file.open(outputFile, File::writeFlag))
    return false;

  bool_t usePdfPages = false;
  for(List<InputData::Component>::Iterator i = inputData.document.begin(), end = inputData.document.end(); i != end; ++i)
    if(i->type == InputData::Component::pdfType)
    {
      usePdfPages = true;
      break;
    }

  String className = inputData.className;
  if(className.isEmpty())
    className = "article";

  file.write(String("\\documentclass[a4paper]{") + className + "}\n");
  file.write("\\usepackage[utf8]{inputenc}\n");
  if(usePdfPages)
    file.write("\\usepackage{pdfpages}\n");
  file.write("\n");
  for(List<String>::Iterator i = inputData.headerTexFiles.begin(), end = inputData.headerTexFiles.end(); i != end; ++i)
  {
    file.write(*i);
    file.write("\n");
  }
  file.write("\n");

  file.write("\\begin{document}\n");
  file.write("\n");

  for(List<InputData::Component>::Iterator i = inputData.document.begin(), end = inputData.document.end(); i != end; ++i)
  {
    const InputData::Component& component = *i;
    switch(component.type)
    {
    case InputData::Component::texType:
      file.write(component.content);
      file.write("\n");
      break;
    case InputData::Component::texTocType:
      file.write("\\tableofcontents\n");
      break;
    case InputData::Component::texPartType:
      file.write(String("\\part{") + texEscape(component.content) + "}\n");
      break;
    case InputData::Component::pdfType:
      file.write(String("\\includepdf[pages=-]{") + component.filePath + "}\n");
      break;
    case InputData::Component::mdType:
      {
        String output;
        if(!markdown2Tex(component.filePath, component.content, output))
          return false;
        file.write(output);
      }
      file.write("\n");
      break;
    }
    file.write("\n");
  }

  file.write("\\end{document}\n");

  return true;
}

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
  if(!loadInputFile(configFile, inputData) ||
     !createOutputFile(inputData, outputFile))
    return 1;

  return 0;
}
