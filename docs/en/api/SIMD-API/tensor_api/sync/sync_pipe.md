# sync_pipe

<!-- md-trans-meta sourceCommit=53cacf821c4d3ab06652650874e352140527b080 translatedAt=2026-11-19T03:22:37.987Z pushedAt=2026-11-19T05:54:06.700Z nl=2 -->

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

Completes synchronization control within the same pipeline to constrain the execution order of a single pipeline or all pipelines. When `pipe_t::PIPE_ALL` is passed in, the subsequent instructions are unblocked only after the preceding instructions of all pipelines are completed. When another valid pipeline value is passed in, only the subsequent instructions of that pipeline are blocked until the preceding instructions of that pipeline are completed, without affecting the instruction execution of other pipelines.

## Prototype

```cpp
__aicore__ inline void sync_pipe(pipe_t pipe)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| pipe | Input | Pipeline that needs to be synchronized, of the type `pipe_t`. The supported values are `pipe_t::PIPE_V`, `pipe_t::PIPE_M`, `pipe_t::PIPE_MTE1`, `pipe_t::PIPE_MTE2`, `pipe_t::PIPE_MTE3`, `pipe_t::PIPE_FIX`, and `pipe_t::PIPE_ALL`. `pipe_t::PIPE_S` is not supported. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Synchronization between Scalar pipelines is automatically guaranteed by hardware. This API does not support `PIPE_S`.
- Synchronization between `PIPE_V` is automatically guaranteed by hardware.
- `sync_pipe(pipe_t::PIPE_ALL)` waits for the completion of all previously submitted instructions in all pipelines, which may affect performance. If only a single pipeline needs to be waited for, pass in the corresponding pipeline value.
- When the destination addresses of `pipe_t::PIPE_MTE2` and `pipe_t::PIPE_MTE3` overlap, you need to insert synchronization to ensure that multiple data movement instructions are executed serially.

## Example

In the following example, the GM-to-UB data movement and UB-to-GM data movement are first completed through MTE2/MTE3 mutexes with the same ID, and finally sync_pipe is called to wait for the completion of the preceding instructions in all pipelines.

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr uint8_t buffer_lock_id = 0;

__global__ __vector__ void sync_pipe_kernel(__gm__ float* src, __gm__ float* dst)
{
    using namespace asc::te;
    __ubuf__ float buffer[total_length];

    auto layout = make_layout(make_shape(total_length));
    auto src_gm = make_tensor(make_mem_ptr<location::gm>(src), layout);
    auto dst_gm = make_tensor(make_mem_ptr<location::gm>(dst), layout);
    auto ub_tensor = make_tensor(make_mem_ptr(buffer), layout);

    mutex<pipe_t::PIPE_MTE2> copy_in_mutex(buffer_lock_id);
    copy_in_mutex.lock();
    copy(make_copy(copy_gm_to_ub{}), ub_tensor, src_gm);
    copy_in_mutex.unlock();

    mutex<pipe_t::PIPE_MTE3> copy_out_mutex(buffer_lock_id);
    copy_out_mutex.lock();
    copy(make_copy(copy_ub_to_gm{}), dst_gm, ub_tensor);
    copy_out_mutex.unlock();
    sync_pipe(pipe_t::PIPE_ALL);
}
```
