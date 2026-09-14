# BiSheng Compiler Overview<a name="ZH-CN_TOPIC_0000002457558450"></a>

<!-- md-trans-meta sourceCommit=b95888b990f715970dcd51b458687611b5196a2c translatedAt=2026-08-26T10:51:37.981Z pushedAt=2026-08-29T10:02:23.364Z -->

BiSheng Compiler is a heterogeneous compiler designed specifically for Ascend AI processors. It natively supports the compilation of Ascend C operator code and can efficiently convert operator programs written by users into binary executable files or dynamic libraries. The compiler executable is named bisheng, supports multiple host platforms such as x86 and aarch64, and directly provides instruction set compilation support for the device-side AI Core architecture. With BiSheng Compiler, developers can complete operator development and optimization for Ascend platforms more efficiently and conveniently.

Currently, the compiler supports the following models:
-   Ascend 950PR/Ascend 950DT
-   Atlas A3 training products/Atlas A3 inference products
-   Atlas A2 training products/Atlas A2 inference products
-   Atlas inference products

The source files and header files supported by BiSheng Compiler are as follows:
| File Name Extension | Description | Content |
|--|--|--|
|.c| C source file | Host-only code | 
|.cpp, .cc, .cxx | C++ source file | Host-only code | 
|.h, .hpp, .hh, .hxx | C/C++ header file | Device code, host code, host/device mixed code | 
|.asc | Ascend C source file | Device code, host code, host/device mixed code | 

-   For programming constraints in heterogeneous compilation scenarios, see [Constraints](constraints.md).
-   For more basic knowledge about BiSheng Compiler, see [*BiSheng Compiler User Guide*](https://www.hiascend.com/document/redirect/CannCommunityBiSheng).
