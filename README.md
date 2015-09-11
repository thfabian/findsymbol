# FindSymbol - 1.0 [![Build Status](https://travis-ci.org/thfabian/findsymbol.svg?branch=master)](https://travis-ci.org/thfabian/findsymbol)
FindSymbol is a tool to scan your system for specific symbols and report their location, usually archives (.a) or shared objects (.so). This is useful to resolve the location of undefined symbols during the linking phase of the compilation.

## Example
Suppose you want to compile the following minimalistic Qt5 example (example.cpp):
```c++
#include <QtWidgets/QApplication>
 
int main(int argc, char* argv[]) 
{
    QApplication app(argc, argv);
    return app.exec();
}
```
with
```sh
$ clang++ -o example -I/usr/include/qt5 -fPIC example.cpp 
```
you will get a linking error looking something like:
```
$ /tmp/example-754cd1.o: In function `main':
$ example.cpp:(.text+0x2f): undefined reference to `QApplication::QApplication(int&, char**, int)'
$ example.cpp:(.text+0x34): undefined reference to `QApplication::exec()'
$ example.cpp:(.text+0x52): undefined reference to `QApplication::~QApplication()'
$ example.cpp:(.text+0x6d): undefined reference to `QApplication::~QApplication()'
$ clang: error: linker command failed with exit code 1 (use -v to see invocation)
```
Which most likely means you haven't linked against the correct libraries. In some cases it can get very tedious to determine the proper libraries and hence this is where FindSymbol comes in handy. To resolve for example the first symbol we invoke:
```sh
$ findsymbol -demangle "QApplication::QApplication(int&, char**, int)"
```
The argument `-demangle` tells the program to demangle C++ mangled names in libraries. The result of the invocation:
```
$ Symbol `QApplication::QApplication(int&, char**, int)' found in:
$  -- /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5.2.1
```
which suggests we need to link against `libQt5Widgets` and hence the following compiles without errors.
```sh
$ clang++ -o example -I/usr/include/qt5 -fPIC example.cpp -lQt5Widgets 
```
<span style="color:red">Note:</span> the very first invocation commonly takes significantly longer (10-100x) as the program is highly IO bound and scans thousand of archives and shared objects which are usually not in memory, resulting in a tremendous amount of page faults. This example shows two consecutive invocations:
```
$ findsymbol -show-stats "whatever"
$ No libaries found for symbol `whatever'.
$ Parsed symbols:       5083413 
$ Scanned files:          39858 
$ Elapsed time:           14345 ms
$
$ findsymbol -show-stats "whatever"
$ No libaries found for symbol `whatever'.
$ Parsed symbols:       5083413 
$ Scanned files:          39858 
$ Elapsed time:             345 ms

```

## Installation
FindSymol is built upon [LLVM](http://llvm.org/) (3.6) with further dependencies on:
* [GNU libiberty](https://code.google.com/p/toolbox-of-eric/downloads/detail?name=libiberty.tar.gz&can=2&q=)
* [pugixml](http://pugixml.org/)
* [Intel(R) TBB](https://www.threadingbuildingblocks.org/)

If you are using Ubuntu you can install the additional dependencies with:
```sh
$ sudo apt-get install libiberty-dev libpugixml-dev libedit-dev libtbb-dev 
```
For LLVM (3.6) packages take a look [here](http://llvm.org/apt/).

To compile the program:
```sh
$ git clone https://github.com/thfabian/findsymbol
$ cd findsymbol
$ mkdir build
$ cd build
$ cmake ../
$ make
$ sudo make install
```

In case you want to install the autocomplete script for command-line options, configure cmake with:
```sh
$ cmake -DFS_INSTALL_BASH=ON ../
```
By default it will install the completion script in `/etc/bash_completion.d`, this behaviour can be altered with `-DFS_INSTALL_BASH_DIR=PATH`.

## License
[MIT](https://opensource.org/licenses/MIT)
