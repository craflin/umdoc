
#include <nstd/Debug.h>
#include <nstd/File.h>
#include <nstd/Error.h>

#include "Generator.h"
#include "OutputData.h"

bool_t Generator::generate(const String& engine, const OutputData& outputData, const String& outputFile)
{
  File file;
  if(!file.open(outputFile, File::writeFlag))
    return false;

  if(!outputData.className.isEmpty())
  {
    if(!file.write(String("\\documentclass{") + outputData.className + "}\n"))
      return false;
  }
  else
  {
    if(!file.write(String("\\documentclass[a4paper]{article}\n")))
      return false;

    if(engine == "pdflatex")
      if(!file.write("\\usepackage[utf8]{inputenc}\n"))
        return false;

    if(!file.write("\\usepackage[english]{babel}\n"))
       return false;

    // change parindent and parskip
    if(!file.write("\\setlength\\parindent{0pt}\n") ||
       !file.write("\\setlength\\parskip{5pt}\n"))
       return false;

    // change page geometry
    if(!file.write("\\usepackage{geometry}\n") ||
       !file.write("\\geometry{\n") ||
       !file.write(" a4paper,\n") ||
       !file.write(" left=25mm,\n") ||
       !file.write(" right=25mm,\n") ||
       !file.write(" top=38mm,\n") ||
       !file.write(" bottom=25mm,\n") ||
       !file.write("}\n"))
       return false;

    // we need hyperlinks in toc
    if(!file.write("\\usepackage{hyperref}\n") ||
       !file.write("\\hypersetup{%\n") ||
       !file.write(" colorlinks,\n") ||
       !file.write(" citecolor=black,\n") ||
       !file.write(" filecolor=black,\n") ||
       !file.write(" linkcolor=black,\n") ||
       !file.write(" urlcolor=blue\n") ||
       !file.write("}\n"))
       return false;

    // customized enumerations
    if(!file.write("\\usepackage{enumitem}\n") ||
       !file.write("\\setlist{topsep=0pt}\n"))
      return false;

    // customized blockquotes
    if(!file.write("\\usepackage{quoting}\n"))
      return false;

    // change default font, you can overwrite this in your own own tex header file if you want
    //if(!file.write("\\usepackage{sourcecodepro}\n") ||
    //   !file.write("\\renewcommand*\\rmdefault{phv}\n"))
    //   return false;

    // mordern fonts
    if(!file.write("\\usepackage[default,osf]{sourcesanspro}\n") ||
       !file.write("\\usepackage[scaled=.95]{sourcecodepro}\n"))
       return false;

    // command to insert a horizontal rule
    if(!file.write("\\newcommand\\fullrule{\\vspace{-3pt}\\rule{\\textwidth}{0.4pt}\\vspace{4pt}}\n"))
      return false;

    // package to include pdf pages
    if(outputData.hasPdfSegments)
      if(!file.write("\\usepackage{pdfpages}\n"))
        return false;
  }

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

String Generator::texEscape(char_t c)
{
  switch(c)
  {
    case '\\':
      return "{\\textbackslash}";
    case '<':
      return "{\\textless}";
    case '>':
      return "{\\textgreater}";
    case '_':
      return "{\\_\\-}"; // allow line break after _
    case '-':
      return "-{}"; // do not merge -- into a long -
    case '/':
      return "{/\\-}"; // allow line break after /
    case '$':
    case '%':
    case '}':
    case '&':
    case '#':
    case '{':
      {
        String result("\\");
        result.append(c);
        return result;
      }
    default:
      {
        String result;
        result.append(c);
        return result;
      }
  }
}

String Generator::texEscape(const String& str)
{
  String result(str.length());
  char_t c;
  String endSequence;
  List<String> endSequenceStack;
  for(const char_t* start = str, * i = start, * end = start + str.length(); i < end; ++i)
  {
    switch(c = *i)
    {
    case '\\':
      if(i + 1 < end && String::find("\\`*_{}[]()#+-.!", *(i + 1)))
        ++i;
      result.append(texEscape(*i));
      break;
    default:
      if(!endSequence.isEmpty() && String::compare(i, endSequence, endSequence.length()) == 0)
      {
        if(*(const char_t*)endSequence == '*' || *(const char_t*)endSequence == '_')
        {
          if(String::find(" \t", i[endSequence.length()]) && (i == start || String::find(" \t", i[-1])))
          { // "[...] if you surround an * or _ with spaces, it’ll be treated as a literal asterisk or underscore."
            for(size_t j = 0; j < endSequence.length(); ++j)
              result.append(texEscape(*(const char_t*)endSequence));
            i += endSequence.length() - 1;
            continue;
          }

          result.append("}");
          i += endSequence.length() - 1;
          if(endSequenceStack.isEmpty())
            endSequence.clear();
          else
            endSequence = endSequenceStack.back(), endSequenceStack.removeBack();
          continue;
        }
      }
      if(c == '*' || c == '_')
      {
        String sequence;
        sequence.attach(i, i[1] == c ? 2 : 1);

        if(String::find(" \t", i[sequence.length()]) && (i == start || String::find(" \t", i[-1])))
        { // "[...] if you surround an * or _ with spaces, it’ll be treated as a literal asterisk or underscore."
          for(size_t j = 0; j < sequence.length(); ++j)
            result.append(texEscape(c));
          i += sequence.length() - 1;
          continue;
        }

        result.append(sequence.length() == 2 ?  String("\\textbf{") : String("\\emph{"));
        i += sequence.length() - 1;
        if(!endSequence.isEmpty())
          endSequenceStack.append(endSequence);
        endSequence = sequence;
        continue;
      }
      result.append(texEscape(c));
      break;
    }
  }
  while(!endSequenceStack.isEmpty())
  {
    result.append("}");
    endSequenceStack.removeBack();
  }
  if(!endSequence.isEmpty())
    result.append("}");
  return result;
}

String OutputData::generate() const
{
  String result(segments.size() * 256);
  for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
  {
    const Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append(segment->generate());
  }
  return result;
}

String OutputData::SeparatorSegment::generate() const
{
  return String();
}

String OutputData::ParagraphSegment::generate() const
{
  return String("\n") + Generator::texEscape(text) + "\n";
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
  return String("\n\\fullrule\n");
}

String OutputData::ListSegment::generate() const
{
  String result("\n\\begin{itemize}\n\\item ");
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
  {
    const Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate());
  }
  for(List<ListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
  {
    ListSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    result.append("\\item ");
    for(List<Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
    {
      const Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate());
    }
  }
  result.append("\\end{itemize}\n");
  return result;
}

String OutputData::NumberedListSegment::generate() const
{
  String result("\n\\begin{enumerate}\n\\item ");
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
  {
    const Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate());
  }
  for(List<NumberedListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
  {
    NumberedListSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    result.append("\\item ");
    for(List<Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
    {
      const Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate());
    }
  }
  result.append("\\end{enumerate}\n");
  return result;

}

String OutputData::BlockquoteSegment::generate() const
{
  String result("\n\\begin{quoting}\n");
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
  {
    const Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate());
  }
  for(List<BlockquoteSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
  {
    BlockquoteSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    for(List<Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
    {
      const Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate());
    }
  }
  result.append("\n\\end{quoting}\n");
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
  return String("\n\\clearpage\n\\part{") + Generator::texEscape(title) + "}\n";
}

String OutputData::PdfSegment::generate() const
{
  return String("\n\\includepdf[pages=-]{") + filePath + "}\n";
}
