# Ascend C API List

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-06T12:46:32.051Z pushedAt=2026-09-11T07:22:23.151Z -->

Ascend C provides a hierarchical API system that covers complete capabilities ranging from low-level C extensions to high-level C++ class libraries. Based on standard C/C++ syntax, it enables developers to flexibly implement fine-grained memory management and efficient vector/matrix operations under multiple programming models such as AI Core (SIMD/SIMT) and AI CPU.

## API Classification Overview

The following table shows the overall classification of Ascend C APIs, helping developers quickly locate the required APIs based on the programming model and functional requirements.

**Table 1**  Ascend C API Classification

| API Level-1 Category | API Level-2 Category | Classification Description |
| --- | --- | --- |
| [SIMD API](SIMD-API/basic_api/basic_api_list.md) | [Basic API](SIMD-API/basic_api/basic_api.md) | Abstracts hardware capabilities, exposes chip capabilities, and ensures completeness and compatibility. APIs marked as ISASI (Instruction Set Architecture Special Interface, which are hardware-architecture-related APIs) cannot guarantee compatibility across hardware versions. |
| [SIMD API](SIMD-API/C-API/C-API.md) | [C API](SIMD-API/C-API/C-API.md) | Pure C APIs that expose the complete programming capabilities of the chip, support array memory allocation, are generally pointer-based, and provide a C-language programming experience consistent with the industry. |
| [SIMD API](SIMD-API/advanced_api/advanced_api_list.md) | [Advanced API](SIMD-API/advanced_api/advanced_apis.md) | Implements some commonly used computation algorithms to improve programming and development efficiency, and typically invokes multiple basic APIs. Advanced APIs include the math library, Matmul, Softmax, and other APIs. Advanced APIs guarantee compatibility. |
| [SIMT API](SIMT-API/overview.md) | - | Benchmarked against the industry, it provides single-instruction multiple-thread APIs that implement parallel computation in the form of multiple threads per instruction. SIMT programming is mainly used for vector computation and is especially suitable for scenarios such as discrete access and complex control logic. The SIMT API supports two programming models: SIMT programming and SIMD-SIMT hybrid programming. For the specific supported APIs, see [SIMT Programming API List](SIMT-API/SIMT_programming_intro/api_list.md) and [SIMD-SIMT Hybrid Programming API List](SIMT-API/SIMD_SIMT_hybrid_programming_intro/api_list_148.md), respectively. |
| [AI CPU API](AI-CPU-API/AI_CPU_API_list.md) | - | Usually serves as a supplement to the above APIs, mainly handling non-matrix, branch-intensive computations with relatively complex logic. |
| [Utils API](Utils-API/utils_api_list.md) | - | Rich general-purpose utility classes covering the standard library (currently supporting only SIMD), platform information retrieval, runtime compilation, and log output, supporting developers in efficiently implementing operator development and performance optimization. |

## Header Files and Library Files Required for Calling APIs

After installing the firmware, driver, and CANN software package, you can reference the header files and library files of Ascend C APIs only after installing, compiling, and running applications.

You can include the required files based on the Ascend C APIs actually used. The purpose of each header file is described in the following table.

**Table 2** SIMT programming model header file list

| Header File | Description | Corresponding Library File |
| --- | --- | --- |
| simt/asc_simt.h | Provides the external APIs of the SIMT API. If the computation requires only a specific data type such as fp8, you can include only simt_api/asc_fp8.h. | Not applicable |

**For more details about the header files included in SIMT programming, see [SIMT-API](SIMT-API/overview.md).**

**Table 3** SIMD basic programming capability header file list

| API Capability Level | Development Scenario | Header File | Description | Corresponding Library File |
| --- | --- | --- | --- | --- |
| C language pointer-based programming | Develops any operator (including fused operators) | c_api/asc_simd.h | Provides the full set of external APIs of the SIMD C API. | Not applicable |
| C++ Tensor-based programming | Develops any operator (including fused operators) | tensor_api/tensor.h | Provides the full set of external APIs of the SIMD C++ Tensor API. | Not applicable |
| TPipe/TQue framework-based programming | Develops any operator (including fused operators) | basic_api/kernel_basic_intf.h | Provides the full set of external APIs of SIMD TPipe/TQue framework-based programming. | Not applicable |
| TPipe/TQue framework-based programming | Develops only vector operators | basic_api/kernel_vec_intf.h | Provides the external APIs of SIMD TPipe/TQue framework-based programming required for vector computation. | Not applicable |
| TPipe/TQue framework-based programming | Develops only matrix operators | basic_api/kernel_cube_intf.h | Provides the external APIs of SIMD TPipe/TQue framework-based programming required for matrix computation. | Not applicable |

**Table 4** SIMD high-level API header file list

| Header File | Description | Corresponding Library File |
| --- | --- | --- |
| adv_api/kernel_api.h | Provides the Device-side external APIs of the SIMD high-level API. It can be called only in the TPipe/TQue framework-based programming mode. | Not applicable |
| adv_api/tiling_api.h | Provides the Host-side external APIs of the SIMD high-level API. It can be called only in the TPipe/TQue framework-based programming mode. | libtiling_api.a |
