# typecheck

This project is a language-agnostic typechecker.  It is designed to be abstract from any particular language, but rather provide a framework that language-implementers can use to handle typechecking their statically-typed languages.

## Motivation
This project was created out of my own need for a typechecker for my own compiler for [pseudocode](https://github.com/mattpaletta/pseudocode) (soon to be open-sourced).  I decide to separate the typechecker firstly so it could be easily tested as a different project, and secondly so I, and others, could use it for other projects in the future and benefit from any improvements made to this library over time.  You can see other projects using the project on the `THIRD_PARTY.md` file in this repo.

## Build Status
[![Build Status](https://travis-ci.com/mattpaletta/typecheck.svg?token=ysncAybhRTtbpjrpSW8S&branch=master)](https://travis-ci.com/mattpaletta/typecheck)

## Getting Started
To save this document from getting too long, this has been moved to a separate document, `GettingStarted.md`, found in this repo.  This goes over the API at a high-level.  For more examples, you can find tests in the `test/` folder.

## Code Style
Code style is checked by clang-tidy (*ON* by default).  You can check it by running with:
```bash
cd ${PROJECT_ROOT}/build
cmake -DENABLE_CLANG_TIDY=ON ..
```

## Build Options
The supported build tool is CMake.  All of the CMake build options have been placed in a single file, which you can view here: [CMake Build Options](https://github.com/mattpaletta/typecheck/blob/master/cmake/options.cmake)

## Supported Platforms
Currently being tested using [Travis CI](https://travis-ci.com/mattpaletta/typecheck.svg?token=ysncAybhRTtbpjrpSW8S&branch=master) on Windows, Mac, and Ubuntu, compiling with:
- MSVC
- gcc
- clang

Typecheck requires C++17.

## Dependencies
This library is designed to generally use versions of libraries pre-installed on your system, but if they are not available, build them from source automatically.
Libraries this project depends on:
- [Protobuf](https://github.com/protocolbuffers/protobuf) (WIP to remove this dependency)
- [Catch2](https://github.com/catchorg/Catch2) (only for testing)

## How to use?
This project was build for, and is used in my own project: [Pseudocode](https://github.com/mattpaletta/pseudocode)
(Soon to be opensourced, if not already.)
```cmake
fetch_extern(typecheck https://github.com/mattpaletta/typecheck master)
```
I use `FetchContent` to along with a helper function to grab this library.  You can see that function here: [fetch_extern](https://github.com/mattpaletta/typecheck/blob/master/cmake/fetch_extern.cmake).
Alternatively, you can add it as a git submodule include the directory:
```cmake
add_subdirectory(typecheck)
```
You can then link it to your project:
```cmake
add_library(my_library ...)
target_link_libraries(my_library PUBLIC ... typecheck ...)
```

## Contribute
I welcome contributions of all sorts.  I consider myself new to the open-source community, so if you're looking for things to contribute, here are some ideas to get started:
- Spelling errors in comments & variable names
- Improve test coverage + add edge cases (build with `ENABLE_COVERAGE`)
- Improvements on performance, readability, etc.
- Suggestions or ideas of larger improvements (leave an issue, and we can discuss)
- Improvements to documentation or code comments to add or update where relevant

## Credits
The algorithm used in this project are heavily influenced by the Hindley-Milner type system and the [Swift](https://swift.org/) type checker.  Some of the excellent resources used are listed below:
- [Hildley-Milner Type System (Wiki)](https://en.wikipedia.org/wiki/Hindley–Milner_type_system)
- [Hindley-Milner Interfence - Write You A Haskell - Stephen Diehl](http://dev.stephendiehl.com/fun/006_hindley_milner.html)
- [Exponential time complexity in the Swift type checker](https://www.cocoawithlove.com/blog/2016/07/12/type-checker-issues.html)
- [Type Checker Design & Implementation - Apple Swift 2.2 Docs](https://apple-swift.readthedocs.io/en/latest/TypeChecker.html)

## Information

### Questions, Comments, Concerns, Queries, Qwibbles?

If you have any questions, comments, or concerns please leave them in the [GitHub Issues Tracker](https://github.com/mattpaletta/typecheck/issues)

### Bug reports

If you discover any bugs, feel free to create an issue on GitHub. Please add as much information as possible to help us fixing the possible bug. We also encourage you to help even more by forking and sending us a pull request.

## Maintainers

* Matthew Paletta (https://github.com/mattpaletta)

## License

GPL-3.0 License. Copyright 2020 Matthew Paletta. http://mrated.ca
