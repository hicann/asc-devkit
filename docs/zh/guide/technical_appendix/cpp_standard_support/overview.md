# 概述

本文介绍C/C++语言标准在Ascend C中的支持情况，包括语言标准的选择方式、Device侧的语法特性支持情况与使用约束，以及本章节的组织结构。Ascend C源文件由bisheng编译器编译，Host侧与Device侧代码在同一编译单元内统一处理，整体支持情况如下：

-   Host侧与clang15一致，支持完整的C/C++标准。
-   Device侧支持C++11、C++14与C++17三档语言标准，默认按C++17编译。

## 语言标准的选择

在bisheng编译命令中添加`-std=c++<version>`或`--std=c++<version>`，可指定本次编译使用的C++语言标准。该选项同时作用于Host侧和Device侧代码。Device侧可选择的语言标准及对应编译选项如下：

-   C++11（ISO/IEC 14882:2011）：`-std=c++11`
-   C++14（ISO/IEC 14882:2014）：`-std=c++14`
-   C++17（ISO/IEC 14882:2017）：`-std=c++17`，默认值

Host侧与clang15一致，遵循其默认支持的语言标准范围。低于C++11的标准，如C++98、C++03，Device侧不支持。

**表1**  语言标准的指定方式

| 指定方式 | 用法 |
| --- | --- |
| bisheng命令行编译 | 在编译命令中追加`-std=c++<version>` |
| CMake工程 | 设置`CMAKE_ASC_STANDARD`变量 |

> [!NOTE]说明
>
> - 指定低于C++17的标准时，部分高版本语言特性会以clang扩展告警的形式继续可用，而非全部变为编译错误；需要严格限制语言版本时，可配合`-Werror`编译选项将此类告警升级为错误。
> - 部分API接口内部实现依赖C++17标准，若指定低于C++17的标准，可能导致部分API无法使用。
>

## 语言特性与限制导航

-   **[Device侧C++语言支持](./cpp_language_feature/cpp_language_feature.md)**：C++语言特性在Device侧的支持情况。
    -   [C++11语言特性](./cpp_language_feature/cpp11_language_feature.md)：C++11核心语言特性在Device侧的支持列表。
    -   [C++14语言特性](./cpp_language_feature/cpp14_language_feature.md)：C++14语言特性在Device侧的支持列表。
    -   [C++17语言特性](./cpp_language_feature/cpp17_language_feature.md)：C++17语言特性在Device侧的支持列表。
    -   [Lambda表达式](./cpp_language_feature/lambda_expressions.md)：不同编程场景下Lambda表达式的使用方式与注意事项。
-   **[Device侧C/C++语法限制](./cpp_language_restrictions/cpp_language_restrictions.md)**：Device侧通用的语法与能力限制。
    -   [C/C++通用语法限制](./cpp_language_restrictions/general_restrictions.md)：指针与内存地址、变量、函数、类、模板等语法使用限制，含核函数参数列表限制、SIMT VF函数限制、SIMD与SIMT混合编程结构体使用限制。
