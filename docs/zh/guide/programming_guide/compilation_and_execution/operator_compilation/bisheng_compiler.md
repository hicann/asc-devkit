# 毕昇编译器简介<a name="ZH-CN_TOPIC_0000002457558450"></a>

毕昇编译器是专为昇腾AI处理器设计的异构编译器，原生支持Ascend C算子代码的编译，可将用户编写的算子程序高效转换为二进制可执行文件或动态库。编译器的可执行文件名为bisheng，支持x86、aarch64等多种主机平台，并直接提供对设备侧AI Core架构的指令集编译支持。借助毕昇编译器，开发者可以更高效、便捷地完成面向昇腾平台的算子开发与优化工作。

目前，编译器支持如下型号：
<!-- npu="950" id1 -->
-   Ascend 950PR/Ascend 950DT
<!-- end id1 -->
<!-- npu="A3" id2 -->
-   Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id2 -->
<!-- npu="910b" id3 -->
-   Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id3 -->
<!-- npu="310p" id4 -->
-   Atlas 推理系列产品
<!-- end id4 -->

毕昇编译器支持的源码文件核头文件如下：
| 文件扩展名 | 描述 | 内容 |
|--|--|--|
|.c| C源码文件 | Host-only代码 | 
|.cpp, .cc, .cxx | C++源码文件| Host-only代码 | 
|.h, .hpp, .hh, .hxx | C/C++头文件| Device代码， Host代码， Host/Device混合代码 | 
|.asc | Ascend C源码文件| Device代码， Host代码， Host/Device混合代码 | 

-   异构编译场景中的编程相关约束请参考[约束说明](constraints.md)。
-   如果需要了解更多的毕昇编译器的基础知识，详细内容请参考[《毕昇编译器用户指南》](https://www.hiascend.com/document/redirect/CannCommunityBiSheng)。
