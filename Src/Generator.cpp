
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

  if(!file.write(String("\\documentclass[a4paper]{") + className + "}\n"))
    
     //!file.write("\\directlua{print(texconfig.max_print_line)}"))
     //!file.write("\\usepackage[utf8]{inputenc}\n"))
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
  String result(str.length());
  char_t c;
  for(const char_t* i = str, * end = i + str.length(); i < end; ++i)
  {
    switch(c = *i)
    {
    case '\\':
      result.append("{\\textbackslash}");
      break;
    case '<':
      result.append("{\\textless}");
      break;
    case '>':
      result.append("{\\textgreater}");
      break;
    case '_':
      result.append("{\\_\\-}");
      break;
    case '-':
      result.append("-{}");
      break;
    case '$':
    case '%':
    case '}':
    case '&':
    case '#':
    case '{':
      result.append('\\');
      result.append(c);
      break;
    default:
      result.append(c);
      break;
    }
  }
  return result;
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
  String result("\\begin{itemize}%\n\\item ");
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
    result.append((*i)->generate());
  for(List<ListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
  {
    ListSegment* siblingSegment = *i;
    result.append("\\item ");
    for(List<Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
      result.append((*i)->generate());
  }
  result.append("\\end{itemize}\n");
  return result;
}

String OutputData::CodeSegment::generate() const
{
  String result("\n\\begin{verbatim}\n");
  for(List<String>::Iterator i = lines.begin(), end = lines.end(); i != end; ++i)
  {
    result.append(*i);
    result.append("\n");
  }
  result.append("\\end{verbatim}\n");
  return result;
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
