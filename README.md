# platformake [![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE) [![Build Status](https://travis-ci.org/nabijaczleweli/platformake.svg?branch=master)](https://travis-ci.org/nabijaczleweli/platformake)
A zero-effort pseudo-preprocessor for multiplatforming<sup>1</sup> Makefiles

## Libraries used
1. [`TCLAP`](http://tclap.sourceforge.net/) for parsing command line arguments <sub>(it's really cool, seriously, go check it out)</sub>
2. [`simpleopt`](https://github.com/brofield/simpleopt) for file globbing
3. [`pcre2`](http://www.pcre.org) for Perl-compatible regular expressions

## Building
Building requires [GNU Make](https://www.gnu.org/software/make) for main files as well as [CMake](http://www.cmake.org) and [`ninja`](https://martine.github.io/ninja) for building `pcre2`.

Before building the project for the first time run `make deps`, this will build and copy over all the dependencies used.<br />
To build it all the subsequent times just run `make`

Be warned: `make clean` cleans up *only* the *de iure* project files, to clean the dependencies as well, use `make clean-all`

On Windows, the build requires [`busybox`](http://frippery.org/busybox)

## [License](LICENSE)
MIT - a.k.a. do anything with it, but tell people I made it and don't hold me liable for anything.

<sub>multiplatforming<sup>1</sup> - the act of making something multiplatform</sub>
