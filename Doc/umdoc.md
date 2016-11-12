
# Introduction

*umdoc* is a Markdown to *LaTeX* to *PDF* converter. It is small command line tool, which converts a Markdown file or set of Markdown files with optional layout information written in *LaTeX* into an input file for a *LaTeX* engine like `lualatex` or `pdflatex`. This file is than converted into a *PDF* document using such an engine.

The Markdown dialect understood by *umdoc* was influenced by *[GitHub](http://github.com) flavored Markdown* and some [Pandoc](http://pandoc.org) Markdown extensions.

The tool can be used in three ways:
1. It can directly convert a Markdown file (ending with `.md`) into *PDF*:
   ```
umdoc example.md
```
2. It can read and convert an *umdoc* *XML* file (see section~[](#umdoc-xml-file)), which defines markdown input file and optional layout information, into a *PDF* document:
   ```
umdoc example.xml
```

3. It can simply be launched in a working directory where it expects to find an *umdoc* *XML* file with the name `umdoc.xml`:
   ```
umdoc
```

## Motivation

There are plenty of options out there if you want to write documentation that can be delivered on paper or in form of a *PDF*. A WYSIWYG word processor can be used, you can use a sophisticated markup language like *LaTeX* or a anything in between.
But writing documentation should be simple and convenient. If you collaborate with others, you want to use something that can easily be version controlled and merged with version control systems like *Git*. Hence, using tools that rely on a more or less unreadable formant are not an option.
Markup languages have other disadvantages. You have to know them to use them. Markdown on the other hand can easily be learned simply by looking at a Markdown document. 
<!-- Using a markup language like *LaTeX* might be an option, but it will most definitely cause other issues since it requires extensive knowledge about the language to be used properly. -->
So, using something like Markdown and converting it into a neatly formatted document seems like the perfect solution if your documentation needs can be met with basic text formatting constructs.
But, if it actually comes to converting Markdown into a document, you will realize that it is not as easy as it should be.

This is because Markdown was designed to be converted into inner text *HTML* and not into pages. To create a document, you will probably require:
* The possibility to define the basic layout of document (cover page, table of contents, etc.).
* The possibility to define a page header and footer.
* Support for tables
* Figure and table environments
* Cross references
* Syntax highlighting

All this cannot not be accomplished with traditional Markdown without heavily relying on *HTML*. Hence, you require some extensions of the Markdown language and a way to define the layout of the document to be created.

*umdoc* tries to overcome the shortcomings of traditional Markdown by ...


<!-- Writing documentation of any kind should be simple. -->

<!-- If you are a software developer, than you probably think that writing and maintaining documentation should be as easy as writing software, if not easier. For this very reason using plain text README files... -->

<!-- 
# Alternatives

* markdown was not designed to be converted into pdf
* viele einfache tools
* customizability
* pandoc

-->

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
* Create output in other formats than *PDF* or *HTML*.

# Installation

The *umdoc* tool consists of a single binary without any notable dependencies except *LaTeX*. To install it, simply download the binary for your platform, place it somewhere on your system and ensure that it can be found with the *PATH* environment variable. You can also call the *umdoc* tool directly if you do not want to touch your *PATH* variable.

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

# Usage

The *umdoc* executable can be launched like this:

```
umdoc [<input-file>] [-o <output-file>] [-e <latex-engine>] [-a <aux-directory>] [-t]
```

The default behavior of *umdoc* is to look in the working directory for a `umdoc.xml` file, which is converted into a `.tex` and into a *PDF* in the same directory. This default behavior can be changed by specifying the input file with `<input-file>` and/or the output file with `-o <output-file>`.

##### Options

* `<input-file>`

  The input file, which can be an *umdoc* *XML* file or a `.md` file. The default is `umdoc.xml`.
  
* `-o <output-file>`, `--output=<output-file>`

  The name or the path of the output *PDF* file. The default is derived from the input file where the file name extension is replaced by `.pdf`.
  
* `-e <latex-engine>`, `--engine=<latex-engine>`

  The launch command of the *LaTeX* engine to be used to convert the created `.tex` document into *PDF*. The default is `lualatex`.

* `-a <aux-directory>`, `--aux-directory=<aux-directory>`
 
  A directory for intermediate files like the created `.tex` file or files created from the *LaTeX* engine. The default is the directory of the output file plus the base name of the output file.
   
* `-t`, `--tex`

  Stop after creating the `.tex` file.

## The *umdoc* *XML* File {#umdoc-xml-file}

The purpose of the *umdoc* is to provide the *umdoc* with everything it needs to create the output file. This includes: Markdown input files, *LaTeX* "header" files, and basic layout information like where to insert the table of contents, page breaks or pages from other sources like `.pdf` or `.tex` files.

# Supported Markdown Features

## Titles

```
# Example Title

## Example Title Level 2

Example text.
```

results in:
```latexexample
# 1 ~ ~ ~Example Title {-}

## 1.1 ~ ~ ~Example Title Level 2 {-}

Example text.
```

## Cross References

This references to section~[](#horizontal-rules).

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

## Inline Links

```
This is an [example link](https://github.com/craflin/umdoc).
```

results in:
```latexexample
This is an [example link](https://github.com/craflin/umdoc).
```

## Code Spans

```
Some text with code like `printf()`. Code span with backticks `` `test` ``.
```

results in:
```latexexample
Some text with code like `printf()`. Code span with backticks `` `test` ``.
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

## Inline Images

```
Text with inline an inline image like this ![](circle.png).
```

results in:
```latexexample
Text with inline an inline image like this ![](circle.png).
```

## Fenced Code Blocks

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








