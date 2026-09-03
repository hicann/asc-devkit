# Ascend C样例

简体中文 | [English](./README_en.md)

本目录提供Ascend C不同编程方式的使用样例，覆盖基础调用、调试工具、功能特性、API类库和性能实践等内容。

## 样例列表

| 目录名称 | 内容说明 |
| ---- | ---- |
| [01_simd_cpp_api](./01_simd_cpp_api) | Ascend C C++ API样例，覆盖入门、工具、功能特性、API类库、最佳实践、兼容性参考和Tensor API编程样例 |
| [02_simd_c_api](./02_simd_c_api) | Ascend C C API样例，覆盖基础调用、工具能力和接口特性 |
| [03_simt_api](./03_simt_api) | Ascend C SIMT编程样例，覆盖入门、调试工具、核心特性和实践参考 |
| [04_aicpu](./04_aicpu) | Ascend C AICPU编程样例，覆盖入门和功能特性 |
| [05_simd_simt_hybrid](./05_simd_simt_hybrid) | Ascend C SIMD与SIMT混合编程样例，覆盖入门和高性能优化样例 |

## npu-arch编译选项说明

开发者需根据实际执行环境，修改具体样例目录下CMakeLists.txt文件中的--npu-arch编译选项。产品型号与npu-arch参数的对应关系如下。

| 产品型号 | npu-arch参数 |
| ---- | ---- |
| Ascend 950PR/Ascend 950DT | --npu-arch=dav-3510 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 | --npu-arch=dav-2201 |
| Atlas 推理系列产品AI Core | --npu-arch=dav-2002 |
