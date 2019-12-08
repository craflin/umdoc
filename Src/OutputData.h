
#pragma once

#include <nstd/String.h>
#include <nstd/List.h>
#include <nstd/HashMap.h>
#include <nstd/Array.h>
#include <nstd/Map.h>
#include <nstd/Variant.h>

class Generator;

class OutputData
{
public:
  class EnvironmentInfo
  {
  public:
    bool verbatim;
  };

  class Segment
  {
  public:
    Segment(int indent) : valid(true), indent(indent), parent(0) {}

  public:
    virtual ~Segment() {};
    virtual bool merge(Segment& segment, bool newParagraph) = 0;
    virtual String generate(Generator& generator) const = 0;

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
    String text;
  public:
    ParagraphSegment(int indent, const String& text) : Segment(indent), text(text) {}
    const String& getText() const {return text;}
    void setText(const String& text) {this->text = text;}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class TitleSegment : public Segment
  {
  public:
    int level;
    String title;
    Map<String, Variant> arguments;
  public:
    TitleSegment(int indent, int level) : Segment(indent), level(level) {}
    bool parseArguments(const String& title, String& error);
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class SeparatorSegment : public Segment
  {
  public:
    SeparatorSegment(int indent) : Segment(indent), lines(1) {}
    int getLines() const {return lines;}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  private:
    int lines;
  };

  class FigureSegment : public Segment
  {
  public:
    String title;
    String path;
    Map<String, Variant> arguments;
  public:
    FigureSegment(int indent, const String& title, const String& path) : Segment(indent), title(title), path(path) {}
    bool parseArguments(const String& line, String& error);
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class RuleSegment : public Segment
  {
  public:
    RuleSegment(int indent) : Segment(indent) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class BulletListSegment : public Segment
  {
  public:
    List<BulletListSegment*> siblingSegments;
    List<Segment*> childSegments;
    char symbol;
    int childIndent;
  public:
    BulletListSegment(int indent, char symbol, uint childIndent) : Segment(indent), symbol(symbol), childIndent(childIndent) {}
    ~BulletListSegment();
    char getSymbol() const {return symbol;}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class NumberedListSegment : public Segment
  {
  public:
    List<NumberedListSegment*> siblingSegments;
    List<Segment*> childSegments;
    uint number;
    int childIndent;
  public:
    NumberedListSegment(int indent, uint number, uint childIndent) : Segment(indent), number(number), childIndent(childIndent) {}
    ~NumberedListSegment();
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class BlockquoteSegment : public Segment
  {
  public:
    List<BlockquoteSegment*> siblingSegments;
    List<Segment*> childSegments;
    int childIndent;
  public:
    BlockquoteSegment(int indent, uint childIndent) : Segment(indent), childIndent(childIndent) {}
    ~BlockquoteSegment();
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class EnvironmentSegment : public Segment
  {
  public:
    int backticks;
    bool verbatim;
    String language;
    Map<String, Variant> arguments;
    List<String> lines;
    List<Segment*> segments;
  public:
    EnvironmentSegment(int indent, int backticks) : Segment(indent), backticks(backticks), verbatim(true) {}
    ~EnvironmentSegment();
    void addLine(const String& line) {lines.append(line);}
    bool parseArguments(const String& line, const HashMap<String, EnvironmentInfo>& knownEnvironments, String& error);
    bool isVerbatim() const {return verbatim;}
    int getBackticks() const {return backticks;}
    void swapSegments(List<Segment*>& segments) {this->segments.swap(segments);}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class TableSegment : public Segment
  {
  public:
    class ColumnData
    {
    public:
      int indent;
      String text;
    };
    class CellData
    {
    public:
      List<Segment*> segments;
    };
    class RowData
    {
    public:
      Array<CellData> cellData;
    };
    class ColumnInfo
    {
    public:
      enum Alignment
      {
        undefinedAlignment,
        leftAlignment,
        rightAlignment,
        centerAlignment
      };
    public:
      int indent;
      Alignment alignment;
      Map<String, Variant> arguments;
      ColumnInfo(int indent) : indent(indent), alignment(undefinedAlignment) {}
    };
  public:
    bool isSeparatorLine;
    Array<ColumnInfo> columns;
    List<RowData> rows;
    ParagraphSegment* captionSegment;
    Map<String, Variant> arguments;
  public:
    TableSegment(int indent) : Segment(indent), isSeparatorLine(false), captionSegment(0) {}
    ~TableSegment();
    bool parseArguments(const String& title, List<ColumnData>& columns, String& error);
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class TexSegment : public Segment
  {
  public:
    String content;
  public:
    TexSegment(const String& content) : Segment(0), content(content) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class TexPartSegment : public Segment
  {
  public:
    String title;
  public:
    TexPartSegment(const String& title) : Segment(0), title(title) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class PdfSegment : public Segment
  {
  public:
    String filePath;
  public:
    PdfSegment(const String& filePath) : Segment(0), filePath(filePath) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

public:
  String className;
  List<String> headerTexFiles;
  bool hasPdfSegments;
  List<Segment*> segments;
  HashMap<String, EnvironmentInfo> environments;
  HashMap<String, String> variables;

public:
  OutputData() : hasPdfSegments(false) {}
  ~OutputData();
};
