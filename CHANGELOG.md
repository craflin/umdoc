
# 0.1.2 (unreleased)

* Added support for footnotes

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
