
# 0.2.6

* Fixed dead lock caused by non utf-8 characters

# 0.2.5

* Added support for placeholders in markdown files
* Fixed issue where lines are accidentally interpreted as a horizontal rule

# 0.2.4

* Fixed table parsing issue

# 0.2.3

* Refactored table parsing
* Build Windows builds with /MT

# 0.2.2

* Fixed configuration mapping issue with Visual Studio CDeploy package
* Added support for grid table syntax
* Fixed issues with paragraphs in tables

# 0.2.1

* Added support for older latex engines by translating non breaking space to ~
* Switched to CDeploy to create packages

# 0.2.0

* Added support for custom environment command execution
* Added support for mathematical LaTex expressions
* Added HTML generator
* Removed -t flag and recognize file format based on the output file extension
* Added "xplain" environment for environment that span multiple pages
* Switched to xelatex by default
* Added "xtab" table style
* Support figures width and height attributes
* Introduced new syntax to insert a section number in cross document references
* Changed handling of the character ~, its no longer equal to a non breaking space
* Added support for HTML 1.0 entities and for &nbsp;
* Switched to CMake and CPack

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

# 0.1.0

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
