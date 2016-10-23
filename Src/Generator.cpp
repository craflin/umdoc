
#include <nstd/Debug.h>
#include <nstd/File.h>
#include <nstd/Directory.h>
#include <nstd/Error.h>

#include "Generator.h"
#include "OutputData.h"

bool Generator::generate(const String& engine, const OutputData& outputData, const String& outputFile)
{
  this->outputData = &outputData;

  File file;
  if(!file.open(outputFile, File::writeFlag))
    return false;

  if(!outputData.className.isEmpty())
  {
    if(!file.write(String("\\documentclass{") + outputData.className + "}\n\n"))
      return false;
  }
  else
  {
    if(!file.write(String("\\documentclass[a4paper]{article}\n\n")))
      return false;

    if(engine == "pdflatex")
      if(!file.write("\\usepackage[utf8]{inputenc}\n\n"))
        return false;

    // inword wrap for english
    if(!file.write("\\usepackage[english]{babel}\n\n"))
       return false;

    // change page geometry
    if(!file.write("\\usepackage{geometry}\n") ||
       !file.write("\\geometry{\n") ||
       !file.write(" a4paper,\n") ||
       !file.write(" left=25mm,\n") ||
       !file.write(" right=25mm,\n") ||
       !file.write(" top=38mm,\n") ||
       !file.write(" bottom=25mm,\n") ||
       !file.write("}\n\n"))
       return false;

    // we need hyperlinks in toc and for \href
    if(!file.write("\\usepackage{hyperref}\n") ||
       !file.write("\\hypersetup{%\n") ||
       !file.write(" colorlinks,\n") ||
       !file.write(" citecolor=black,\n") ||
       !file.write(" filecolor=black,\n") ||
       !file.write(" linkcolor=black,\n") ||
       !file.write(" urlcolor=blue\n") ||
       !file.write("}\n\n"))
       return false;

    // package to include images
    if(!file.write("\\usepackage{graphicx}\n\n"))
       return false;

    // customized enumerations
    if(!file.write("\\usepackage{enumitem}\n") ||
       !file.write("\\setlist{topsep=0pt}\n\n"))
      return false;

    // customized blockquotes
    if(!file.write("\\usepackage{quoting}\n\n"))
      return false;

    // mordern fonts
    if(!file.write("\\usepackage[default,osf]{sourcesanspro}\n") ||
       !file.write("\\usepackage[scaled=.95]{sourcecodepro}\n\n"))
       return false;

    // change parindent and parskip
    if(!file.write("\\setlength\\parindent{0pt}\n") ||
       !file.write("\\setlength\\parskip{5pt}\n\n"))
       return false;

    // package insert graphics at the desired position
    if(!file.write("\\usepackage{float}\n\n"))
      return false;

    // package for pretty code and preformated blocks
    if(!file.write("\\usepackage{listings}\n\n"))
      return false;

    // package to use colored fonts
    if(!file.write("\\usepackage{xcolor}\n") ||
       !file.write("\\definecolor{boxBackgroundColor}{RGB}{230,230,230}\n") ||
       !file.write("\\definecolor{boxFrameColor}{RGB}{128,128,128}\n"))
      return false;

    // command to insert a horizontal rule
    if(!file.write("\\newcommand\\HorizontalRule{\\vspace{-3pt}\\rule{\\linewidth}{0.4pt}\\vspace{4pt}}\n\n"))
      return false;

    // command to insert an image into text
    if(!file.write("\\newcommand\\InlineImage[1]{\\raisebox{-0.1em}{\\includegraphics[height=0.9em]{#1}}}\n\n"))
      return false;

     
    if(!file.write("\\lstnewenvironment{plain}{\\lstset{frame=single,basicstyle=\\ttfamily,breaklines=true,showstringspaces=false,backgroundcolor=\\color{boxBackgroundColor},rulecolor=\\color{boxFrameColor}}\\vspace{\\parskip}\\minipage{\\linewidth}}{\\endminipage}\n\n"))
      return false;

    //if(!file.write("\\renewenvironment{plain}{"
    //               "\\begin{lstlisting}[frame=single,basicstyle=\\ttfamily,breaklines=true,showstringspaces=false,backgroundcolor=\\color{boxBackgroundColor},rulecolor=\\color{boxFrameColor}]"
    //               "}{"
    //               "\\end{lstlisting}"
    //               "}\n"))
    //  return false;

    //if(!file.write("\\newenvironment{latexexample}{"
    //               "\\minipage\\HorizontalRule"
    //               "}{"
    //               "\\HorizontalRule\\endminipage"
    //               "}\n"))
    //  return false;

    if(!file.write("\\newenvironment{latexexample}{\\vspace{\\parskip}\\begin{minipage}{\\linewidth}\\HorizontalRule}{\n\\HorizontalRule\\end{minipage}}\n"))
      return false;

    // package to include pdf pages
    if(outputData.hasPdfSegments)
      if(!file.write("\\usepackage{pdfpages}\n\n"))
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

String Generator::texEscapeChar(char c)
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

bool Generator::matchInlineLink(const char* s, const char* end, const OutputData& outputData, const char*& pos, String& result)
{
  if(*s != '[')
    return false;
  const char* nameStart = ++s;
  while(*s != ']')
    if(++s >= end)
      return false;
  const char* nameEnd = s++;
  if(*s != '(')
    return false;
  const char* linkStart = ++s;
  const char* linkEnd = 0;
  while(*s != ')')
  {
    if(*s == ' ' && !linkEnd)
      linkEnd = s;
    if(++s >= end)
      return false;
  }
  if(!linkEnd)
    linkEnd = s;
  ++s;
  result.append("\\href{");
  result.append(linkStart, linkEnd - linkStart);
  result.append("}{");
  String name;
  name.attach(nameStart, nameEnd - nameStart);
  result.append(texEscape(name, outputData));
  result.append("}");
  pos = s;
  return true;
}

bool Generator::matchInlineImage(const char* s, const char* end,const OutputData& outputData, const char*& pos, String& result)
{
  if(*s != '!')
    return false;
  if(*(++s) != '[')
    return false;
  while(*(++s) != ']')
    if(++s >= end)
      return false;
  if(*(++s) != '(')
    return false;
  const char* pathStart = ++s;
  const char* pathEnd = 0;
  while(*s != ')')
  {
    if(*s == ' ' && !pathEnd)
      pathEnd = s;
    if(++s >= end)
      return false;
  }
  if(!pathEnd)
    pathEnd = s;
  ++s;
  String path;
  path.attach(pathStart, pathEnd - pathStart);
  if(!File::isAbsolutePath(path))
    path = outputData.inputDirectory + "/" + path;
  result.append("\\InlineImage{");
  result.append(File::getRelativePath(outputData.outputDirectory, path));
  result.append("}");
  pos = s;
  return true;
}
/*
String Generator::mardownUnescape(const String& str)
{
  const char* start = str;
  const char* i = String::find(start, '\\');
  if(!i)
    return str;
  String result(str.length());
  result.append(str.substr(0, i - start));
  const char* end = start + str.length();
  for(; i < end; ++i)
    if(*i == '\\')
    {
      if(i + 1 < end && String::find("\\`*_{}[]()#+-.!", *(i + 1)))
        ++i;
      result.append(*i);
    }
  return result;
}
*/
String Generator::texEscape(const String& str, const OutputData& outputData)
{
  String result(str.length());
  char c;
  String endSequence;
  List<String> endSequenceStack;
  bool ignoreSingleBacktick = false;
  for(const char* start = str, * i = start, * end = start + str.length(); i < end;)
  {
    switch(c = *i)
    {
    case '\\':
      if(i + 1 < end && String::find("\\`*_{}[]()#+-.!", *(i + 1)))
        ++i;
      result.append(texEscapeChar(*i));
      ++i;
      break;
    default:
      if(!endSequence.isEmpty() && String::compare(i, endSequence, endSequence.length()) == 0)
      {
        if(*(const char*)endSequence == '*' || *(const char*)endSequence == '_')
        {
          if(String::find(" \t", i[endSequence.length()]) && (i == start || String::find(" \t", i[-1])))
          { // "[...] if you surround an * or _ with spaces, it’ll be treated as a literal asterisk or underscore."
            for(size_t j = 0; j < endSequence.length(); ++j)
              result.append(texEscapeChar(*(const char*)endSequence));
            i += endSequence.length();
            continue;
          }
        }

        if(*(const char*)endSequence == '`')
          while(!result.isEmpty() && String::find(" \t", ((const char*)result)[result.length() - 1]))
            result.resize(result.length() - 1);

        result.append("}");
        i += endSequence.length();
        if(endSequence == "``")
          ignoreSingleBacktick = false;
        if(endSequenceStack.isEmpty())
          endSequence.clear();
        else
          endSequence = endSequenceStack.back(), endSequenceStack.removeBack();
        continue;
      }
      if(c == '*' || c == '_' || c == '`')
      {
        String sequence;
        sequence.attach(i, i[1] == c ? 2 : 1);

        if(c == '*' || c == '_')
        {
          if(String::find(" \t", i[sequence.length()]) && (i == start || String::find(" \t", i[-1])))
          { // "[...] if you surround an * or _ with spaces, it’ll be treated as a literal asterisk or underscore."
            for(size_t j = 0; j < sequence.length(); ++j)
              result.append(texEscapeChar(c));
            i += sequence.length();
            continue;
          }
        }

        if(c == '`' && ignoreSingleBacktick)
        {
          result.append(texEscapeChar(c));
          ++i;
          continue;
        }

        i += sequence.length();
        if(c == '`')
        {
          result.append("\\texttt{");
          if(sequence.length() > 1)
            ignoreSingleBacktick = true;
          while(String::find(" \t", *i))
            ++i;
        }
        else
          result.append(sequence.length() == 2 ?  String("\\textbf{") : String("\\emph{"));
        if(!endSequence.isEmpty())
          endSequenceStack.append(endSequence);
        endSequence = sequence;
        continue;
      }
      if(matchInlineLink(i, end, outputData, i, result))
        continue;
      if(matchInlineImage(i, end, outputData, i, result))
        continue;
      result.append(texEscapeChar(c));
      ++i;
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
    result.append(segment->generate(*this));
  }
  return result;
}

String OutputData::SeparatorSegment::generate(const OutputData& outputData) const
{
  return String();
}

String OutputData::ParagraphSegment::generate(const OutputData& outputData) const
{
  return String("\n") + Generator::texEscape(text, outputData) + "\n";
}

String OutputData::TitleSegment::generate(const OutputData& outputData) const
{
  switch(level)
  {
  case 1:
    return String("\n\\section{") + Generator::texEscape(title, outputData) + "}\n";
    break;
  case 2:
    return String("\n\\subsection{") + Generator::texEscape(title, outputData) + "}\n";
    break;
  case 3:
    return String("\n\\subsubsection{") + Generator::texEscape(title, outputData) + "}\n";
    break;
  case 4:
    return String("\n\\paragraph{") + Generator::texEscape(title, outputData) + "}\n";
    break;
  case 5:
    return String("\n\\subparagraph{") + Generator::texEscape(title, outputData) + "}\n";
    break;
  default:
    ASSERT(false);
  }
  return String();
}

String OutputData::RuleSegment::generate(const OutputData& outputData) const
{
  return String("\n\\HorizontalRule\n");
}

String OutputData::BulletListSegment::generate(const OutputData& outputData) const
{
  String result("\n\\begin{itemize}\n\\item ");
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
  {
    const Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate(outputData));
  }
  for(List<BulletListSegment*>::Iterator i = siblingSegments.begin(), end = siblingSegments.end(); i != end; ++i)
  {
    BulletListSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    result.append("\\item ");
    for(List<Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
    {
      const Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(outputData));
    }
  }
  result.append("\\end{itemize}\n");
  return result;
}

String OutputData::NumberedListSegment::generate(const OutputData& outputData) const
{
  String result("\n\\begin{enumerate}\n\\item ");
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
  {
    const Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate(outputData));
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
      result.append(segment->generate(outputData));
    }
  }
  result.append("\\end{enumerate}\n");
  return result;

}

String OutputData::BlockquoteSegment::generate(const OutputData& outputData) const
{
  String result("\n\\begin{quoting}\n");
  for(List<Segment*>::Iterator i = childSegments.begin(), end = childSegments.end(); i != end; ++i)
  {
    const Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate(outputData));
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
      result.append(segment->generate(outputData));
    }
  }
  result.append("\n\\end{quoting}\n");
  return result;
}

String OutputData::EnvironmentSegment::generate(const OutputData& outputData) const
{
  String environment = language;
  if(environment.isEmpty())
    //environment = "verbatim";
    environment = "plain";

  String result;
  //result.append("\n\\vspace{\\parskip}\\begin{minipage}{\\linewidth}\n");
  result.append(String("\n\\begin{") + environment + "}\n");
  //String result(String("\\begin{lstlisting}[frame=single,basicstyle=\\ttfamily,breaklines=true,showstringspaces=false,backgroundcolor=\\color{white},rulecolor=\\color{white}]\n"));
  if(verbatim)
  {
    for(List<String>::Iterator i = lines.begin(), end = lines.end(); i != end; ++i)
    {
      result.append(*i);
      result.append("\n");
    }
  }
  else
  {
    for(List<Segment*>::Iterator i = segments.begin(), end = segments.end(); i != end; ++i)
    {
      const Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(outputData));
    }
  }
  result.append(String("\\end{") + environment + "}\n");
  //result.append("\\end{minipage}\n");
  //result.append("\\end{lstlisting}");
  return result;
}

String OutputData::TexSegment::generate(const OutputData& outputData) const
{
  return content + "\n";
}

String OutputData::TexTocSegment::generate(const OutputData& outputData) const
{
  return String("\n\\pagestyle{empty}\n\\tableofcontents\n");
}

String OutputData::TexPartSegment::generate(const OutputData& outputData) const
{
  return String("\n\\clearpage\n\\part{") + Generator::texEscape(title, outputData) + "}\n";
}

String OutputData::PdfSegment::generate(const OutputData& outputData) const
{
  String path = filePath;
  if(!File::isAbsolutePath(path))
    path = outputData.inputDirectory + "/" + path;
    return String("\n\\includepdf[pages=-]{") + File::getRelativePath(outputData.outputDirectory, path) + "}\n";
}
