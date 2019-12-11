
#include "TexGenerator.h"

#include <nstd/Debug.h>
#include <nstd/File.h>
#include <nstd/Directory.h>
#include <nstd/Error.h>

#include "OutputData.h"

const char* TexGenerator::defaultListingsLanguages[] = {"ABAP", "ACM", "ACMscript", "ACSL", "Ada", "Algol", "Ant", "Assembler", "Awk", "bash", "Basic", "C", "C++", "Caml", "CIL", "Clean", "Cobol", "Comal 80", "command.com", "Comsol", "csh", "Delphi", "Eiffel", "Elan", "erlang", "Euphoria", "Fortran", "GCL", "Gnuplot", "hansl", "Haskell", "HTML", "IDL", "inform", "Java", "JVMIS", "ksh", "Lingo", "Lisp", "LLVM", "Logo", "Lua", "make", "Matlab", "Mathematica", "Mercury", "MetaPost", "Miranda", "Mizar", "ML", "Modula-2", "MuPAD", "NASTRAN", "Oberon-2", "OCL", "Octave", "Oz", "Perl", "Pascal", "PHP", "PL/I", "Plasm", "PostScript", "POV", "Prolog", "Promela", "PSTricks", "Python", "R", "Reduce", "Rexx", "RSL", "Ruby", "S", "SAS", "Scala", "Scilab", "sh", "SHELXL", "SPARQL", "Simula", "SQL", "tcl", "TeX", "VBScript", "Verilog", "VHDL", "VRML", "XML", "XSLT"};
const usize TexGenerator::numOfDefaultListingsLanguages = sizeof(defaultListingsLanguages) / sizeof(*defaultListingsLanguages);

bool TexGenerator::generate(const String& engine, const OutputData& outputData, const String& outputFile)
{
  //this->outputData = &outputData;

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

    if(engine != "lualatex")
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
       !file.write(" top=25mm,\n") ||
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
       !file.write(" anchorcolor=black,\n") ||
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
       !file.write("\\usepackage[scaled=.8]{sourcecodepro}\n\n"))
       return false;

    // change parindent and parskip
    if(!file.write("\\setlength\\parindent{0pt}\n") ||
       !file.write("\\setlength\\parskip{5pt}\n\n"))
       return false;

    // customized section titles
    if(!file.write("\\usepackage{titlesec}\n\n") ||
       !file.write("\\setcounter{secnumdepth}{4}\n") ||
       //!file.write("\\titleformat{\\section}{\\fontsize{7em}{7em}\\selectfont\\bfseries}{\\thesection}{0.1em}{\\LARGE\\scshape}\n") ||
       //!file.write("\\titleformat{\\part}{\\fontsize{5em}{5em}\\selectfont\\bfseries}{\\makebox[7.5cm][r]{\\thepart}}{0.8em}{\\LARGE\\scshape}\n") ||
       //!file.write("\\titleformat{\\subsection}{\\normalfont\\Large\\bfseries}{\\thesubsection}{1em}{}\n") ||
       //!file.write("\\titleformat{\\subsubsection}{\\normalfont\\large\\bfseries}{\\thesubsubsection}{1em}{}\n") ||
       !file.write("\\titleformat{\\paragraph}{\\normalfont\\normalsize\\bfseries}{\\theparagraph}{1em}{}\n") ||
       !file.write("\\titleformat{\\subparagraph}{\\normalfont\\normalsize\\bfseries}{\\thesubparagraph}{1em}{}\n\n") ||
       //!file.write("\\titlespacing*{\\part}{-1.5cm}{10pt}{5pt}\n") ||
       //!file.write("\\titlespacing*{\\section}{-1.5cm}{30pt}{5pt}\n") ||
       //!file.write("\\titlespacing*{\\subsection}{-1.5cm}{10pt}{5pt}\n") ||
       //!file.write("\\titlespacing*{\\subsubsection}{-1.5cm}{10pt}{5pt}\n") ||
       //!file.write("\\titlespacing*{\\paragraph}{-1.5cm}{10pt}{5pt}\n") ||
       //!file.write("\\titlespacing*{\\subparagraph}{-0cm}{10pt}{5pt}\n\n"))
       false)
      return false;

    // workaround for titlesec section numbering issue http://tex.stackexchange.com/questions/299969/titlesec-loss-of-section-numbering-with-the-new-update-2016-03-15
    if(!file.write("\\usepackage{etoolbox}\n") ||
       !file.write("\\makeatletter\n") ||
       !file.write("\\patchcmd{\\ttlh@hang}{\\parindent\\z@}{\\parindent\\z@\\leavevmode}{}{}\n") ||
       !file.write("\\patchcmd{\\ttlh@hang}{\\noindent}{}{}{}\n") ||
       !file.write("\\makeatother\n\n"))
      return false;

    // package insert graphics at the desired position
    if(!file.write("\\usepackage{float}\n\n"))
      return false;

    // package for pretty code and preformated blocks
    if(!file.write("\\usepackage{listings}\n\n"))
      return false;

    // package to use colored fonts
    if(!file.write("\\usepackage{xcolor}\n"))
      return false;

    // package for custom environments
    if(!file.write("\\usepackage{environ}\n\n"))
      return false;

    // command to insert a horizontal rule
    //if(!file.write("\\newcommand\\HorizontalRule{\\vspace{-3pt}\\rule{\\linewidth}{0.4pt}\\vspace{4pt}}\n\n"))
    if(!file.write("\\newcommand\\HorizontalRule{\\raisebox{3.5pt}[1.5ex]{\\rule{\\linewidth}{0.4pt}}}\n\n"))
      return false;

    // command to insert an image into text
    if(!file.write("\\newcommand\\InlineImage[1]{\\raisebox{-0.1em}{\\includegraphics[height=0.9em]{#1}}}\n\n"))
      return false;

    // configure table environment
    //if(!file.write("\\renewcommand{\\arraystretch}{1.2}\n\n"))
    //  return false;
    //if(!file.write("\\usepackage{mdframed}\n\n"))
    //  return false;
    if(!file.write("\\usepackage{array}\n"))
      return false;
    //if(!file.write("\\renewcommand{\\arraystretch}{1.5}\n\n"))
    //  return false;
    if(!file.write("\\renewcommand{\\extrarowheight}{2pt}\n\n"))
      return false;

    // package for xtab tables
    if(!file.write("\\usepackage{xtab}\n\n"))
      return false;

    // prepare environments for syntax highlighting
    if(!file.write("\\definecolor{boxBackgroundColor}{RGB}{245,245,245}\n") ||
       !file.write("\\definecolor{boxFrameColor}{RGB}{128,128,128}\n") ||
       !file.write("\\definecolor{codeRedColor}{RGB}{163,21,21}\n") ||
       !file.write("\\definecolor{codeBlueColor}{RGB}{0,0,255}\n") ||
       !file.write("\\definecolor{codeGreenColor}{RGB}{0,128,0}\n"))
       return false;
    if(!file.write("\\lstdefinelanguage{XML}"
"{"
"  basicstyle=\\ttfamily\\color{codeBlueColor},"
"  morestring=[b]\","
"  moredelim=[s][\\color{codeBlueColor}]{<}{\\ },"
"  moredelim=[s][\\color{codeBlueColor}]{</}{>},"
"  morecomment=[s]{<?}{?>},"
"  morecomment=[s]{<!--}{-->},"
"  commentstyle=\\color{codeGreenColor},"
"  stringstyle=\\color{codeRedColor},"
"  identifierstyle=\\color{red}"
"}\n\n"))
      return false;

    if(!file.write("\\newcommand\\EnvironmentCaption[1]{\\parbox{\\textwidth}{\\textbf{#1}}}\n\n"))
      return false;

    if(!file.write("\\lstset{frame=single,basicstyle=\\ttfamily,breaklines=true,showstringspaces=false,backgroundcolor=\\color{boxBackgroundColor},rulecolor=\\color{boxFrameColor},keywordstyle=\\color{codeBlueColor},stringstyle=\\color{codeRedColor},commentstyle=\\color{codeGreenColor}}\n"))
      return false;
    if(!file.write("\\lstnewenvironment{plain}[1][]{\\lstset{#1}\\vspace{\\parskip}\\minipage{\\linewidth}}{\\endminipage}\n") ||
       !file.write("\\lstnewenvironment{xplain}{\\lstset{frame=none,backgroundcolor=}\\vspace{\\parskip}}{}\n"))
      return false;
    for(usize i = 0; i < numOfDefaultListingsLanguages; ++i)
    {
      String language = String::fromCString(defaultListingsLanguages[i]);
      //if(!file.write(String("\\lstnewenvironment{") + TexGenerator::getEnvironmentName(language) + "}{\\lstset{language=" + language + ",frame=single,basicstyle=\//\ttfamily,breaklines=true,showstringspaces=false,backgroundcolor=\\color{boxBackgroundColor},rulecolor=\\color{boxFrameColor}}\\vspace{\\parskip}\\minipage{\\linewidth}}{\\endminipage}\n"))
      //  return false;
      if(!file.write(String("\\lstnewenvironment{") + TexGenerator::getEnvironmentName(language) + "}[1][]{\\lstset{language=" + language + ",#1}\\vspace{\\parskip}\\minipage{\\linewidth}}{\\endminipage}\n"))
        return false;
    }
    if(!file.write("\n"))
        return false;

    // create environment for latex examples
    //if(!file.write("\\newenvironment{boxed}{\\vspace{\\parskip}\\begin{minipage}{\\linewidth}\\HorizontalRule}{\n\\HorizontalRule\\end{minipage}}\n\n"))
    //  return false;
    if(!file.write("\\NewEnviron{boxed}[1][]{\\vspace{\\parskip}\\hspace{-3.4pt}\\fcolorbox{boxFrameColor}{white}{\\minipage{\\linewidth}\n\\vspace{3.3pt}\\BODY\n\\vspace{3.4pt}\\endminipage}\\vspace{3.3pt}}\n\n"))
      return false;
    //if(!file.write("\\newenvironment{boxed}{\\vspace{\\parskip}\\begin{mdframed}[backgroundcolor=yellow!10]\\begin{minipage}{\\linewidth}}{\\end{minipage}\\end{mdframed}}\n\n"))
    //  return false;

    // package to include pdf pages
    if(outputData.hasPdfSegments)
      if(!file.write("\\usepackage{pdfpages}\n\n"))
        return false;

    // customize inline code spans
    if(!file.write("\\let\\oldtexttt\\texttt\n") ||
      !file.write("\\renewcommand{\\texttt}[1]{\\fcolorbox{boxFrameColor}{boxBackgroundColor}{\\raisebox{0pt}[0.45em][0pt]{\\oldtexttt{#1}}}}\n\n"))
       return false;
  }

  if(!file.write("\n"))
    return false;
  for(List<String>::Iterator i = outputData.headerTexFiles.begin(), end = outputData.headerTexFiles.end(); i != end; ++i)
    if(!file.write(*i) ||
       !file.write("\n"))
       return false;
  if(!file.write("\n\\begin{document}\n\n") ||
     !file.write(generate(outputData)) ||
     !file.write("\n\\end{document}\n"))
    return false;

  return true;
}

String TexGenerator::getErrorString() const
{
  return Error::getErrorString();
}

String TexGenerator::texEscapeChar(char c)
{
  switch(c)
  {
    case '\\':
      return String("{\\textbackslash}");
    case '<':
      return String("{\\textless}");
    case '>':
      return String("{\\textgreater}");
    case '_':
      return String("{\\_\\allowbreak}"); // allow line break after _
    case '-':
      return String("{-\\allowbreak}"); // do not merge -- into a long - // don't use \textendash!
    case '/':
      return String("{/\\allowbreak}"); // allow line break after /
    case '^':
      return String("\\textasciicircum");
    case '~':
      return String("{\\textasciitilde}");
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

bool TexGenerator::matchInlineLink(const char* s, const char* end, const char*& pos, String& result)
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
  String link;
  link.attach(linkStart, linkEnd - linkStart);
  String name;
  name.attach(nameStart, nameEnd - nameStart);
  if(link.startsWith("#"))
  {
    if(name.isEmpty())
    {
      result.append("\\ref{");
      result.append(link.substr(1));
      result.append("}");
    }
    else
    {
      result.append("\\hyperref[");
      result.append(link.substr(1));
      result.append("]{");
      if (name.endsWith("#"))
      {
        name.resize(name.length() - 1);
        name = texEscape(name);
        name.append(String("\\ref{") + link.substr(1) + "}");
      }
      else
        name = texEscape(name);
      name.replace(' ', '~');
      result.append(name);
      result.append("}");
    }
  }
  else
  {
    result.append("\\href{");
    result.append(link);
    result.append("}{");
    if(name.isEmpty())
    {
      result.append("\\mbox{");
      result.append(texEscape(link));
      result.append("}");
    }
    else
      result.append(texEscape(name));
    result.append("}");
  }
  pos = s;
  return true;
}

bool TexGenerator::matchInlineImage(const char* s, const char* end, const char*& pos, String& result)
{
  if(*s != '!')
    return false;
  if(*(++s) != '[')
    return false;
  ++s;
  while(*s != ']')
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
  result.append("\\InlineImage{");
  result.append(path);
  result.append("}");
  pos = s;
  return true;
}

bool TexGenerator::matchLineBreak(const char* s, const char* end, const char*& pos, String& result)
{
  if(*(s++) != '<')
    return false;
  while(String::isSpace(*s) && s < end)
    ++s;
  if(*(s++) != 'b')
    return false;
  if(*(s++) != 'r')
    return false;
  while(String::isSpace(*s) && s < end)
    ++s;
  if(*(s++) != '/')
    return false;
  if(*(s++) != '>')
    return false;
  pos = s;
  result.append("\\newline ");
  return true;
}

bool TexGenerator::matchInlineFootnote(const char* s, const char* end, const char*& pos, String& result)
{
  if(*(s++) != '[')
    return false;
  if(*(s++) != '^')
    return false;
  const char* textStart = s;
  while(*s != ']')
  {
    if(++s >= end)
      return false;
  }
  const char* textEnd = s++;
  String text;
  text.attach(textStart, textEnd - textStart);
  pos = s;
  result.append("\\footnote{");
  result.append(text);
  result.append("}");
  return true;
}

String TexGenerator::texEscape(const String& str)
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
          if((String::find(" \t", i[endSequence.length()]) && (i == start || String::find(" \t", i[-1]))) ||
            (*(const char*)endSequence == '_' && String::isAlphanumeric(i[endSequence.length()])))
          { // "[...] if you surround an * or _ with spaces, it�ll be treated as a literal asterisk or underscore."
            for(usize j = 0; j < endSequence.length(); ++j)
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
          if((String::find(" \t", i[sequence.length()]) && (i == start || String::find(" \t", i[-1]))) ||
            (c == '_' && i != start && String::isAlphanumeric(i[-1])))
          { // "[...] if you surround an * or _ with spaces, it�ll be treated as a literal asterisk or underscore."
            for(usize j = 0; j < sequence.length(); ++j)
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
      if(matchInlineLink(i, end, i, result))
        continue;
      if(matchInlineImage(i, end, i, result))
        continue;
      if(matchLineBreak(i, end, i, result))
        continue;
      if(matchInlineFootnote(i, end, i, result))
        continue;
      if(c == ':' && String::isAlpha(i[1]) && i > start && !String::isAlphanumeric(i[-1]))
        result.append(texEscapeChar(c) + "{\\allowbreak}");  // allow line breaks after e.g. "::"
      else if(String::isAlpha(c) && String::isLowerCase(c) && String::isAlpha(i[1]) && String::isUpperCase(i[1]))
        result.append(texEscapeChar(c) + "\\-");  // allow line breaks in camel case
      else if(String::find("<({[", c) && i > start && String::isAlphanumeric(i[-1]))
        result.append(String("{\\allowbreak}") + texEscapeChar(c));  // allow line breaks before <, (, { or [
      else
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

String TexGenerator::getEnvironmentName(const String& language)
{
  String result(language.length());
  for(const char* i = language; *i; ++i)
    switch(*i)
    {
    case '+':
      result.append("plus");
      break;
    default:
      if(String::isAlphanumeric(*i))
        result.append(String::toLowerCase(*i));
    }
  if(result.length() == 1)
    result.append("language");
  return result;
}

String TexGenerator::getTexSize(const String& size, bool width)
{
  if(size.endsWith("%"))
  {
    double percentageSize = size.toDouble() / 100.0;
    return String::fromDouble(percentageSize) + (width ? String("\\textwidth") : String("\\textheight"));
  }
  else
    return size;
}

String TexGenerator::generate(const OutputData& data)
{
  String result(data.segments.size() * 256);
  for(List<OutputData::Segment*>::Iterator i = data.segments.begin(), end = data.segments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append(segment->generate(*this));
  }
  return result;
}

String TexGenerator::generate(const OutputData::SeparatorSegment& segment)
{
  return String();
}

String TexGenerator::generate(const OutputData::FigureSegment& segment)
{
  String path = segment.path;
  List<String> flagsList;
  String label = segment.arguments.find("#")->toString();
  if(!label.isEmpty())
      label = String("\\label{") + label + "}";
  {
    String width = segment.arguments.find("width")->toString();
    if(!width.isEmpty())
      flagsList.append(String("width=") + TexGenerator::getTexSize(width));
  }
  {
    String height = segment.arguments.find("height")->toString();
    if(!height.isEmpty())
      flagsList.append(String("height=") + TexGenerator::getTexSize(height, false));
  }
  String flags;
  flags.join(flagsList, ',');
  return String("\n\\begin{figure}[H]\\centering\\includegraphics[") + flags + "]{" + path + "}\\caption{" + TexGenerator::texEscape(segment.title) + "}" + label + "\\end{figure}\n";
}

String TexGenerator::generate(const OutputData::ParagraphSegment& segment)
{
  return String("\n") + TexGenerator::texEscape(segment.text) + "\n";
}

String TexGenerator::generate(const OutputData::TitleSegment& segment)
{
  String result;
  switch(segment.level)
  {
  case 1:
    result = String("\n\\section{") + TexGenerator::texEscape(segment.title) + "}\n";
    break;
  case 2:
    result = String("\n\\subsection{") + TexGenerator::texEscape(segment.title) + "}\n";
    break;
  case 3:
    result = String("\n\\subsubsection{") + TexGenerator::texEscape(segment.title) + "}\n";
    break;
  case 4:
    result = String("\n\\paragraph{") + TexGenerator::texEscape(segment.title) + "}\n";
    break;
  case 5:
  default:
    result = String("\n\\subparagraph{") + TexGenerator::texEscape(segment.title) + "}\n";
    break;
  }
  if(segment.arguments.contains("-") || segment.arguments.contains(".unnumbered"))
  {
    const char* x = result.find('{');
    if(x)
      result = result.substr(0, x - (const char*)result) + "*" + result.substr(x - (const char*)result);
  }
  String label = segment.arguments.find("#")->toString();
  if(!label.isEmpty())
  {
    result.append("\\label{");
    result.append(label);
    result.append("}");
  }
  return result;
}

String TexGenerator::generate(const OutputData::RuleSegment& segment)
{
  return String("\n\\HorizontalRule\n");
}

String TexGenerator::generate(const OutputData::BulletListSegment& segment)
{
  String result("\n\\begin{itemize}\n\\item ");
  for(List<OutputData::Segment*>::Iterator i = segment.childSegments.begin(), end = segment.childSegments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append(segment->generate(*this));
  }
  for(List<OutputData::BulletListSegment*>::Iterator i = segment.siblingSegments.begin(), end = segment.siblingSegments.end(); i != end; ++i)
  {
    OutputData::BulletListSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    result.append("\\item ");
    for(List<OutputData::Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
  }
  result.append("\\end{itemize}\n");
  return result;
}

String TexGenerator::generate(const OutputData::NumberedListSegment& segment)
{
  String result("\n\\begin{enumerate}\n\\item ");
  for(List<OutputData::Segment*>::Iterator i = segment.childSegments.begin(), end = segment.childSegments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate(*this));
  }
  for(List<OutputData::NumberedListSegment*>::Iterator i = segment.siblingSegments.begin(), end = segment.siblingSegments.end(); i != end; ++i)
  {
    OutputData::NumberedListSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    result.append("\\item ");
    for(List<OutputData::Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
  }
  result.append("\\end{enumerate}\n");
  return result;

}

String TexGenerator::generate(const OutputData::BlockquoteSegment& segment)
{
  String result("\n\\begin{quoting}\n");
  for(List<OutputData::Segment*>::Iterator i = segment.childSegments.begin(), end = segment.childSegments.end(); i != end; ++i)
  {
    const OutputData::Segment* segment = *i;
    if(!segment->isValid())
      continue;
    result.append((*i)->generate(*this));
  }
  for(List<OutputData::BlockquoteSegment*>::Iterator i = segment.siblingSegments.begin(), end = segment.siblingSegments.end(); i != end; ++i)
  {
    OutputData::BlockquoteSegment* siblingSegment = *i;
    if(!siblingSegment->isValid())
      continue;
    for(List<OutputData::Segment*>::Iterator i = siblingSegment->childSegments.begin(), end = siblingSegment->childSegments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
  }
  result.append("\n\\end{quoting}\n");
  return result;
}

String TexGenerator::generate(const OutputData::EnvironmentSegment& segment)
{
  String environment = segment.language;
  if(environment.isEmpty())
    environment = "plain";
  environment = TexGenerator::getEnvironmentName(environment);

  String caption = segment.arguments.find("caption")->toString();
  String flags;
  if(!caption.isEmpty())
    flags += String("title=\\EnvironmentCaption{") + TexGenerator::texEscape(caption) + "}";

  String result;
  result.append(String("\n\\begin{") + environment + "}");
  result.append(flags.isEmpty() ? String("\n") : String("[") +  flags + "]\n");
  if(segment.verbatim)
  {
    for(List<String>::Iterator i = segment.lines.begin(), end = segment.lines.end(); i != end; ++i)
    {
      result.append(*i);
      result.append("\n");
    }
  }
  else
  {
    for(List<OutputData::Segment*>::Iterator i = segment.segments.begin(), end = segment.segments.end(); i != end; ++i)
    {
      const OutputData::Segment* segment = *i;
      if(!segment->isValid())
        continue;
      result.append(segment->generate(*this));
    }
  }
  result.append(String("\\end{") + environment + "}\n");
  return result;
}

String TexGenerator::generate(const OutputData::TableSegment& segment)
{
  String result;
  String caption;
  bool xtabGridStyle = segment.arguments.contains(".xtabgrid");
  bool gridStyle = segment.arguments.contains(".grid") || xtabGridStyle;
  bool xtabStyle = segment.arguments.contains(".xtab") || xtabGridStyle;
  if(segment.captionSegment)
  {
    caption = segment.captionSegment->getText();
    if(caption.startsWith(":"))
      caption = caption.substr(1);
    else // Table:
      caption = caption.substr(6);
    result.append("\n\\begin{table}[H]\\centering");
  }
  else
    result.append("\n\\begin{center}");
  if(xtabStyle)
  {
    result.append(String("\\tabletail{\\hline\\multicolumn{") + String::fromUInt(segment.columns.size()) + "}{c}{\\vspace{-1cm}\\small\\emph{\\ldots}}\\\\}%\n");
    result.append(String("\\tablefirsthead{}%\n"));
    if (!gridStyle)
        result.append(String("\\tablehead{\\multicolumn{") + String::fromUInt(segment.columns.size()) + "}{c}{\\small\\emph{\\ldots}}\\\\\\hline}%\n");
    else
        result.append(String("\\tablehead{\\multicolumn{") + String::fromUInt(segment.columns.size()) + "}{c}{\\small\\emph{\\ldots}}\\\\}%\n");
    result.append("\\begin{xtabular}{");
  }
  else
    result.append("\\begin{tabular}{");
  if(gridStyle)
    result.append("|");
  for(Array<OutputData::TableSegment::ColumnInfo>::Iterator i = segment.columns.begin(), end = segment.columns.end(); i != end; ++i)
  {
    const OutputData::TableSegment::ColumnInfo& columnInfo = *i;
    String width = columnInfo.arguments.find("width")->toString();
    if(!width.isEmpty())
      result.append(String("p{") + TexGenerator::getTexSize(width) + "}");
    else
    {
      char a = columnInfo.alignment == OutputData::TableSegment::ColumnInfo::rightAlignment ? 'r' : (columnInfo.alignment == OutputData::TableSegment::ColumnInfo::centerAlignment ? 'c' : 'l');
      result.append(a);
    }
    if(gridStyle)
      result.append("|");
  }
  result.append("}\n");
  result.append("\\hline\n");
  for(List<OutputData::TableSegment::RowData>::Iterator i = segment.rows.begin(), end = segment.rows.end(); i != end; ++i)
  {
    OutputData::TableSegment::RowData& rowData = *i;
    usize columnIndex = 0;
    {
      for(Array<OutputData::TableSegment::CellData>::Iterator begin = rowData.cellData.begin(), i = begin, end = rowData.cellData.end(); i != end; ++i, ++columnIndex)
      {
        //const ColumnInfo& columnInfo = columns[columnIndex];
        OutputData::TableSegment::CellData& cellData = *i;
        if(i != begin)
          result.append(" & ");
        //String width = columnInfo.arguments.find("width")->toString();
        //if(!width.isEmpty())
        //  result.append(String("\\parbox[t][][t]{") + TexGenerator::getTexSize(width) + "}{");
        for(List<OutputData::TableSegment::Segment*>::Iterator i = cellData.segments.begin(), end = cellData.segments.end(); i != end; ++i)
        {
          OutputData::TableSegment::Segment* segment = *i;
          if(!segment->isValid())
            continue;
          result.append(segment->generate(*this));
        }
        //if(!width.isEmpty())
        //  result.append("\\vspace{5pt}}");
      }
    }
    result.append(" \\\\\n");
    if(i == segment.rows.begin() || gridStyle)
      result.append("\\hline\n");
  }
  if(segment.rows.size() > 1 && !gridStyle)
    result.append("\\hline\n");
  if(xtabStyle)
    result.append("\\end{xtabular}");
  else
    result.append("\\end{tabular}");
  if(segment.captionSegment)
  {
    String label = segment.arguments.find("#")->toString();
    if(!label.isEmpty())
      label = String("\\label{") + label + "}";
    result.append(String("\\caption{") + TexGenerator::texEscape(caption) + "}" + label + "\\end{table}");
  }
  else
    result.append("\\end{center}");
  return result;
}

String TexGenerator::generate(const OutputData::TexSegment& segment)
{
  return String("\n") + segment.content + "\n";
}

String TexGenerator::generate(const OutputData::TexPartSegment& segment)
{
  return String("\n\\clearpage\n\\part{") + TexGenerator::texEscape(segment.title) + "}\n";
}

String TexGenerator::generate(const OutputData::PdfSegment& segment)
{
  return String("\n\\includepdf[pages=-]{") + segment.filePath + "}\n";
}
