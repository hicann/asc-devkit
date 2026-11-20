# block, intra, inter, and subblock Synchronization

<!-- md-trans-meta sourceCommit=53cacf821c4d3ab06652650874e352140527b080 translatedAt=2026-11-19T03:19:41.354Z pushedAt=2026-11-19T05:54:06.697Z nl=2 -->

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

This group of APIs provides four types of arrive/wait synchronization capabilities:

- block synchronization: implements synchronization between the AIC and all AIVs within a single AI Core.
- intra synchronization: implements synchronization between the AIC and a single AIV within a single AI Core.
- inter synchronization: implements synchronization between different AI Cores.
- subblock synchronization: implements synchronization between subblocks (slave cores within a group) within a single AI Core.

Each type of synchronization provides two APIs: `arrive` and `wait`.

## Prototype

```cpp
__aicore__ inline void sync_block_arrive(pipe_t pipe, int64_t flag_id)
__aicore__ inline void sync_block_wait(pipe_t pipe, int64_t flag_id)

__aicore__ inline void sync_intra_arrive(pipe_t pipe, uint64_t sync_id)
__aicore__ inline void sync_intra_wait(pipe_t pipe, uint64_t sync_id)

__aicore__ inline void sync_inter_arrive(pipe_t pipe, int64_t flag_id)
__aicore__ inline void sync_inter_wait(pipe_t pipe, int64_t flag_id)

__aicore__ inline void sync_subblock_arrive(pipe_t pipe, int64_t flag_id)
__aicore__ inline void sync_subblock_wait(pipe_t pipe, int64_t flag_id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| pipe | Input | Pipeline associated with the synchronization, of type `pipe_t`. |
| flag_id | Input | block, inter, or subblock synchronization flag, used to identify the same group of synchronization signals. The value range is `[0, 15]`, and each `flag_id` has its own independent 4-bit counter. |
| sync_id | Input | intra synchronization flag, used to identify the same group of synchronization signals. Each `sync_id` has its own independent 4-bit counter. The AIC side has 32 `sync_id` values (0 to 31), and each AIV side has 16 `sync_id` values (0 to 15). |

## Return Value

None

## Constraints

- When `sync_block_arrive` and `sync_block_wait`, `sync_inter_arrive` and `sync_inter_wait`, or `sync_subblock_arrive` and `sync_subblock_wait` are used in pairs, the `flag_id` values must be exactly the same; otherwise, undefined behavior may occur.
- When `sync_intra_arrive` and `sync_intra_wait` are used in pairs, the two APIs are not required to pass the same `sync_id`. Instead, they must conform to the following cross-core ID mapping relationship; otherwise, undefined behavior may occur:
    - In AIV0, the `sync_id` value of `sync_intra_arrive` ranges from `0` to `15`, corresponding respectively to the `sync_id` values `0` to `15` of `sync_intra_wait` in the AIC.
    - In AIV1, the `sync_id` value of `sync_intra_arrive` ranges from `0` to `15`, corresponding respectively to the `sync_id` values `16` to `31` of `sync_intra_wait` in the AIC.
    - In the AIC, the `sync_id` value of `sync_intra_arrive` ranges from `0` to `15`, corresponding respectively to the `sync_id` values `0` to `15` of `sync_intra_wait` in AIV0.
    - In the AIC, the `sync_id` value of `sync_intra_arrive` ranges from `16` to `31`, corresponding respectively to the `sync_id` values `0` to `15` of `sync_intra_wait` in AIV1.
- The kernel function cannot use the `__cube__` or `__vector__` function execution space qualifiers; otherwise, the hardware does not enable the scheduling module and synchronization cannot be performed normally. The function execution space qualifiers supported by each synchronous API are as follows:

    | Synchronous API | Supported function execution space qualifiers |
    | :--- | :--- |
    | `sync_block_arrive` / `sync_block_wait` | `__mix__(1, 1)`, `__mix__(1, 2)` |
    | `sync_intra_arrive` / `sync_intra_wait` | `__mix__(1, 2)` |
    | `sync_inter_arrive` / `sync_inter_wait` | `__mix__(0, 1)`, `__mix__(1, 0)`, `__mix__(1, 1)`, `__mix__(1, 2)` |
    | `sync_subblock_arrive` / `sync_subblock_wait` | `__mix__(1, 1)`, `__mix__(1, 2)` |

- The `pipe` values supported by each API are as follows:

    | Synchronous API | `pipe` supported by the AIC | `pipe` supported by the AIV |
    | :--- | :--- | :--- |
    | `sync_block_arrive` / `sync_inter_arrive` | `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` | `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    | `sync_block_wait` / `sync_inter_wait` | `PIPE_S`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` | `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    | `sync_intra_arrive` / `sync_intra_wait` | `PIPE_S`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` | `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    | `sync_subblock_arrive` | Not supported | `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    | `sync_subblock_wait` | Not supported | `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |

- `sync_subblock_arrive` and `sync_subblock_wait` support only AIV calls.
- The `arrive` API does not block subsequent instructions in the `pipe` pipeline; the `wait` API blocks subsequent instructions in the `pipe` pipeline until the corresponding counter is non-zero. When the counter is non-zero, the `wait` API releases the block and decrements the counter by 1.
- For `sync_block_arrive`, `sync_block_wait`, `sync_inter_arrive`, `sync_inter_wait`, `sync_subblock_arrive`, and `sync_subblock_wait`, the `flag_id` value range is `[0, 15]`, and values out of range are truncated to the lower 4 bits according to the bit width.
- For `sync_intra_arrive` and `sync_intra_wait`, the `sync_id` value range of the AIC is `[0, 31]`, and values out of range are truncated to the lower 5 bits according to the bit width; the `sync_id` value range of the AIV is `[0, 15]`, and values out of range are truncated to the lower 4 bits according to the bit width.
- Each counter can accumulate up to 15 times consecutively. The counter value must not exceed 15; otherwise, an exception is triggered.

## Example

The following example uses block synchronization in a `__mix__(1, 2)` kernel function: the AIV fills the UB and writes the result back to Global Memory, then calls `sync_block_arrive` to notify the AIC; the AIC calls `sync_block_wait` to wait for the notification and then writes to Global Memory.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr int64_t sync_flag_id = 0;

__global__ __mix__(1, 2) void block_sync_kernel(__gm__ float* output)
{
    using namespace asc::te;
    init_soc_state();
    __ubuf__ float data_buffer[total_length];

    auto layout = make_layout(make_shape(total_length));
    auto data_ub = make_tensor(make_mem_ptr(data_buffer), layout);
    auto output_gm = make_tensor(make_mem_ptr<location::gm>(output), layout);

    if ASCEND_IS_AIV {
        for (uint32_t i = 0; i < total_length; ++i) {
            data_buffer[i] = 1.0f;
        }
        copy(make_copy(copy_ub_to_gm{}), output_gm, data_ub);
        sync_block_arrive(pipe_t::PIPE_MTE3, sync_flag_id);
    }

    if ASCEND_IS_AIC {
        sync_block_wait(pipe_t::PIPE_S, sync_flag_id);
        output[0] = 1.0f;
    }
    sync_pipe(pipe_t::PIPE_ALL);
}
```
