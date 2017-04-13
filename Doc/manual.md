
# Introduction

*umdoc* is a Markdown to *LaTeX* to *PDF* converter.
It is small command line tool, which converts a Markdown file or set of Markdown files with optional layout information written in *LaTeX* into an input file (`.tex` file) for a *LaTeX* engine like `lualatex` or `pdflatex`. This file is then converted into a *PDF* document using such an engine.

The tool can be used in various ways:

1. It can directly convert a Markdown file (ending with `.md`) into *LaTeX* or *PDF*:
   ```
umdoc [-t] example.md
```
2. It can read and convert an *umdoc* *XML* file (see section~[](#umdoc-xml-file)), which defines Markdown input files and optional layout information, into a *LaTeX* or *PDF* document:
   ```
umdoc [-t] example.xml
```

3. It can simply be launched in a working directory where it expects to find an *umdoc* *XML* file with the name `umdoc.xml`, which is converted *LaTeX* or *PDF*:
   ```
umdoc [-t]
```

The *umdoc* tool creates a *PDF* document by default. The `-t` option tells *umdoc* to stop after creating the *LaTeX* file. (See section~[](#usage) for a full list of accepted command line arguments.)

## Motivation

There are plenty of options out there if you want to write documentation that can be delivered on paper or in form of a *PDF*.
A WYSIWYG word processor can be used, you can use a sophisticated markup language like *LaTeX* or a anything in between.
But, writing documentation should be simple and convenient.
If you collaborate with others, you want to use something that can easily be version controlled and merged with version control systems like *Git*.
Hence, using tools that rely on a more or less unreadable format are not an option.
Markup languages have other disadvantages.
You have to know them to use them.
Markdown on the other hand can easily be learned simply by looking at a Markdown document. 
So, using Markdown and converting it into a neatly formatted document seems like the perfect solution if your documentation needs can be met with basic text formatting constructs.
But, if it actually comes to converting Markdown into a document, you will realize that it is not as easy as it should be.

This is because Markdown was designed to be converted into inner text *HTML* and not into pages.
To create a document, you will probably require:
* The possibility to define the basic layout of document (cover page, table of contents, etc.).
* The possibility to define a page header and footer.
* Support for tables
* Figure and table environments
* Cross references
* Syntax highlighting

All this cannot not be accomplished with traditional Markdown without heavily relying on *HTML*.
Hence, you require some extensions to the Markdown language and a way to define the layout of the document to be created.

*umdoc* tries to overcome the shortcomings of traditional Markdown with some Markdown extensions inspired by *[GitHub](http://github.com) flavored Markdown* and [Pandoc](http://pandoc.org).
Additionally, layout information can be specified using an *umdoc* specific *XML* file (see section~[](#umdoc-xml-file)).

##### Project Goals

The *umdoc* project tries to achieve the following goals:

* Be easy to use.
* Create a decent out of the box result.
* Be easy to develop.
* Be customizable.
* Support user defined *LaTeX* environments.

##### Project Non-Goals

In contrary, goals that the project does not try to achieve are:

* Support of all Markdown features and dialects.
* Create output in other formats than *LaTeX* or *PDF*.

# Installation

The *umdoc* tool consists of a single binary without any notable dependencies except a *LaTeX* engine.
To install it, simply [download](https://github.com/craflin/umdoc/releases) the binary for your platform, place it somewhere on your system and ensure that it can be found with the *PATH* environment variable.
You can also call the *umdoc* tool directly if you do not want to touch your *PATH* variable.

Additionally, you have to install a *TeX* distribution like [*MiKTeX*](http://miktex.org) on Windows or [*TeX Live*](http://www.tug.org/texlive/) on Linux or Windows.
Most Linux distributions provide packages for *TeX Live* that you can install with the packaging system of that distribution (*APT*, *RPM*, etc.).

# Compilation

If you do not trust binaries or if you want to participate in the development of *umdoc*, you can compile *umdoc* for yourself.

## Windows

* Clone the Git repository.
* Initialize submodules.
* Call `generate.bat` to generate a solution file (`umdoc.sln`) for Visual Studio 2013.
* Open `umdoc.sln` in Visual Studio.
* Compile the *umdoc* project in Visual Studio.

## Linux

* Ensure you have `git` and `g++` installed.
* Clone the Git repository. `git clone git@github.com:craflin/umdoc.git`
* Initialize submodules. `git submodule update --init`
* Call `./generate` to compile *umdoc*.

# Usage {#usage}

The *umdoc* executable can be launched with the following arguments:

```
umdoc [<input-file>] [-a <aux-directory>] [-e <latex-engine>] [-o <output-file>] [-t]
      [--<variable>=<value>]
```

The default behavior of *umdoc* is to look in the working directory for a `umdoc.xml` file, which is converted into a *LaTeX* and into a *PDF* in the same directory.
This behavior can be changed by specifying the input file with `<input-file>` and/or the output file with `-o <output-file>`.

##### Options

* `<input-file>`

  The input file, which can be an *umdoc* *XML* file (see section~[](#umdoc-xml-file)) or a Markdown (`.md`) file. The default is `umdoc.xml`.
  
* `-o <output-file>`, `--output=<output-file>`

  The name or the path of the output *PDF* file. The default is derived from the input file where the file name extension is replaced by `.pdf`.
  If `-t` is set, this option specifies the name or the path of output *LaTeX* (`.tex`) file and the extension of the input file is replaced `.tex` in the default case.
  
* `-e <latex-engine>`, `--engine=<latex-engine>`

  The launch command of the *LaTeX* engine to be used to convert the created *LaTeX* document into *PDF*.
  The default is `lualatex`.

* `-a <aux-directory>`, `--aux-directory=<aux-directory>`

  A directory for intermediate files like the created *LaTeX* file or files created from the *LaTeX* engine.
  The default is the directory of the output file plus the base name of the output file.

* `-t`, `--tex`

  Stop after creating the *LaTeX* (`.tex`) file. If `-t` is set, `-o` specifies the name or the path of the *LaTeX* output file.

* `--version`

  Print version and exit.

* `--<placeholder>=<value>`

  Overwrite the *umdoc* *XML* file placeholder `<placeholder>` with value `<value>` (see section~[](#umdoc-xml-file-placeholder)).

## The *umdoc* *XML* File {#umdoc-xml-file}

The purpose of the *umdoc* *XML* file is to provide *umdoc* with everything it needs to create the output file.
This includes: Markdown input files, *LaTeX* "header files", and basic layout information like where to insert the table of contents, page breaks or pages from other sources like `.pdf` or `.tex` files.

The *XML* file defines an `<umdoc>` element in which the following elements can be used:

* `<environment>`

  Defines a custom *LaTeX* environment (see section~[](#umdoc-xml-file-custom-environments)).

* `<set>`

  Defines a placeholder string (see section~[](#umdoc-xml-file-placeholder)), which can be used to insert a string into *LaTeX* header files (see section~[](#umdoc-xml-tex-header)) and into in document *LaTeX* files (see section~[](#umdoc-xml-tex-in-document)).

* `<tex>`

  Inserts a *LaTeX* header file into the generated *LaTeX* document (see section~[](#umdoc-xml-tex-header)).

* `<document>`

  Defines the structure of the document to be generated. (see section~[](#umdoc-xml-document)).

##### Example

An *umdoc* *XML* file might look like this:

```xml "umdoc.xml"
<umdoc>
    <set name="version" value="1.0"/>
    <tex file="style.tex"/>
    <document>
        <pdf file="coverpage.pdf"/>
        <toc/>
        <break/>
        <md file="01_introduction.md"/>
        <md file="02_platform.md"/>
        <md file="03_implementation.md"/>
        <md file="04_results.md"/>
    </document>
</umdoc>
```
  
### *LaTeX* Class Name {#latex-class}

By default *umdoc* uses the *LaTeX* class `article` with various style extensions.
This can be overwritten within the `<umdoc>` element:

```xml
<umdoc class="<class_name>">
```

Overwriting the class name disables all default style extensions.
Hence, the *LaTeX* class `<class_name>` has to provide an implementation of all the *LaTeX* commands that are required for the Markdown features used in a Markdown file (see section~[](#supported-markdown-features)).
  
### Custom Environments {#umdoc-xml-file-custom-environments}

The spectrum of supported environments or "languages" in fenced code blocks (see section~[](#fenced-code-blocks)) can be extended with custom *LaTeX* environments.
Therefore, your *LaTeX* class (see section~[](#latex-class)) or one of the *LaTeX* header files (see section~[](#umdoc-xml-tex-header)) has to declare the environment.
To use them in a Markdown file, *umdoc* has to be made aware of such an environment using the `<environment>` element:

```xml
<environment name="<name>" verbatim="<bool>"/>
```

``verbatim`` can be set to `true` or `false`. 
If it is set to `true`, the code in the fenced code block will be passed unchanged to the *LaTeX* environment. 
If `verbatim` is set to `false`, the code in the fenced code block will be considered to be Markdown and converted to *LaTeX* as such.

### Placeholders {#umdoc-xml-file-placeholder}

The *umdoc* *XML* file can define placeholders, which are replaced in inserted *LaTeX* files (see section~[](#umdoc-xml-tex-header) and section~[](#umdoc-xml-tex-in-document)):

```xml
<set name="<name>" value="<value>"/>
```

For each defined placeholder the occurrence of `%name%` is replaced by `<value>` in the inserted *LaTeX* files.
The value of a placeholder can be overwritten using a `--<placeholder>=<value>` command line argument when calling the *umdoc* executable.

### *LaTeX* Header Files {#umdoc-xml-tex-header}

A `<tex>` element can be used to insert the contents of a `.tex` file into the generated *LaTeX* file before the `\begin{document}` section of the generated file:

```xml
<tex file="<file>"/>
```

The attribute `file` specifies the path to the *LaTeX* file.
The path is relative to the location of the *umdoc* *XML* file.
The file may contain placeholders (see section~[](#umdoc-xml-file-placeholder)).

### Document Structure {#umdoc-xml-document}

The `<document>` element defines the structure of the document to be generated.
The following elements can be used within this element:

* `<break>`

  Inserts a manual page break (see section~[](#umdoc-xml-page-break)).

* `<environment>`

  Defines a custom *LaTeX* environment (see section~[](#umdoc-xml-file-custom-environments)).

* `<md>`

  Inserts a converted Markdown file (see section~[](#umdoc-xml-markdown)).

* `<part>`

  Inserts a segment title into the generated document (see section~[](#umdoc-xml-part)).

* `<pdf>`

  Inserts a *PDF* document (see section~[](#umdoc-xml-pdf)).

* `<set>`

  Defines a placeholder string (see section~[](#umdoc-xml-file-placeholder)), which can be used to insert a string into *LaTeX* header files (see section~[](#umdoc-xml-tex-header)) and into in document *LaTeX* files (see section~[](#umdoc-xml-tex-in-document)).

* `<tex>`

  Inserts a *LaTeX* (`.tex`) file into the generated *LaTeX* document (see section~[](#umdoc-xml-tex-in-document)).

* `<toc>`

  Inserts a table of contents (see section~[](#umdoc-xml-toc)).

* `<lof>`

  Inserts a list of figures (see section~[](#umdoc-xml-lof)).

* `<lot>`

  Inserts a list of tables (see section~[](#umdoc-xml-lof)).

#### Manual Page Breaks {#umdoc-xml-page-break}

A manual page break can be inserted using the `<break/>` element.
This is most useful after inserting the table of contents (see section~[](#umdoc-xml-toc)) or after inserting a Markdown or *LaTeX* file.

The `<break>` element translates to the *LaTeX* command `\\clearpage`.

#### Markdown Files {#umdoc-xml-markdown}

A Markdown file can be inserted with:

```xml
<md file="<file>"/>
```

The attribute `file` specifies the path to the Markdown (`.md`) file.
The path is relative to the location of the *umdoc* *XML* file.
The content of the file is interpreted and converted to *LaTeX* respectively to the supported Markdown features (see section~[](#supported-markdown-features)).

#### Segment Titles {#umdoc-xml-part}

A segment title, which is hierarchically above the highest section title of a Markdown file (see section~[](#markdown-titles)), can be inserted with:

```xml
<part title="<title>"/>
```

The `title` attribute supports basic Markdown styling (see section~[](#markdown-styling)).

The `<part>` element translates to the *LaTeX* command `\\part`.

#### *PDF* Files {#umdoc-xml-pdf}

A *PDF* document can be inserted into the generated document with:

```xml
<pdf file="<file>"/>
```

The attribute `file` specifies the path to the *PDF* file.
The path is relative to the location of the *umdoc* *XML* file.

The `<part>` element translates to the *LaTeX* command `\\includepdf` from the *pdfpages* package.

#### *LaTeX* Files {#umdoc-xml-tex-in-document}

A `<tex>` element inserts the contents of a *LaTeX* (`.tex`) file directly into the generated *LaTeX* file:

```xml
<tex file="<file>"/>
```

The attribute `file` specifies the path to the *LaTeX* file.
The path is relative to the location of the *umdoc* *XML* file.
The file may contain placeholders (see section~[](#umdoc-xml-file-placeholder)).

#### Table of Contents {#umdoc-xml-toc}

An automatically generated table of contents can be inserted using the element `<toc/>`.

This translates to the *LaTeX* command `\\tableofcontents`.

#### List of Figures {#umdoc-xml-lof}

An automatically generated list of figures can be inserted using the element `<lof/>`.

This translates to the *LaTeX* command `\\listoffigures`.

#### List of Tables {#umdoc-xml-lot}

An automatically generated list of tables can be inserted using the element `<lot/>`.

This translates to the *LaTeX* command `\\listoftables`.

# Supported Markdown Features {#supported-markdown-features}

## Titles {#markdown-titles}

*umdoc* supports Atx-style and Setext-style titles.

```
# Example Title

## Example Title Level 2

### Example Title Level 3

Example text.
```

or

```
Example Title
=============

Example Title Level 2
---------------------

### Example Title Level 3

Example text.
```

results in:
```boxed
1 ~ ~ ~Example Title {-}
=============

1.1 ~ ~ ~Example Title Level 2 {-}
---------------------

### 1.1.1 ~ ~ ~Example Title Level 3 {-}

Example text.
```

Depending on the title level, this translate to the *LaTeX* commands `\\section`, `\\subsection`, `\\subsubsection`, `\\paragraph`, and `\\subparagraph`.
Titles of a level beyond level 5 will also be translated to `\\subparagraph`.

## Basic Markdown Styling {#markdown-styling}

### Italic and Bold Text

Placing the character `\*` or `\_` around some text will turn it to italic.
Two consecutive `\*` or `\_` characters (`\*\*` or `\_\_`) will turn the text to bold.
But, `\*` and `\_` will be left unchanged when they are surrounded by spaces.
Additionally, the `\_` character will be left unchanged if it occurs in a word.

```
The following is *italic* and so is _this_. __This__ is bold and so is **this**. You can use * or _ characters surrounded by spaces and the _ character is left unchanged when it occurs in_word. You can use * or ** for in**word**highlighting.
```

results in:
```boxed
The following is *italic* and so is _this_. __This__ is bold and so is **this**. You can use * or _ characters surrounded by spaces and the _ character is left unchanged when it occurs in_word. You can use * or ** for in**word**highlighting.
```

Italic text translates to the *LaTeX* command `\\emph` and bold text to `\\textbf`.

### Code Spans

Text can be formatted with a fixed width font by surrounding it with single or double back ticks `` ` ``.

```
Some text with code like `printf()`. Code span with back ticks `` `test` ``.
```

results in:
```boxed
Some text with code like `printf()`. Code span with back ticks `` `test` ``.
```

This translates to the *LaTeX* command `\\texttt`. The default *umdoc* style redefines `\\texttt` as:
```
\let\oldtexttt\texttt
\renewcommand{\texttt}[1]{\fcolorbox{boxFrameColor}{boxBackgroundColor}{\raisebox{0pt}[0.45em][0pt]{\oldtexttt{#1}}}}
```

### Inline Links {#markdown-inline-links}

A link can be inserted using the syntax `(<text>)[<url>]`.

```
This is an [example link](https://github.com/craflin/umdoc). You can also create a link to a [email address](mailto:some@email.com) links. Links without a text look like this: [](https://github.com/craflin/umdoc)
```

results in:
```boxed
This is an [example link](https://github.com/craflin/umdoc). You can also create a link to a [email address](mailto:some@email.com) links. Links without a text look like this: [](https://github.com/craflin/umdoc)
```

<!-- The `<text>` is optional and `<url>` will be used if it left blank. -->

A link translates to the *LaTeX* command `\\hyperref` from the *hyperref* package.

### Cross References {#markdown-cross-references}

Cross references to section titles can be used by labeling the title and linking to it using the syntax of inline links (see section~[](#markdown-inline-links)).
Section titles can be labeled by adding `{#<label>}` to the end of the title.
A link to such a title can be created by using `#<label>` as the `<url>` in the inline link.
The section number will be used as `<text>` of the link if it is left blank.

```
This references to section~[](#markdown-cross-references) and here is a reference to [Cross References](#markdown-cross-references).
```

results in:
```boxed
This references to section~[](#markdown-cross-references) and here is a reference to [Cross References](#markdown-cross-references).
```

A cross reference with a `<text>` translates to the *LaTeX* command `\\hyperref` and a reference without a `<text>` to the command `\\ref`.

### Inline Images

A small image or an icon can be inserted using the syntax `\!\[<title>](<file>)`.

```
This is an example text with an inline image like this ![](circle.png).
```

results in:
```boxed
This is an example text with an inline image like this ![](circle.png).
```

This translates to the custom *LaTeX* command `\\InlineImage`. By the default *umdoc* style it is defined with:
```
\newcommand\InlineImage[1]{\raisebox{-0.1em}{\includegraphics[height=0.9em]{#1}}}
```

 `<file>` is a path relative to the location of the *umdoc* *XML* file.

### Footnotes

A footnote can be inserted using the syntax `\[^<text>]`.

```
A footnote like this[^This is a example footnote.].
```

This translates to the *LaTeX* command `\\footnote`.

## Comments

*XML* style comments (`<\!-- some text -->`) are removed from the input file before parsing.
Hence, these comments can be used anywhere in the file (except in comments) and they can span multiple lines.

## Character Escaping

The following characters: 

`` \\\`\*\_\{\}\[\]\(\)\#\+\-\.\! ``

can be escaped with a black slash (`\\`) to prevent them from being interpreted as markdown formatting characters.

## Paragraphs

Paragraphs can be separated with an empty line.

```
This is the first sentence of a paragraph.
And this is the second.

Here starts a new paragraph.
```

results in:
```boxed
This is the first sentence of a paragraph.
And this is the second.

~

Here starts a new paragraph.
```

## Block Quotes

Block quotes can be inserted using a line staring with a `>` character.

```
> test 1
> test 2 
```

results in:
```boxed
> test 1
> test 2 
```

This translates to the *LaTeX* environment `quoting`.

## Horizontal Rules

Lines of asterisks (`\*`), hyphens (`\-`), or underscores (`\_`) are interpreted as a horizontal rule.

```
***
```

or

```
*******
```

or

```
* * * * *
```

or

```
-------
```

or

```
______
```

results in:
```boxed
___
```

This translates to the custom *LaTeX* command `\\HorizontalRule`. By the default *umdoc* style it is defined with:
```
\newcommand\HorizontalRule{\raisebox{3.5pt}[1.5ex]{\rule{\linewidth}{0.4pt}}}
```

## Bullet Lists {#markown-bullet-list}

Lines starting with asterisk (`\*`), plus (`\+`), or hyphen (`\-`) are interpreted as bullet list items.

```
* item a
* item b
* item c
```

or

```
+ item a
+ item b
+ item c
```

or

```
- item a
- item b
- item c
```

results in:
```boxed
- item a
- item b
- item c
```

Bullet lists translate to the *LaTeX* environment `itemize`.

List items can have multiple paragraphs.

```
* item a

  line 2

* item b
* item c
```

results in:
```boxed
* item a

  line 2

* item b
* item c
```

Lists can be nested.

```
* item a
  * subitem a
  * subitem a
* item b
* item c
```

results in:
```boxed
* item a
  * subitem a
  * subitem a
* item b
* item c
```

## Numbered Lists

A line starting with number followed by a dot (`.`) is interpreted as an element of a numbered list.

```
1. item a
2. item b
3. item c
```

results in:
```boxed
1. item a
2. item b
3. item c
```

This translates to the *LaTeX* environment `enumerate`.

Like bullet lists (see section~[](#markown-bullet-list)) numbered list items can have multiple paragraphs and they can be nested.

```
1. item a
   1. subitem a
   2. subitem a
2. item b
3. item c
```

results in:
```boxed
1. item a
   1. subitem a
   2. subitem a
2. item b
3. item c
```

## Fenced Code Blocks {#fenced-code-blocks}

A fenced code block inserts a custom environment into the generated *LaTeX* document. The code block starts with three or more back ticks (`` ` ``).

````
```
test
```
````

results in:
```
test
```

The environment to be used can be specified with an argument after back ticks and a caption can be inserted with a second argument.

````
```<language> "<caption>"
<code>
```
````

translates to:
```
\begin{<language>}[title=\\EnvironmentCaption{<caption>}]
<code>
\end{<language>}
```

The custom command `\\EnvironmentCaption` is by the default *umdoc* style defined with:
```
\newcommand\EnvironmentCaption[1]{\parbox{\textwidth}{\textbf{#1}}}
```

`<language>` is case insensitive.
If `<language>` is just a single character like (`c` or `r`) the word `language` will be appended.
If it contains the special character `+` it will be replaced `plus`.
If the `<language>` argument is omitted, the environment `plain` will be used for basic verbatim code.

The default *umdoc* style supports the following languages using the *LaTeX* package `listings`: `ABAP`, `ACM`, `ACMscript`, `ACSL`, `Ada`, `Algol`, `Ant`, `Assembler`, `Awk`, `bash`, `Basic`, `C`, `C++`, `Caml`, `CIL`, `Clean`, `Cobol`, `Comal 80`, `command.com`, `Comsol`, `csh`, `Delphi`, `Eiffel`, `Elan`, `erlang`, `Euphoria`, `Fortran`, `GCL`, `Gnuplot`, `hansl`, `Haskell`, `HTML`, `IDL`, `inform`, `Java`, `JVMIS`, `ksh`, `Lingo`, `Lisp`, `LLVM`, `Logo`, `Lua`, `make`, `Matlab`, `Mathematica`, `Mercury`, `MetaPost`, `Miranda`, `Mizar`, `ML`, `Modula-2`, `MuPAD`, `NASTRAN`, `Oberon-2`, `OCL`, `Octave`, `Oz`, `Perl`, `Pascal`, `PHP`, `PL/I`, `Plasm`, `PostScript`, `POV`, `Prolog`, `Promela`, `PSTricks`, `Python`, `R`, `Reduce`, `Rexx`, `RSL`, `Ruby`, `S`, `SAS`, `Scala`, `Scilab`, `sh`, `SHELXL`, `SPARQL`, `Simula`, `SQL`, `tcl`, `TeX`, `VBScript`, `Verilog`, `VHDL`, `VRML`, `XML`, `XSLT`

Additionally, it supports the non-verbatim environment `boxed`, which places a box around Markdown code.

##### Example

````
```c "main.c"
#include <stdio.h>

int main(void)
{
    printf("Hello World\n");
    return 0;
}
```
````

results in:

```c "main.c"
#include <stdio.h>

int main(void)
{
    printf("Hello World\n");
    return 0;
}
```

## Figures with Caption

An image in a single line with the form `\!\[<title>](<file>)` is considered to be a figure with a captionb.

```
![Example Figure](circle.png)
```

results in:
```boxed
![Example Figure](circle.png)
```

This translate to the *LaTeX* environment `figure` where the command `\\includesgraphics` is used to insert the image `\\caption` to set the title.

`<file>` is a path relative to the location of the *umdoc* *XML* file.

## Tables {#tables}

A table can be inserted by formatting the table with the character `|`.
The second line separates table header from the table content.

```
| 1   | 2   | 3   | 4   |
| --- | --- | --- | --- |
| 5   | 6   | 7   | 8   |
| 9   | 10  | 11  | 12  |
```

results in:
```boxed
| 1   | 2   | 3   | 4   |
| --- | --- | --- | --- |
| 5   | 6   | 7   | 8   |
| 9   | 10  | 11  | 12  |
```

The second line can specify the alignment of the table column using the character `:`.

```
| **left** | **right** | **center** |
| :---     |      ---: |   :---:    |
| a        |         b |     c      |
| a        |         b |     c      |
```

results in:
```boxed
| **left** | **right** | **center** |
| :---     |      ---: |   :---:    |
| a        |         b |     c      |
| a        |         b |     c      |
```

The width of column can be specified by adding the attribute `width` to the header cell and cells can consist of multiple paragraphs.

```
| left {width=5cm} | right {width=7cm} |
| ----  | ----------------- |
| cell 1 | cell 2 | 

           Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do 
           eiusmod tempor incididunt ut labore et dolore magna aliqua.

  Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do 
  eiusmod tempor incididunt ut labore et dolore magna aliqua.
```

results in:
```boxed
| left {width=5cm} | right {width=7cm} |
| ----  | ----------------- |
| cell 1 | cell 2 | 

           Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do 
           eiusmod tempor incididunt ut labore et dolore magna aliqua.

  Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do 
  eiusmod tempor incididunt ut labore et dolore magna aliqua.
```

Tables translate to the *LaTeX* environment `tabular` where `\\hline` is inserted at the beginning, after the first line and after the last line. The alignment of the cells is set with `l`, `r`, `c`, or `p{<width>}`.

## Tables with Caption

A table is considered to have a caption if the paragraph following the table starts with `:` or `Table:`.

```
|a|b|
|c|d|

Table: Example Table
```

results in:
```boxed
|a|b|
|c|d|

Table: Example Table
```

Tables with caption translate to a *LaTeX* table as in section~[](#tables) surrounded by the environment `table`.
