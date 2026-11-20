# set_atomic_max

<!-- md-trans-meta sourceCommit=53cacf821c4d3ab06652650874e352140527b080 translatedAt=2026-11-19T02:20:18.764Z pushedAt=2026-11-19T05:54:06.628Z nl=2 -->

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

Atomic fetch-and-max process: The data to be transferred to GM is compared with the existing data on GM, and the maximum value of the two is written to GM. This API enables atomic fetch-and-max for subsequent transfer instructions whose destination address is GM.

## Prototype

```cpp
template <typename T>
__aicore__ inline void set_atomic_max()
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type for atomic fetch-and-max. |

## Data Type

`T` supports `int8_t`, `int16_t`, `int32_t`, `half`, `bfloat16_t`, and `float`.

## Return Value

None

## Constraints

- The supported data path is UB/L0C Buffer->GM.
- Once enabled, it takes effect on all subsequent transfer instructions whose destination address is GM. It can be disabled by calling [`disable_dma_atomic`](disable_dma_atomic.md).
- This API does not automatically zero the existing data on GM before execution. If the original data on GM is expected to be zero before atomic fetch-and-max, it must be cleared manually.
- This API takes effect only on subsequent transfer instructions that are transferred through the MTE3 and Fixpipe units and whose destination address is GM. It does not take effect on scalar write-to-GM instructions.
- The synchronization between this API and the immediately following transfer instruction is ensured by hardware.
- The data type of the subsequent transfer instruction must be consistent with `T`.
- This API cannot ensure the execution order of subsequent transfer instructions.

## Example

In the following example, the MTE2/MTE3 mutex with the same ID is first used to ensure that the input transfer is complete. Then DMA atomic fetch-and-max is enabled to compute the input data with the initial value in GM and write the result back to Global Memory. Finally, the atomic mode is disabled.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr uint8_t buffer_lock_id = 0;

__global__ __vector__ void atomic_max_kernel(__gm__ float* output, __gm__ float* input)
{
    using namespace asc::te;
    __ubuf__ float input_buffer[total_length];

    auto layout = make_frame_layout<nd_ext_layout_ptn>(1, total_length);
    auto input_gm = make_tensor(make_mem_ptr<location::gm>(input), layout);
    auto output_gm = make_tensor(make_mem_ptr<location::gm>(output), layout);
    auto input_ub = make_tensor(make_mem_ptr(input_buffer), layout);

    mutex<pipe_t::PIPE_MTE2> copy_in_mutex(buffer_lock_id);
    copy_in_mutex.lock();
    copy(make_copy(copy_gm_to_ub{}), input_ub, input_gm);
    copy_in_mutex.unlock();

    set_atomic_max<float>();
    mutex<pipe_t::PIPE_MTE3> copy_out_mutex(buffer_lock_id);
    copy_out_mutex.lock();
    copy(make_copy(copy_ub_to_gm{}), output_gm, input_ub);
    copy_out_mutex.unlock();
    disable_dma_atomic();
}
```
