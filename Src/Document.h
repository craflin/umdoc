
#pragma once

#include <nstd/String.h>
#include <nstd/List.h>

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

    int_t getIndent() const {return indent;}

  private:
    int_t indent;
  };

  class TitleSegment : public Segment
  {
  public:
    TitleSegment(int_t indent, const String& line);
  public:
    virtual void_t print();
    virtual bool_t merge(Segment& segment) {return false;}
  private:
    int level;
    String title;
  };

  class ParagraphSegment : public Segment
  {
  public:
    ParagraphSegment(int_t indent, const String& line) : Segment(indent) {text.attach((const char_t*)line + indent, line.length() - indent);}

  public:
    virtual void_t print();
    virtual bool_t merge(Segment& segment);
  private:
    String text;
  };

  class SeparatorSegment : public Segment
  {
  public:
    SeparatorSegment(int_t indent) : Segment(indent) {}
  public:
    virtual void_t print();
    virtual bool_t merge(Segment& segment);
  };

  class RuleSegment : public Segment
  {
  public:
    RuleSegment(int_t indent) : Segment(indent) {}
  public:
    virtual void_t print();
    virtual bool_t merge(Segment& segment) {return false;}
  };

  class ListSegment : public Segment
  {
  public:
    ListSegment(int_t indent, uint_t childIndent) : Segment(indent), childIndent(childIndent) {}
  public:
    virtual void_t print();
    virtual bool_t merge(Segment& segment);
  private:
    List<ListSegment*> siblingSegments;
    List<Segment*> childSegments;
    int_t childIndent;
    Segment* parent;
  };

public:
  const String& getErrorString() const {return lastError;}
  int_t getErrorColumn() const {return errorColumn;}

  bool_t addLine(const String& line, size_t offset = 0);
  String generate() const;

private:
  List<Segment*> segments;
  String lastError;
  int_t errorColumn;

private:
  void_t addSegment(Segment& segment);
};
