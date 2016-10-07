
#include <nstd/Debug.h>
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
     !file.write("\n\\end{document}\n"))
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
  return String("\n") + Generator::texEscape(text) + "\n";
}

String OutputData::SeparatorSegment::generate() const
{
  // todo
  return String();
}

String OutputData::TitleSegment::generate() const
{
  switch(level)
  {
  case 1:
    return String("\n\\section{") + Generator::texEscape(title) + "}\n";
    break;
  case 2:
    return String("\n\\subsection{") + Generator::texEscape(title) + "}\n";
    break;
  case 3:
    return String("\n\\subsubsection{") + Generator::texEscape(title) + "}\n";
    break;
  case 4:
    return String("\n\\paragraph{") + Generator::texEscape(title) + "}\n";
    break;
  case 5:
    return String("\n\\subparagraph{") + Generator::texEscape(title) + "}\n";
    break;
  default:
    ASSERT(false);
  }
  return String();
}

String OutputData::RuleSegment::generate() const
{
  return String("\n\\rule{\\textwidth}{1pt}\n");
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
  return content + "\n";
}

String OutputData::TexTocSegment::generate() const
{
  return String("\n\\pagestyle{empty}\n\\tableofcontents\n");
}

String OutputData::TexPartSegment::generate() const
{
  return String("\n\\part{") + Generator::texEscape(title) + "}\n";
}

String OutputData::PdfSegment::generate() const
{
  return String("\n\\includepdf[pages=-]{") + filePath + "}\n";
}
