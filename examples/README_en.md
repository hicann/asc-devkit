# Ascend C Examples

English | [简体中文](./README.md)

This directory provides usage examples for different Ascend C programming modes, covering basic invocation, debugging tools, features, API libraries, performance practices, and other content.

## Example List

| Directory Name | Description |
| --- | --- |
| [01_simd_cpp_api](./01_simd_cpp_api) | Ascend C C++ API examples, covering getting started, tools, features, API libraries, best practices, compatibility references, and Tensor API programming samples |
| [02_simd_c_api](./02_simd_c_api) | Ascend C C API examples, covering basic invocation, tool capabilities, and API features |
| [03_simt_api](./03_simt_api) | Ascend C SIMT programming examples, covering getting started, debugging tools, core features, and practice references |
| [04_aicpu](./04_aicpu) | Ascend C AICPU programming examples, covering getting started and features |
| [05_simd_simt_hybrid](./05_simd_simt_hybrid) | Ascend C SIMD and SIMT hybrid programming examples, covering getting started and high-performance optimization examples |

## npu-arch Build Option Description

Developers need to modify the `--npu-arch` build option in the `CMakeLists.txt` file under the specific example directory according to the actual execution environment. The correspondence between product models and `npu-arch` parameters is as follows.

| Product Model | npu-arch Parameter |
| ---- | ---- |
| Ascend 950PR/Ascend 950DT | --npu-arch=dav-3510 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products | --npu-arch=dav-2201 |
| Atlas Inference Series Products AI Core | --npu-arch=dav-2002 |
