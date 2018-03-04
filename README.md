
# umdoc

*umdoc* is a Markdown to *LaTeX* to *PDF* converter.
It is small command line tool, which converts a Markdown file or set of Markdown files with optional layout information written in *LaTeX* into an input file (`.tex` file) for a *LaTeX* engine like `lualatex` or `pdflatex`. This file is then converted into a *PDF* document using such an engine.

## Examples & Documentation

Further documentation and a showcase of *umdoc*'s capabilities can be found in its [user manual](https://github.com/craflin/umdoc/releases/download/0.1.4/umdoc-0.1.4.pdf).

## Build Instructions

To compile *umdoc* from sources follow the following steps:

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