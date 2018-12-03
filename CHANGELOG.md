
# 0.1.6

* Build Windows build with /MT

# 0.1.5

* Support XML comments in verbatim environments
* Support cross references to figures and tables
* Improved the user manual

# 0.1.4

* Added support for tilde character

# 0.1.3

* Added support for tables with caption
* Added *tablesfigure* example, which demonstrations generating a list of figures or tables 
* Added support for grid tables
* Fixed replacing placeholders in header tex files

# 0.1.2

* Added support for footnotes
* Allow line breaks after or before some special characters
* Fixed resource file including issue
* Fixed issue with titlesec section numbering on Ubuntu 16.04

# 0.1.1

* Fixed not working conversion of Markdown to *PDF* without a configuration file 
* Fixed access to input file when *umdoc* should operate in the working directory
* Added *md2tex* example

# 0.1

* The initial release of *umdoc* supports the following features:
    * Importing styles from *TEX* files
    * Inserting of *TEX* files
    * Inserting of *PDF* files
    * Inserting a table of contents
    * Creation of documents with multiple parts
    * A front end for *lualatex*/*pdflatex*
    * Support for custom *LaTeX* classes
    * Support for custom *LaTeX* environments
* The following Markdown features are supported:
    * Simple paragraphs
    * Emphasis
    * Setext headers
    * Atx headers
    * Blockquotes
    * Bullet lists
    * Numbered lists
    * Horizontal rules
    * Inline links
    * Code spans
    * Inline images
    * Underscores in words
    * Fenced code blocks
    * Tables
    * Header attributes
    * Implicit figures
* The package includes the following examples:
    * *coverpage* - A configuration, which includes a *PDF* page    
    * *customenvironment* - A configuration, which defines a custom environment
    * *headerfooter* - A configuration, which uses the *LaTeX* package *fancyhdr*
    * *latexclass* - A configuration, which uses a custom *LaTeX* class
