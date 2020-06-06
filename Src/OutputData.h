
#pragma once

#include <nstd/String.hpp>
#include <nstd/List.hpp>
#include <nstd/HashMap.hpp>
#include <nstd/Array.hpp>
#include <nstd/Map.hpp>
#include <nstd/Variant.hpp>
#include <nstd/RefCount.hpp>

class Generator;

struct OutputData
{
  enum OutputFormat
  {
    plainFormat,
    texFormat,
    htmlFormat,
  };

  struct EnvironmentInfo
  {
    bool verbatim;
    String command;
  };

  class Segment : public RefCount::Object
  {
  public:
    Segment* _parent;

  public:
    Segment(int indent) : _valid(true), _indent(indent), _parent(0) {}

  public:
    virtual ~Segment() {};
    virtual bool merge(Segment& segment, bool newParagraph) = 0;
    virtual String generate(Generator& generator) const = 0;

    int getIndent() const {return _indent;}

    bool isValid() const {return _valid;}
    void invalidate() {_valid = false;}

  protected:
    bool _valid;
    int _indent;
    
  };

  class ParagraphSegment : public Segment
  {
  public:
    String _text;
  public:
    ParagraphSegment(int indent, const String& text) : Segment(indent), _text(text) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class TitleSegment : public Segment
  {
  public:
    int _level;
    String _title;
    Map<String, Variant> _arguments;
  public:
    TitleSegment(int indent, int level) : Segment(indent), _level(level) {}
    bool parseArguments(const String& title, String& error);
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class SeparatorSegment : public Segment
  {
  public:
    SeparatorSegment(int indent) : Segment(indent) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class FigureSegment : public Segment
  {
  public:
    String _title;
    String _path;
    Map<String, Variant> _arguments;
  public:
    FigureSegment(int indent, const String& title, const String& path) : Segment(indent), _title(title), _path(path) {}
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
    List<BulletListSegment*> _siblingSegments;
    List<Segment*> _childSegments;
    char _symbol;
    int _childIndent;
  public:
    BulletListSegment(int indent, char symbol, uint childIndent) : Segment(indent), _symbol(symbol), _childIndent(childIndent) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class NumberedListSegment : public Segment
  {
  public:
    List<NumberedListSegment*> _siblingSegments;
    List<Segment*> _childSegments;
    uint _number;
    int _childIndent;
  public:
    NumberedListSegment(int indent, uint number, uint childIndent) : Segment(indent), _number(number), _childIndent(childIndent) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class BlockquoteSegment : public Segment
  {
  public:
    List<BlockquoteSegment*> _siblingSegments;
    List<Segment*> _childSegments;
    int _childIndent;
  public:
    BlockquoteSegment(int indent, uint childIndent) : Segment(indent), _childIndent(childIndent) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class EnvironmentSegment : public Segment
  {
  public:
    int _backticks;
    bool _verbatim;
    String _command;
    String _language;
    Map<String, Variant> _arguments;
    List<Segment*> _segments;
    List<String> _lines;
  public:
    EnvironmentSegment(int indent, int backticks) : Segment(indent), _backticks(backticks), _verbatim(true) {}
    bool parseArguments(const String& line, const HashMap<String, EnvironmentInfo>& knownEnvironments, String& error);
    bool process(OutputData::OutputFormat format, String& error);
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  private:
      List<RefCount::Ptr<Segment>> _allocatedSegments;
      friend class Parser;
  };

  class TableSegment : public Segment
  {
  public:
    struct CellData
    {
      List<OutputData::Segment*> outputSegments;
      List<OutputData::Segment*> segments;
    };
    struct RowData
    {
      Array<CellData> cellData;
    };
    struct ColumnInfo
    {
      enum Alignment
      {
        undefinedAlignment,
        leftAlignment,
        rightAlignment,
        centerAlignment
      };

      int indent;
      Alignment alignment;
      Map<String, Variant> arguments;
      String text;
      ColumnInfo(int indent) : indent(indent), alignment(undefinedAlignment) {}
    };
    enum Type
    {
      PipeTable,
      GridTable,
    };
  public:
    Type _tableType;
    bool _isSeparatorLine;
    Array<ColumnInfo> _columns;
    List<RowData> _rows;
    ParagraphSegment* _captionSegment;
    Map<String, Variant> _arguments;
    bool _forceNewRowNextMerge;
  public:
    TableSegment(int indent) : Segment(indent), _tableType(PipeTable), _isSeparatorLine(false), _captionSegment(0), _forceNewRowNextMerge(false) {}
    bool parseArguments(const String& line, String& error);
  public:
    bool merge(Segment& segment, bool newParagraph) override;
    String generate(Generator& generator) const override;
  };

  class TexSegment : public Segment
  {
  public:
    String _content;
  public:
    TexSegment(const String& content) : Segment(0), _content(content) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class TexPartSegment : public Segment
  {
  public:
    String _title;
  public:
    TexPartSegment(const String& title) : Segment(0), _title(title) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  class PdfSegment : public Segment
  {
  public:
    String _filePath;
  public:
    PdfSegment(const String& filePath) : Segment(0), _filePath(filePath) {}
  public:
    bool merge(Segment& segment, bool newParagraph) override {return false;}
    String generate(Generator& generator) const override;
  };

  OutputFormat format;
  String className;
  List<String> headerTexFiles;
  bool hasPdfSegments;
  List<Segment*> segments;
  HashMap<String, EnvironmentInfo> environments;
  HashMap<String, String> variables;

  OutputData() : format(plainFormat), hasPdfSegments(false) {}

private:
  List<RefCount::Ptr<Segment>> allocatedSegments;

  friend class Parser;
};
