# Overview

<!-- md-trans-meta sourceCommit=0b5b2914f0d49ced19c24b1e0a1c213cf196c7af translatedAt=2026-11-09T11:12:47.003Z pushedAt=2026-11-19T06:32:20.004Z nl=2 -->

This document describes the support for C/C++ language standards in Ascend C, including how to select a language standard, the supported syntax features and usage constraints on the device side, and the organization of this chapter. Ascend C source files are compiled by the BiSheng compiler. Host-side and device-side code are processed together in the same compilation unit. The overall support is as follows:

-   The host side is consistent with clang15 and supports the complete C/C++ standards.
-   The device side supports three language standards: C++11, C++14, and C++17, and is compiled with C++17 by default.

## Language Standard Selection

In both SIMD and SIMT programming scenarios, you can specify the C++ language standard for a compilation unit through the `-std=c++<version>` or `--std=c++<version>` compilation option. This option enables all language features of the selected standard and applies to the compilation process on both the host side and the device side. The language standards available on the device side and their corresponding compilation options are as follows:

-   C++11 (ISO/IEC 14882:2011): `-std=c++11`
-   C++14 (ISO/IEC 14882:2014): `-std=c++14`
-   C++17 (ISO/IEC 14882:2017): `-std=c++17`, the default value

The host side is consistent with clang15 and follows its default supported language standard range. Standards earlier than C++11, such as C++98 and C++03, are not supported on the device side.

**Table 1** Specified manner of the language standard

| Specified Manner | Usage |
| --- | --- |
| bisheng command-line compilation | Append `-std=c++<version>` to the compilation command |
| CMake project | Set the `CMAKE_ASC_STANDARD` variable |

> [!NOTE] Note
> - When a standard earlier than C++17 is specified, some higher-version language features remain available in the form of clang extension warnings instead of all becoming compilation errors. To strictly restrict the language version, you can use the `-Werror` compilation option to escalate such warnings to errors.
> - Some API interfaces rely on the C++17 standard in their internal implementation. If a standard earlier than C++17 is specified, some APIs may become unavailable.

## Language Features and Restrictions Navigation

-   **[Device-Side C++ Language Support](./cpp_language_feature/cpp_language_feature.md)**: Support status of C++ language features on the device side.
    -   [C++11 Language Features](./cpp_language_feature/cpp11_language_feature.md): Support list of C++11 core language features on the device side.
    -   [C++14 Language Features](./cpp_language_feature/cpp14_language_feature.md): Support list of C++14 language features on the device side.
    -   [C++17 Language Features](./cpp_language_feature/cpp17_language_feature.md): Support list of C++17 language features on the device side.
    -   [Lambda Expressions](./cpp_language_feature/lambda_expressions.md): Usage and precautions of lambda expressions in different programming scenarios.
-   **[Device-Side C/C++ Syntax Restrictions](./cpp_language_restrictions/cpp_language_restrictions.md)**: General syntax and capability restrictions on the device side.
    -   [General C/C++ Syntax Restrictions](./cpp_language_restrictions/general_restrictions.md): Usage restrictions on pointers and memory addresses, variables, functions, classes, templates, and other syntax, including kernel function parameter list restrictions, SIMT VF function restrictions, and restrictions on using structures in SIMD and SIMT mixed programming.
