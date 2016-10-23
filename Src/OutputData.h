
#pragma once

#include <nstd/String.h>
#include <nstd/List.h>
#include <nstd/HashMap.h>

class OutputData
{
public:
  class Segment
  {
  public:
    Segment(int indent) : valid(true), indent(indent), parent(0) {}

  public:
    virtual ~Segment() {};
    virtual String generate(const OutputData& outputData) const = 0;
    virtual bool merge(Segment& segment) = 0;

    int getIndent() const {return indent;}
    Segment* getParent() const {return parent;}
    void setParent(Segment& parent) {this->parent = &parent;}

    bool isValid() const {return valid;}
    void invalidate() {valid = false;}

  protected:
    bool valid;
    int indent;
    Segment* parent;
  };

  class ParagraphSegment : public Segment
  {
  public:
    ParagraphSegment(int indent, const String& line) : Segment(indent), text(line) {}
    const String& getText() const {return text;}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment);
  private:
    String text;
  };

  class TitleSegment : public Segment
  {
  public:
    TitleSegment(int indent, int level, const String& title) : Segment(indent), level(level), title(title) {}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment) {return false;}
  private:
    int level;
    String title;
  };

  class SeparatorSegment : public Segment
  {
  public:
    SeparatorSegment(int indent) : Segment(indent), lines(1) {}
    int getLines() const {return lines;}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment);
  private:
    int lines;
  };

  class RuleSegment : public Segment
  {
  public:
    RuleSegment(int indent) : Segment(indent) {}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment) {return false;}
  };

  class BulletListSegment : public Segment
  {
  public:
    BulletListSegment(int indent, char symbol, uint childIndent) : Segment(indent), symbol(symbol), childIndent(childIndent) {}
    ~BulletListSegment();
    char getSymbol() const {return symbol;}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment);
  private:
    List<BulletListSegment*> siblingSegments;
    List<Segment*> childSegments;
    char symbol;
    int childIndent;
  };

  class NumberedListSegment : public Segment
  {
  public:
    NumberedListSegment(int indent, uint number, uint childIndent) : Segment(indent), number(number), childIndent(childIndent) {}
    ~NumberedListSegment();
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment);
  private:
    List<NumberedListSegment*> siblingSegments;
    List<Segment*> childSegments;
    uint number;
    int childIndent;
  };

  class BlockquoteSegment : public Segment
  {
  public:
    BlockquoteSegment(int indent, uint childIndent) : Segment(indent), childIndent(childIndent) {}
    ~BlockquoteSegment();
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment);
  private:
    List<BlockquoteSegment*> siblingSegments;
    List<Segment*> childSegments;
    int childIndent;
  };

  class EnvironmentSegment : public Segment
  {
  public:
    EnvironmentSegment(int indent) : Segment(indent), verbatim(true) {}
    ~EnvironmentSegment();
    void addLine(const String& line) {lines.append(line);}
    bool parseArguments(const String& line, const HashMap<String, bool>& knownEnvironments, String& error);
    bool isVerbatim() const {return verbatim;}
    void swapSegments(List<Segment*>& segments) {this->segments.swap(segments);}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment) {return false;}
  private:
    bool verbatim;
    String language;
    List<String> lines;
    List<Segment*> segments;
  };

  class TexSegment : public Segment
  {
  public:
    TexSegment(const String& content) : Segment(0), content(content) {}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment) {return false;}
  private:
    String content;
  };

  class TexTocSegment : public Segment
  {
  public:
    TexTocSegment() : Segment(0) {}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment) {return false;}
  };

  class TexPartSegment : public Segment
  {
  public:
    TexPartSegment(const String& title) : Segment(0), title(title) {}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment) {return false;}
  private:
    String title;
  };

  class PdfSegment : public Segment
  {
  public:
    PdfSegment(const String& filePath) : Segment(0), filePath(filePath) {}
  public:
    virtual String generate(const OutputData& outputData) const;
    virtual bool merge(Segment& segment) {return false;}
  private:
    String filePath;
  };

public:
  String inputDirectory;
  String outputDirectory;
  String className;
  List<String> headerTexFiles;
  bool hasPdfSegments;
  List<Segment*> segments;
  HashMap<String, bool> environments;

public:
  OutputData() : hasPdfSegments(false) {}
  ~OutputData();

  String generate() const;
};