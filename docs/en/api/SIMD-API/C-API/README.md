# C API

<!-- md-trans-meta sourceCommit=282ae070f9d90289ec519e0d57dbf32c7e9ac1e9 translatedAt=2026-09-08T05:22:38.978Z pushedAt=2026-09-08T13:17:42.546Z -->

The C API exposes the complete programming capabilities of the chip and supports the [method of allocating memory as an array](general_description_and_constraints.md#allocating-memory-as-an-array). It is generally based on pointer programming and provides a C language programming experience consistent with industry standards.

## Header Files to Include

> [!NOTE]
> Include the **asc_simd.h** file to call the corresponding C API APIs. Unless otherwise specified, including this header file meets the API calling requirements.
> If the API documentation contains special instructions, follow the specific instructions of the API.

```cpp
#include "c_api/asc_simd.h"
```

## API Types

- Computation of the first n data elements: This type of API uses a "tightly packed" data reading method during computation, that is, it sequentially and continuously obtains the required data starting from the initial position. For example, if N data elements need to be processed, they are obtained starting from position 0 of the source operand and continuing sequentially to position N-1.
- High-dimensional segmented computation: This type of API "skips part of the data" according to the configured rules. It is suitable for scenarios that require interval sampling and offers high flexibility, but additional related parameters need to be configured.
- Synchronous computation: This type of API automatically inserts synchronization operations internally, providing greater ease of use.

## Keywords

|Name|Description|
|-----------------------|-----------------------|
|\_\_gm\_\_| Storage space definition modifier, indicating that the modified variable is located in the Global Memory address space.|
|\_\_ubuf\_\_| Storage space definition modifier, indicating that the modified variable is located in the Unified Buffer address space.|
|\_\_aicore\_\_| Execution space qualifier, indicating that the function can be executed only on the AI Core.|
|\_\_cbuf\_\_| Storage space definition modifier, indicating that the modified variable is located in the L1 Buffer address space.|
|\_\_simd\_callee\_\_| Execution space qualifier, indicating that the function can be executed only on Reg.|

## Pipeline Type

The NPU contains different computation units. During computation, the computation data often needs to be moved to the computation units. The computation processes and data movement processes on different computation units can be divided into different pipelines. Operations executed on the AI Core are assigned to different pipelines (PIPEs) for execution, including the following:
- PIPE_S: Scalar pipeline, responsible for instruction dispatch and scalar computation.
- PIPE_V: Vector computation pipeline.
- PIPE_M: Cube computation pipeline.
- PIPE_MTE1: Transfer operation, including transfer operations from L1 Buffer to L0A Buffer or L0B Buffer, from L1 Buffer to UB, and initialization operations of L0A Buffer or L0B Buffer.
- PIPE_MTE2: Transfer operation, including transfer operations from GM to L1 Buffer, from GM to L0A Buffer or L0B Buffer, from GM to UB, and initialization operations of L1 Buffer.
- PIPE_MTE3: Transfer operation, including operations from UB to GM, from UB to L1 Buffer, and transfer operations from UB to UB.
- PIPE_FIX: Fixpipe pipeline.
- PIPE_ALL: all pipelines.

The [synchronization control](sync/synchronization_control.md) APIs can be used to control the execution order within the same pipeline and the execution order between different pipelines.

## API List

|      Directory      |
|-----------------------|
| [Data Structure](C-API.md#data-structure) |
| [Vector Computation](C-API.md#vector-computation) |
| [Data Transfer](C-API.md#data-transfer) |
| [Scalar Operations](C-API.md#scalar-operations) |
| [Matrix Computation](C-API.md#matrix-computation) |
| [Synchronization Control](C-API.md#synchronization-control) |
| [System Variables](C-API.md#system-variables) |
| [Cache Control](C-API.md#cache-control) |
| [Atomic Operations](C-API.md#atomic-operations) |
| [Other Operations](C-API.md#other-operations) |
| [Reg Data Transfer](C-API.md#reg-data-transfer) |
| [Reg Vector Computation](C-API.md#reg-vector-computation) |

## Example

For the C API samples of Atlas A3 training products/Atlas A3 inference products, see [C API Add sample](../../../../examples/02_simd_c_api/00_introduction/01_add).
For the C API samples of Ascend 950PR/Ascend 950DT, see [C API Add sample](../../../../examples/02_simd_c_api/00_introduction/04_reg_base_add_compute).
