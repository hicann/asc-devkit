# disable_dma_atomic

<!-- md-trans-meta sourceCommit=53cacf821c4d3ab06652650874e352140527b080 translatedAt=2026-11-19T02:19:32.259Z pushedAt=2026-11-19T05:54:06.625Z nl=2 -->

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

Used together with [`set_atomic_add`](set_atomic_add.md), [`set_atomic_max`](set_atomic_max.md), and [`set_atomic_min`](set_atomic_min.md) to disable atomic operations after they are complete, avoiding affecting subsequent data copy.

## Prototype

```cpp
__aicore__ inline void disable_dma_atomic()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Example

The following example first enables DMA atomic accumulation, and after completing one UB-to-GM copy, calls this API to disable atomic mode, avoiding affecting subsequent copy.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr uint8_t buffer_lock_id = 0;

__global__ __vector__ void atomic_add_kernel(__gm__ float* output, __gm__ float* input)
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

    set_atomic_add<float>();
    mutex<pipe_t::PIPE_MTE3> copy_out_mutex(buffer_lock_id);
    copy_out_mutex.lock();
    copy(make_copy(copy_ub_to_gm{}), output_gm, input_ub);
    copy_out_mutex.unlock();
    disable_dma_atomic();
}
```
