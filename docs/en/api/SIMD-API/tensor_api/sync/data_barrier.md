# sync_data_barrier

<!-- md-trans-meta sourceCommit=53cacf821c4d3ab06652650874e352140527b080 translatedAt=2026-11-19T03:20:25.336Z pushedAt=2026-11-19T05:54:06.696Z nl=2 -->

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

Blocks the execution of subsequent instructions until the previously issued but not yet completed memory access instructions complete. You can specify the memory scope to wait for through a parameter.

## Prototype

```cpp
__aicore__ inline void sync_data_barrier(mem_dsb_t scope)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| scope | Input | Memory scope to wait for, of type `mem_dsb_t`. The value is: <br>• `DSB_ALL`: Wait for GM and UB access instructions. <br>• `DSB_DDR`: Wait for GM access instructions. <br>• `DSB_UB`: Wait for UB access instructions. |

## Return Value

None

## Constraints

- The input parameter of type `mem_dsb_t` must be passed directly as an enumeration value literal.
- When the memory scope to wait for is `DSB_ALL`, only GM and Unified Buffer (UB) access instructions are waited for, excluding other memory locations such as L0C Buffer and L1 Buffer.

## Example

In the following example, the first GM data is written first, then this API is called to wait for the write to complete, and then the second GM data is written, ensuring the issue order of the two writes.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr int32_t first_value = 1;
constexpr int32_t second_value = 2;

__global__ __vector__ void data_barrier_kernel(__gm__ int32_t* output)
{
    using namespace asc::te;
    output[0] = first_value;
    sync_data_barrier(mem_dsb_t::DSB_DDR);
    output[1] = second_value;
}
```
