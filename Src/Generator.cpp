
#include <nstd/File.h>
#include <nstd/Error.h>

#include "Generator.h"
#include "OutputData.h"

bool_t Generator::generate(const OutputData& outputData, const String& outputFile)
{
  File file;
  if(!file.open(outputFile, File::writeFlag))
    return false;

  String className = outputData.className;
  if(className.isEmpty())
    className = "article";

  if(!file.write(String("\\documentclass[a4paper]{") + className + "}\n") ||
     !file.write("\\usepackage[utf8]{inputenc}\n"))
     return false;

  if(outputData.hasPdfSegments)
    if(!file.write("\\usepackage{pdfpages}\n"))
      return false;
  if(!file.write("\n"))
    return false;
  for(List<String>::Iterator i = outputData.headerTexFiles.begin(), end = outputData.headerTexFiles.end(); i != end; ++i)
    if(!file.write(*i) ||
       !file.write("\n"))
       return false;
  if(!file.write("\n\\begin{document}\n\n") ||
     !file.write(outputData.generate()) ||
     !file.write("\\end{document}\n"))
    return false;

  return true;
}

String Generator::getErrorString() const
{
  return Error::getErrorString();
}

String Generator::texEscape(const String& str)
{
  // todo
  return str;
}

String OutputData::generate() const
{
  String result(segments.size() * 256);
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    result.append((*i)->generate());
  return result;
}

String OutputData::ParagraphSegment::generate() const
{
  String result("\n\n");
  result.append(this->text);
  result.append('\n');
  return result;
}

String OutputData::SeparatorSegment::generate() const
{
  // todo
  return String();
}

String OutputData::TitleSegment::generate() const
{
  // todo
  return String();
}

String OutputData::RuleSegment::generate() const
{
  // todo
  return String();
}

String OutputData::ListSegment::generate() const
{
  // todo
  return String();
}

String OutputData::CodeSegment::generate() const
{
  // todo
  return String();
}

String OutputData::TexSegment::generate() const
{
  // todo
  return String();
}

String OutputData::TexTocSegment::generate() const
{
  // todo
  return String();
}

String OutputData::TexPartSegment::generate() const
{
  // todo
  return String();
}

String OutputData::PdfSegment::generate() const
{
  // todo
  return String();
}
