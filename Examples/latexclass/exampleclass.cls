\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{exampleclass}

\LoadClass[a4paper]{article}

\usepackage[english]{babel}

\usepackage{geometry}
\geometry{
 a4paper,
 left=25mm,
 right=25mm,
 top=38mm,
 bottom=25mm,
}

\usepackage{hyperref}
\hypersetup{%
 colorlinks,
 citecolor=black,
 filecolor=black,
 linkcolor=black,
 urlcolor=blue
}

\let\oldhyperref\hyperref
\renewcommand{\hyperref}[2][]{\oldhyperref[#1]{\color{blue}#2}}

\usepackage{graphicx}

\usepackage{enumitem}
\setlist{topsep=0pt}

\usepackage{quoting}

\usepackage[default,osf]{sourcesanspro}
\usepackage[scaled=.8]{sourcecodepro}

\setlength\parindent{0pt}
\setlength\parskip{5pt}

\usepackage{titlesec}

\setcounter{secnumdepth}{4}
\titleformat{\paragraph}{\normalfont\normalsize\bfseries}{\theparagraph}{1em}{}
\titleformat{\subparagraph}{\normalfont\normalsize\bfseries}{\thesubparagraph}{1em}{}
\usepackage{float}

\usepackage{listings}

\usepackage{xcolor}
\usepackage{environ}

\newcommand\HorizontalRule{\raisebox{3.5pt}[1.5ex]{\rule{\linewidth}{0.4pt}}}

\newcommand\InlineImage[1]{\raisebox{-0.1em}{\includegraphics[height=0.9em]{#1}}}

\usepackage{array}
\renewcommand{\extrarowheight}{2pt}

\definecolor{boxBackgroundColor}{RGB}{245,245,245}
\definecolor{boxFrameColor}{RGB}{128,128,128}
\definecolor{codeRedColor}{RGB}{163,21,21}
\definecolor{codeBlueColor}{RGB}{0,0,255}
\definecolor{codeGreenColor}{RGB}{0,128,0}
\lstdefinelanguage{XML}{  basicstyle=\ttfamily\color{codeBlueColor},  morestring=[b]",  moredelim=[s][\color{codeBlueColor}]{<}{\ },  moredelim=[s][\color{codeBlueColor}]{</}{>},  morecomment=[s]{<?}{?>},  morecomment=[s]{<!--}{-->},  commentstyle=\color{codeGreenColor},  stringstyle=\color{codeRedColor},  identifierstyle=\color{red}}

\newcommand\EnvironmentCaption[1]{\parbox{\textwidth}{\textbf{#1}}}

\lstset{frame=single,basicstyle=\ttfamily,breaklines=true,showstringspaces=false,backgroundcolor=\color{boxBackgroundColor},rulecolor=\color{boxFrameColor},keywordstyle=\color{codeBlueColor},stringstyle=\color{codeRedColor},commentstyle=\color{codeGreenColor}}
\lstnewenvironment{plain}{\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{abap}[1][]{\lstset{language=ABAP,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{acm}[1][]{\lstset{language=ACM,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{acmscript}[1][]{\lstset{language=ACMscript,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{acsl}[1][]{\lstset{language=ACSL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{ada}[1][]{\lstset{language=Ada,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{algol}[1][]{\lstset{language=Algol,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{ant}[1][]{\lstset{language=Ant,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{assembler}[1][]{\lstset{language=Assembler,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{awk}[1][]{\lstset{language=Awk,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{bash}[1][]{\lstset{language=bash,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{basic}[1][]{\lstset{language=Basic,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{clanguage}[1][]{\lstset{language=C,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{cplusplus}[1][]{\lstset{language=C++,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{caml}[1][]{\lstset{language=Caml,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{cil}[1][]{\lstset{language=CIL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{clean}[1][]{\lstset{language=Clean,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{cobol}[1][]{\lstset{language=Cobol,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{comal80}[1][]{\lstset{language=Comal 80,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{commandcom}[1][]{\lstset{language=command.com,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{comsol}[1][]{\lstset{language=Comsol,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{csh}[1][]{\lstset{language=csh,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{delphi}[1][]{\lstset{language=Delphi,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{eiffel}[1][]{\lstset{language=Eiffel,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{elan}[1][]{\lstset{language=Elan,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{erlang}[1][]{\lstset{language=erlang,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{euphoria}[1][]{\lstset{language=Euphoria,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{fortran}[1][]{\lstset{language=Fortran,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{gcl}[1][]{\lstset{language=GCL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{gnuplot}[1][]{\lstset{language=Gnuplot,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{hansl}[1][]{\lstset{language=hansl,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{haskell}[1][]{\lstset{language=Haskell,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{html}[1][]{\lstset{language=HTML,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{idl}[1][]{\lstset{language=IDL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{inform}[1][]{\lstset{language=inform,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{java}[1][]{\lstset{language=Java,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{jvmis}[1][]{\lstset{language=JVMIS,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{ksh}[1][]{\lstset{language=ksh,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{lingo}[1][]{\lstset{language=Lingo,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{lisp}[1][]{\lstset{language=Lisp,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{llvm}[1][]{\lstset{language=LLVM,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{logo}[1][]{\lstset{language=Logo,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{lua}[1][]{\lstset{language=Lua,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{make}[1][]{\lstset{language=make,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{matlab}[1][]{\lstset{language=Matlab,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{mathematica}[1][]{\lstset{language=Mathematica,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{mercury}[1][]{\lstset{language=Mercury,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{metapost}[1][]{\lstset{language=MetaPost,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{miranda}[1][]{\lstset{language=Miranda,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{mizar}[1][]{\lstset{language=Mizar,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{ml}[1][]{\lstset{language=ML,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{modula2}[1][]{\lstset{language=Modula-2,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{mupad}[1][]{\lstset{language=MuPAD,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{nastran}[1][]{\lstset{language=NASTRAN,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{oberon2}[1][]{\lstset{language=Oberon-2,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{ocl}[1][]{\lstset{language=OCL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{octave}[1][]{\lstset{language=Octave,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{oz}[1][]{\lstset{language=Oz,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{perl}[1][]{\lstset{language=Perl,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{pascal}[1][]{\lstset{language=Pascal,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{php}[1][]{\lstset{language=PHP,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{pli}[1][]{\lstset{language=PL/I,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{plasm}[1][]{\lstset{language=Plasm,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{postscript}[1][]{\lstset{language=PostScript,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{pov}[1][]{\lstset{language=POV,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{prolog}[1][]{\lstset{language=Prolog,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{promela}[1][]{\lstset{language=Promela,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{pstricks}[1][]{\lstset{language=PSTricks,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{python}[1][]{\lstset{language=Python,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{rlanguage}[1][]{\lstset{language=R,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{reduce}[1][]{\lstset{language=Reduce,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{rexx}[1][]{\lstset{language=Rexx,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{rsl}[1][]{\lstset{language=RSL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{ruby}[1][]{\lstset{language=Ruby,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{slanguage}[1][]{\lstset{language=S,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{sas}[1][]{\lstset{language=SAS,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{scala}[1][]{\lstset{language=Scala,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{scilab}[1][]{\lstset{language=Scilab,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{sh}[1][]{\lstset{language=sh,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{shelxl}[1][]{\lstset{language=SHELXL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{sparql}[1][]{\lstset{language=SPARQL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{simula}[1][]{\lstset{language=Simula,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{sql}[1][]{\lstset{language=SQL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{tcl}[1][]{\lstset{language=tcl,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{tex}[1][]{\lstset{language=TeX,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{vbscript}[1][]{\lstset{language=VBScript,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{verilog}[1][]{\lstset{language=Verilog,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{vhdl}[1][]{\lstset{language=VHDL,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{vrml}[1][]{\lstset{language=VRML,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{xml}[1][]{\lstset{language=XML,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}
\lstnewenvironment{xslt}[1][]{\lstset{language=XSLT,#1}\vspace{\parskip}\minipage{\linewidth}}{\endminipage}

\NewEnviron{boxed}[1][]{\vspace{\parskip}\hspace{-3.4pt}\fcolorbox{boxFrameColor}{white}{\minipage{\linewidth}
\vspace{3.3pt}\BODY
\vspace{3.4pt}\endminipage}\vspace{3.3pt}}

\let\oldtexttt\texttt
\renewcommand{\texttt}[1]{\fcolorbox{boxFrameColor}{boxBackgroundColor}{\raisebox{0pt}[0.45em][0pt]{\oldtexttt{#1}}}}
