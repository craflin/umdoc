
#pragma once

#include <nstd/String.h>
#include <nstd/List.h>

class OutputData
{
public:
  class Segment
  {
  public:
    Segment(int_t indent) : valid(true), indent(indent), parent(0) {}

  public:
    virtual ~Segment() {};
    virtual String generate() const = 0;
    virtual bool_t merge(Segment& segment) = 0;

    int_t getIndent() const {return indent;}
    Segment* getParent() const {return parent;}
    void_t setParent(Segment& parent) {this->parent = &parent;}

    bool_t isValid() const {return valid;}
    void_t invalidate() {valid = false;}

  protected:
    bool_t valid;
    int_t indent;
    Segment* parent;
  };

  class ParagraphSegment : public Segment
  {
  public:
    ParagraphSegment(int_t indent, const String& line) : Segment(indent), text(line) {}
    const String& getText() const {return text;}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment);
  private:
    String text;
  };

  class TitleSegment : public Segment
  {
  public:
    TitleSegment(int_t indent, int_t level, const String& title) : Segment(indent), level(level), title(title) {}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment) {return false;}
  private:
    int_t level;
    String title;
  };

  class SeparatorSegment : public Segment
  {
  public:
    SeparatorSegment(int_t indent) : Segment(indent), lines(1) {}
    int_t getLines() const {return lines;}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment);
  private:
    int_t lines;
  };

  class RuleSegment : public Segment
  {
  public:
    RuleSegment(int_t indent) : Segment(indent) {}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment) {return false;}
  };

  class ListSegment : public Segment
  {
  public:
    ListSegment(int_t indent, uint_t childIndent) : Segment(indent), childIndent(childIndent) {}
    ~ListSegment();
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment);
  private:
    List<ListSegment*> siblingSegments;
    List<Segment*> childSegments;
    int_t childIndent;
  };

  class CodeSegment : public Segment
  {
  public:
    CodeSegment(int_t indent) : Segment(indent) {}
    void_t addLine(const String& line) {lines.append(line);}
    bool_t parseArguments(const String& line) {return true;}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment) {return false;}
  private:
    String language;
    List<String> lines;
  };

  class TexSegment : public Segment
  {
  public:
    TexSegment(const String& content) : Segment(0), content(content) {}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment) {return false;}
  private:
    String content;
  };

  class TexTocSegment : public Segment
  {
  public:
    TexTocSegment() : Segment(0) {}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment) {return false;}
  };

  class TexPartSegment : public Segment
  {
  public:
    TexPartSegment(const String& title) : Segment(0), title(title) {}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment) {return false;}
  private:
    String title;
  };

  class PdfSegment : public Segment
  {
  public:
    PdfSegment(const String& filePath) : Segment(0), filePath(filePath) {}
  public:
    virtual String generate() const;
    virtual bool_t merge(Segment& segment) {return false;}
  private:
    String filePath;
  };

public:
  String className;
  List<String> headerTexFiles;
  bool hasPdfSegments;
  List<Segment*> segments;

public:
  OutputData() : hasPdfSegments(false) {}
  ~OutputData();

  String generate() const;
};
