
# umdoc

*umdoc* is a Markdown to *LaTeX* to *PDF* converter.
It is small command line tool, which converts a Markdown file or set of Markdown files into an input file (`.tex` file) for a *LaTeX* engine like `lualatex` or `pdflatex`.
The *LaTeX* engine is then launched to convert the generated file into a *PDF* document.
Optional layout information written in *LaTeX* may be provided to customize the look and feel of the generated document.

## Examples & Documentation

Further documentation and a showcase of *umdoc*'s capabilities can be found in its [user manual](https://github.com/craflin/umdoc/releases/download/0.1.6/umdoc-0.1.6.pdf).

## Build Instructions

To compile *umdoc* from the sources follow the following steps:

### Windows

* Clone the Git repository.
* Initialize submodules.
* Use CMake to generate a solution file (`umdoc.sln`) for some version of Visual Studio.
* Open the generated `umdoc.sln` file in Visual Studio.
* Compile the *umdoc* project in Visual Studio.

### Linux

* Ensure you have `git`, `g++` and `cmake` installed.
* Clone the Git repository. `git clone git@github.com:craflin/umdoc.git`
* Initialize submodules. `git submodule update --init`
* Use CMake to build *umdoc*.