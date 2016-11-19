
# Introduction

*umdoc* is a Markdown to *LaTeX* to *PDF* converter.
It is small command line tool, which converts a Markdown file or set of Markdown files with optional layout information written in *LaTeX* into an input file (`.tex` file) for a *LaTeX* engine like `lualatex` or `pdflatex`. This file is than converted into a *PDF* document using such an engine.

The tool can be used in various ways:

1. It can directly convert a Markdown file (ending with `.md`) into *LaTeX* or *PDF*:
   ```
umdoc [-t] example.md
```
2. It can read and convert an *umdoc* *XML* file (see section~[](#umdoc-xml-file)), which defines Markdown input files and optional layout information, into a *LaTeX* or *PDF* document:
   ```
umdoc [-t] example.xml
```

3. It can simply be launched in a working directory where it expects to find an *umdoc* *XML* file with the name `umdoc.xml`:
   ```
umdoc [-t]
```

The *umdoc* tool creates a *PDF* document by default. The `-t` option tells *umdoc* to stop after creating the `.tex` file. (See section~[](#usage) for a full list of accepted command line arguments.)

## Motivation

There are plenty of options out there if you want to write documentation that can be delivered on paper or in form of a *PDF*. A WYSIWYG word processor can be used, you can use a sophisticated markup language like *LaTeX* or a anything in between.
But writing documentation should be simple and convenient. If you collaborate with others, you want to use something that can easily be version controlled and merged with version control systems like *Git*. Hence, using tools that rely on a more or less unreadable format are not an option.
Markup languages have other disadvantages. You have to know them to use them. Markdown on the other hand can easily be learned simply by looking at a Markdown document. 
So, using something like Markdown and converting it into a neatly formatted document seems like the perfect solution if your documentation needs can be met with basic text formatting constructs.
But, if it actually comes to converting Markdown into a document, you will realize that it is not as easy as it should be.

This is because Markdown was designed to be converted into inner text *HTML* and not into pages. To create a document, you will probably require:
* The possibility to define the basic layout of document (cover page, table of contents, etc.).
* The possibility to define a page header and footer.
* Support for tables
* Figure and table environments
* Cross references
* Syntax highlighting

All this cannot not be accomplished with traditional Markdown without heavily relying on *HTML*. Hence, you require some extensions to the Markdown language and a way to define the layout of the document to be created.

*umdoc* tries to overcome the shortcomings of traditional Markdown with some Markdown extensions inspired by *[GitHub](http://github.com) flavored Markdown* and [Pandoc](http://pandoc.org). Additionally, layout information can be specified using an *umdoc* specific *XML* file (see section~[](#umdoc-xml-file)).

##### Project Goals

The *umdoc* project tries to achieve the following goals:

* Be easy to use.
* Create a decent out of the box result.
* Be easy to develop.
* Be customizable.
* Support user defined *LaTeX* environments.

##### Project Non-Goals

In contrary, goals that the project does not try a achieve are:

* Support of all Markdown features and dialects.
* Create output in other formats than *PDF*.

# Installation

The *umdoc* tool consists of a single binary without any notable dependencies except *LaTeX*.
To install it, simply download the binary for your platform, place it somewhere on your system and ensure that it can be found with the *PATH* environment variable. You can also call the *umdoc* tool directly if you do not want to touch your *PATH* variable.

Additionally, you have to install a *TeX* distribution like [*MiKTeX*](http://miktex.org) on Windows or [*TeX Live*](http://www.tug.org/texlive/) on Linux or Windows. Most Linux distributions provide packages for *TeX Live* that you can install with the packaging system of that distribution (*APT*, *RPM*, etc.).

# Compilation

If you do not trust binaries or if you want to participate in development, you can compile *umdoc* for yourself.

## Windows

* Clone the Git repository.
* Initialize submodules.
* Call `generate.bat` to generate a solution file (`umdoc.sln`) for Visual Studio 2013.
* Open `umdoc.sln` in Visual Studio.
* Compile the *umdoc* project in Visual Studio.

## Linux

* Ensure you have `g++` installed.
* Clone the Git repository. `git clone git@github.com:craflin/umdoc.git`
* Initialize submodules. `git submodule update --init`
* Call `./generate` to compile *umdoc*.

# Usage {#usage}

The *umdoc* executable can be launched like this:

```
umdoc [<input-file>] [-a <aux-directory>] [-e <latex-engine>] [-o <output-file>] [-t]
      [--<variable>=<value>]
```

The default behavior of *umdoc* is to look in the working directory for a `umdoc.xml` file, which is converted into a `.tex` and into a *PDF* in the same directory. The default behavior can be changed by specifying the input file with `<input-file>` and/or the output file with `-o <output-file>`.

##### Options

* `<input-file>`

  The input file, which can be an *umdoc* *XML* file (see section~[](#umdoc-xml-file)) or a Markdown (`.md`) file. The default is `umdoc.xml`.
  
* `-o <output-file>`, `--output=<output-file>`

  The name or the path of the output *PDF* file. The default is derived from the input file where the file name extension is replaced by `.pdf`. If `-t` is set, this option specifies the name or the path of output `.tex` file and the extension of the input file is replaced `.tex` in the default case.
  
* `-e <latex-engine>`, `--engine=<latex-engine>`

  The launch command of the *LaTeX* engine to be used to convert the created `.tex` document into *PDF*. The default is `lualatex`.

* `-a <aux-directory>`, `--aux-directory=<aux-directory>`

  A directory for intermediate files like the created `.tex` file or files created from the *LaTeX* engine. The default is the directory of the output file plus the base name of the output file.

* `-t`, `--tex`

  Stop after creating the `.tex` file. If `-t` is set, `-o` specifies the name or the path of the `.tex` output file.

* `--<placeholder>=<value>`

  Overwrite the *umdoc* *XML* file placeholder `<placeholder>` with value `<value>`. (see section~[](#umdoc-xml-file-placeholder))

## The *umdoc* *XML* File {#umdoc-xml-file}

The purpose of the *umdoc* *XML* file is to provide *umdoc* with everything it needs to create the output file.
This includes: Markdown input files, *LaTeX* "header files", and basic layout information like where to insert the table of contents, page breaks or pages from other sources like `.pdf` or `.tex` files.

The *XML* file defines an `<umdoc>` element in which the following elements can be used:

* `<environment>`

  Defines a custom *LaTeX* environment (see section~[](#umdoc-xml-file-custom-environments)).

* `<set>`

  Defines a placeholder string (see section~[](#umdoc-xml-file-placeholder)), which can be used to insert a string into `.tex` "header files" (see section~[](#umdoc-xml-tex-header)) and into in document `.tex` files (see section~[](#umdoc-xml-tex-in-document)).

* `<tex>`

  Inserts a `.tex` "header file" in the generated `.tex` document (see section~[](#umdoc-xml-tex-header)).

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

By default *umdoc* uses the *LaTeX* class `article` with some style extensions. This can be overwritten within the `<umdoc>` element:

```xml
<umdoc class="<class_name>">
```

Overwriting the class name disables all default style extensions. Hence, the *LaTeX* class `<class_name>` has to provide an implementation for all the *LaTeX* commands that are required for the Markdown features used in a Markdown file (see section~[](#supported-markdown-features)).
  
### Custom Environments {#umdoc-xml-file-custom-environments}

The spectrum of supported environments or "languages" in fenced code blocks (see section~[](#fenced-code-blocks)) can be extended with custom *LaTeX* environments.
Therefore, your *LaTeX* class (see section~[](#latex-class)) or one of the *LaTeX* "header files" (see section~[](#umdoc-xml-tex-header)) has to declare an environment. To use them in a Markdown file, *umdoc* has to be made aware of such an environment using the `<environment>` element:

```xml
<environment name="<name>" verbatim="<bool>"/>
```

``verbatim`` can be set to `true` or `false`. 
If it is set to `true`, the code in the fanced code block will be passed unchanged to the *LaTeX* environment. 
If it is set to `false`, the code in the fenced code block will be consered to be Markdown and converted to *LaTeX* as such.

### Placeholders {#umdoc-xml-file-placeholder}

The *umdoc* *XML* file can define placeholders, which are replaced in inserted `.tex` files:

```xml
<set name="<name>" value="<value>"/>
```

For each defined placeholder the occurrence `%name%` is replaced by `<value>` in inserted `.tex` files. The value of a placeholder can be overwritten using a `--<name>=<value>` command line argument when calling the *umdoc* executable.

### `.tex` "Header Files" {#umdoc-xml-tex-header}

A `<tex>` element can be used to insert the contents of a `.tex` file into the generated `.tex` file before the `\begin{document}` section:

```xml
<tex file="<file>"/>
```

The attribute `file` specifies the path to the `.tex` file. The path is relative to the location of the *umdoc* *XML* file. The file may contain placeholders (see section~[](#umdoc-xml-file-placeholder)).

### The `<document>` element: {#umdoc-xml-document}

The `<document>` defines the structure of the document to be generated. The following elements can be used within this element:

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

  Defines a placeholder string (see section~[](#umdoc-xml-file-placeholder)), which can be used to insert a string into `.tex` "header files" (see section~[](#umdoc-xml-tex-header)) and into in document `.tex` files (see section~[](#umdoc-xml-tex-in-document)).

* `<tex>`

  Inserts a `.tex` file in the generated `.tex` document (see section~[](#umdoc-xml-tex-in-document)).

* `<toc>`

  Inserts a table of contents (see section~[](#umdoc-xml-toc)).

#### Manual Page Breaks {#umdoc-xml-page-break}

A manual page break can be inserted using the `<break/>` element. This is most useful after inserting the table of contents (see section~[](#umdoc-xml-toc)) or after inserting a Markdown or `.tex` file.

The `<break>` element translates to the *LaTeX* command `\\clearpage`.

#### Markdown Files {#umdoc-xml-markdown}

A Markdown file can be inserted with:

```xml
<md file="<file>"/>
```

The attribute `file` specifies the path to the Markdown (`.md`) file. The path is relative to the location of the *umdoc* *XML* file. The content of the file is interpreted and converted to *LaTeX* respectively to the supported Markdown features (see section~[](#supported-markdown-features))

#### Segment Titles {#umdoc-xml-part}

A segment title, which is hierarchically above the highest section title of a Markdown file (see section~[](#markdown-titles)), can be inserted with:

```xml
<part title="<title>"/>
```

The `title` attribute supports basic markdown styling (see section~[](#markdown-styling)).

The `<part>` element translates to the *LaTeX* command `\\part`.

#### *PDF* Files {#umdoc-xml-pdf}

A PDF document can be inserted into the generated document with:

```xml
<pdf file="<file>"/>
```

The attribute `file` specifies the path to the *PDF* file. The path is relative to the location of the *umdoc* *XML* file.

The `<part>` element translates to the *LaTeX* command `\\includepdf` that is provided by the *pdfpages* package.

#### `.tex` Files {#umdoc-xml-tex-in-document}

A `<tex>` element inserts the contents of a `.tex` file directly into the generated `.tex` file:

```xml
<tex file="<file>"/>
```

The attribute `file` specifies the path to the `.tex` file. The path is relative to the location of the *umdoc* *XML* file. The file may contain placeholders (see section~[](#umdoc-xml-file-placeholder)).

#### Table of Contents {#umdoc-xml-toc}

An automatically generated table of contents can be inserted using the element `<toc/>`.

This translates to the *LaTeX* command `\\tableofcontents`.

  
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
```latexexample
1 ~ ~ ~Example Title {-}
=============

1.1 ~ ~ ~Example Title Level 2 {-}
---------------------

### 1.1.1 ~ ~ ~Example Title Level 3 {-}

Example text.
```

Depending on the title level, this translate to the *LaTeX* commands `\\section`, `\\subsection`, `\\subsubsection`, `\\paragraph`, and `\\subparagraph`. Titles of a level beyond level 5 will also be translated to `\\subparagraph`.

## Basic Markdown Styling {#markdown-styling}

### Italic and Bold Text

Placing the character `\*` or `\_` around a string will turn it to italic. Two consecutive `\*` or `\_` characters (`\*\*` or `\_\_`) will turn a string to bold. But, `\*` and `\_` will be left unchanged when they are surrounded by spaces. Additionally the `\_` character will be left unchanged if it occurs in a word.

```
The following is *italic* and so is _this_. __This__ is bold and so is **this**. You can use * or _ characters surrounded by spaces and the _ character is left unchanged when it occurs in_word. You can use * or ** for in**word**highlighting.
```

results in:
```latexexample
The following is *italic* and so is _this_. __This__ is bold and so is **this**. You can use * or _ characters surrounded by spaces and the _ character is left unchanged when it occurs in_word. You can use * or ** for in**word**highlighting.
```

Italic text translates to the *LaTeX* command `\\emph` and bold text to `\\textbf`.

### Code Spans

Text can be formatted with a fixed width font by surrounding it with single or double back ticks `` ` ``:

```
Some text with code like `printf()`. Code span with backticks `` `test` ``.
```

results in:
```latexexample
Some text with code like `printf()`. Code span with backticks `` `test` ``.
```

This translates to the *LaTeX* command `\\texttt`.

### Cross References

This references to section~[](#horizontal-rules).

### Inline Links

```
This is an [example link](https://github.com/craflin/umdoc).
```

results in:
```latexexample
This is an [example link](https://github.com/craflin/umdoc).
```

### Inline Images

```
Text with inline an inline image like this ![](circle.png).
```

results in:
```latexexample
Text with inline an inline image like this ![](circle.png).
```

## Comments

*XML* style comments (`<\!-- some text -->`) are removed from the input file before parsing.
Hence, these comments can be used anywhere in the file (except in comments) and they can span multiple lines.

## Character Escaping

todo

## Horizontal Rules {#horizontal-rules}

Lines of asterisks (\*), hyphens (\-), or underscores (\_) are interpreted as a horizontal rule.

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
```latexexample
___
```

## Bullet Lists

Lines starting with asterisk (\*), plus (\+), or hyphen (\-) are interpreted as bullet list items.

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
```latexexample
- item a
- item b
- item c
```

List items can have multiple paragraphs.

```
* item a

  line 2

* item b
* item c
```

results in:
```latexexample
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
```latexexample
* item a
  * subitem a
  * subitem a
* item b
* item c
```

## Numbered Lists

```
1. item a
2. item b
3. item c
```

results in:
```latexexample
1. item a
2. item b
3. item c
```

```
1. item a
   1. subitem a
   2. subitem a
2. item b
3. item c
```

results in:
```latexexample
1. item a
   1. subitem a
   2. subitem a
2. item b
3. item c
```

## Fenced Code Blocks {#fenced-code-blocks}

```c "main.c"
#include <stdio.h>

int main(void)
{
    printf("Hello World\n");
    return 0;
}
```

```java
class HelloWorldApp {
    public static void main(String[] args) {
        System.out.println("Hello World!");
    }
}
```

```xml
<umdoc>
    <document>
        <pdf file="cover.pdf"/>
        <tableOfContents/>
        <newPage/>
        <!-- some comment -->
        <md file="example.md"/>
    </document>
</umdoc>
```

## Figure Images

```
![Example Figure](circle.png)
```

results in:
```latexexample
![Example Figure](circle.png)
```

## Tables

```
| 1   | 2   | 3   | 4   |
| --- | --- | --- | --- |
| 5   | 6   | 7   | 8   |
| 9   | 10  | 11  | 12  |
```

results in:
```latexexample
| 1   | 2   | 3   | 4   |
| --- | --- | --- | --- |
| 5   | 6   | 7   | 8   |
| 9   | 10  | 11  | 12  |
```

```
| **left** | **right** | **center** |
| :---     |      ---: |   :---:    |
| a        |         b |     c      |
| a        |         b |     c      |
```

results in:
```latexexample
| **left** | **right** | **center** |
| :---     |      ---: |   :---:    |
| a        |         b |     c      |
| a        |         b |     c      |
```

```
| left  | right {width=5cm} |
| ----  | ----------------- |
| lorem ipsum | line 1      | 

                Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do 
                eiusmod tempor incididunt ut labore et dolore magna aliqua.
```

results in:
```latexexample
| left  | right {width=12cm} |
| ----  | ----------------- |
| lorem ipsum | line 1      | 

                Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do 
                eiusmod tempor incididunt ut labore et dolore magna aliqua.
                
                * item a
                * item b
```





