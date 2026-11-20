# Ascend C API List

<!-- md-trans-meta sourceCommit=db723736615bf264561ad2b25178911df894c2f2 translatedAt=2026-11-13T08:54:40.891Z pushedAt=2026-11-16T07:07:41.584Z nl=2 -->

Ascend C provides a hierarchical API system that covers complete capabilities from low-level C extensions to high-level C++ class libraries. It enables developers to use standard C/C++ syntax as the basis to flexibly implement fine-grained memory management and efficient vector/cube computation under multiple programming models such as AI Core (SIMD/SIMT) and AI CPU.

## API Classification Overview

The following table shows the overall classification of Ascend C APIs, helping developers quickly locate the required APIs based on the programming model and functional requirements.

**Table 1**  Ascend C API Classification

| API Level-1 Classification | API Level-2 Classification | Classification Description |
| --- | --- | --- |
| [SIMD API](SIMD-API/basic_api/basic_api_list.md) | [Basic API](SIMD-API/basic_api/basic_api.md) | Implements the abstraction of hardware capabilities, exposes chip capabilities, and ensures completeness and compatibility. APIs marked as ISASI (Instruction Set Architecture Special Interface, APIs related to the hardware architecture) cannot guarantee compatibility across hardware versions. |
| [SIMD API](SIMD-API/SIMD-API.md) | [Tensor API](SIMD-API/tensor_api/tensor_api_list.md) | Describes data through Tensor and its Layout, and provides the currently supported data movement and cube computation capabilities. Uncovered hardware capabilities can be used together with the C API. |
| [SIMD API](SIMD-API/c_api/c_api.md) | [C API](SIMD-API/c_api/c_api.md) | Pure C API that exposes the complete chip programming capability, supports array-based memory allocation, is generally based on pointer programming, and provides a C language programming experience consistent with the industry. |
| [SIMD API](SIMD-API/adv_api/adv_api_list.md) | [Advanced API](SIMD-API/adv_api/adv_api.md) | Implements some commonly used computation algorithms to improve programming and development efficiency, and usually calls multiple basic APIs for implementation. High-level APIs include the math library, Matmul, Softmax, and other APIs. High-level APIs can guarantee compatibility. |
| [SIMT API](SIMT-API/overview.md) | - | Aligned with the industry, provides single instruction multiple thread APIs. Implements parallel computation in the form of a single instruction with multiple threads. SIMT programming is mainly used for vector computation and is especially suitable for scenarios such as discrete access and complex control logic. SIMT API supports two programming models: SIMT programming and SIMD-SIMT hybrid programming. For the specific supported APIs, see [SIMT Programming API List](SIMT-API/SIMT_programming_intro/api_list.md) and [SIMD-SIMT Hybrid Programming API List](SIMT-API/SIMD_SIMT_hybrid_programming_intro/api_list.md). |
| [AI CPU API](AI-CPU-API/ai_cpu_api_list.md) | - | Usually serves as a supplement to the above APIs, mainly handling branch-intensive computation that is non-cube and has relatively complex logic. |
| [Utils API](Utils-API/utils_api_list.md) | - | Rich general-purpose utility classes, covering functions such as the standard library (currently only SIMD is supported), platform information acquisition, runtime compilation, and log output, supporting developers in efficiently implementing operator development and performance optimization. |

## Header Files and Library Files Required for Calling APIs

After the firmware, driver, and CANN software package are installed, the header files and library files of the Ascend C API can be referenced when compiling and running applications.

You can include the dependent files according to the Ascend C APIs actually used. The purpose of each header file is shown in the following table.

**Table 2**  SIMT Programming Model Header File List

| Header File | Description | Corresponding Library File |
| --- | --- | --- |
| simt_api/asc_simt.h | Provides the SIMT API external API. If the computation requires only a specific data type such as fp8, only simt_api/asc_fp8.h needs to be included. | Not involved |

**For more details about SIMT programming header file inclusion, see [SIMT-API](SIMT-API/overview.md).**

**Table 3**  SIMD Basic Programming Capability Header File List

| API Capability Level | Development Scenario | Header File | Description | Corresponding Library File |
| --- | --- | --- | --- | --- |
| C language pointer programming | Develop any operator (including fused operators) | c_api/asc_simd.h | Provides the full set of SIMD C API external APIs. | Not involved |
| C++ Tensor programming | Develop data movement, cube computation, and related fused operators | tensor_api/tensor.h | Provides the currently supported SIMD C++ Tensor API external APIs. Uncovered capabilities can be used together with the C API. | Not involved |
| TPipe/TQue framework programming | Develop any operator (including fused operators) | basic_api/kernel_basic_intf.h | Provides the full set of SIMD TPipe/TQue framework programming external APIs. | Not involved |
| TPipe/TQue framework programming | Develop only vector operators | basic_api/kernel_vec_intf.h | Provides the SIMD TPipe/TQue framework programming external APIs on which vector computation depends. | Not involved |
| TPipe/TQue framework programming | Develop only cube operators | basic_api/kernel_cube_intf.h | Provides the SIMD TPipe/TQue framework programming external APIs on which cube computation depends. | Not involved |

**Table 4**  SIMD High-level API Header File List

| Header File | Description | Corresponding Library File |
| --- | --- | --- |
| adv_api/kernel_api.h | Provides the SIMD high-level API Device-side external API. It can be called only in the TPipe/TQue framework programming mode. | Not involved |
| adv_api/tiling_api.h | Provides the SIMD high-level API Host-side external API. It can be called only in the TPipe/TQue framework programming mode. | libtiling_api.a |
