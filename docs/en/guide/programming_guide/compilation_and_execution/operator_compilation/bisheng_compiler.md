# BiSheng Compiler Overview<a name="ZH-CN_TOPIC_0000002457558450"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T15:04:53.940Z pushedAt=2026-11-03T07:05:13.457Z nl=2 -->

BiSheng Compiler is a heterogeneous compiler designed specifically for Ascend AI processors. It natively supports the compilation of Ascend C operator code and can efficiently convert operator programs written by users into binary executable files or dynamic libraries. The compiler executable is named bisheng, supports multiple host platforms such as x86 and aarch64, and directly provides instruction set compilation support for the device-side AI Core architecture. With BiSheng Compiler, developers can complete operator development and optimization for Ascend platforms more efficiently and conveniently.

Currently, the compiler supports the following models:
<!-- npu="950" id1 -->
-   Ascend 950PR&950DT products
<!-- end id1 -->
<!-- npu="A3" id2 -->
-   Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
-   Atlas A2 products
<!-- end id3 -->
<!-- npu="310p" id4 -->
-   Atlas inference products
<!-- end id4 -->

The source files and header files supported by BiSheng Compiler are as follows:

| File Name Extension | Description | Content |
|--|--|--|
|.c| C source file | Host-only code | 
|.cpp, .cc, .cxx | C++ source file| Host-only code | 
|.h, .hpp, .hh, .hxx | C/C++ header file| Device code, host code, host/device mixed code | 
|.asc | Ascend C source file| Device code, host code, host/device mixed code |

-   For programming-related constraints in heterogeneous compilation scenarios, see [Constraints](constraints.md).
-   For more basic knowledge about BiSheng Compiler, see [BiSheng Compiler](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/920/compiler/BishengCompiler/atlas_bisheng_10_0001.html).
