# asc_sync_notify

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T01:40:03.555367+00:00 pushedAt=2026-11-16T02:10:57.799Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

## Description

The header file path is `"c_api/sync/sync.h"`.

As shown in Figure 1, it is used in pairs with [asc_sync_wait](asc_sync_wait.md) to implement synchronization control between different pipelines within the AI Core. The functions of `asc_sync_notify` and `asc_sync_wait` are as follows:

- `asc_sync_notify`: After all read and write operations of the preceding instructions in the source pipeline are complete, the current instruction begins execution and sets the corresponding flag bit in the hardware to 1. `asc_sync_notify` only sets the corresponding flag bit in the hardware and does not block the next instruction in the source pipeline.
- `asc_sync_wait`: When the destination pipeline reaches this instruction, if the corresponding flag bit in the hardware is 0, subsequent instructions in the destination pipeline remain blocked; if the corresponding flag bit in the hardware is 1, the flag bit is set to 0 and subsequent instructions in the destination pipeline begin execution.

**Figure 1**  Functional diagram of the `asc_sync_notify` and `asc_sync_wait` APIs    
![](../../../../figures/asc_sync_notify.png)

## Prototype

```c
__aicore__ inline void asc_sync_notify(pipe_t pipe,
                                       pipe_t tpipe,
                                       event_t id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pipe | Input | Source pipeline type, that is, "which pipeline's preceding instructions to wait for".<br>The parameter type is the `pipe_t` enumeration. For the meaning of each enumeration value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type). |
| tpipe | Input | Destination pipeline type, that is, "which pipeline's `asc_sync_wait` instruction to unblock". Valid values are the same as those of `pipe` and are defined by the `pipe_t` enumeration. |
| id | Input | Synchronization event ID. Each `pipe` and `tpipe` combination has 8 independent synchronization event IDs. Valid values are defined by the `event_t` enumeration. |

The `event_t` enumeration is defined as follows: <a id="event_t_enum"></a>

```c
typedef enum {
    EVENT_ID0 = 0,
    EVENT_ID1 = 1,
    EVENT_ID2 = 2,
    EVENT_ID3 = 3,
    EVENT_ID4 = 4,
    EVENT_ID5 = 5,
    EVENT_ID6 = 6,
    EVENT_ID7 = 7
} event_t;
```

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- **`pipe` and `tpipe` cannot be combined arbitrarily; their combinations are restricted**: The combinations supported in AIC and AIV differ by product. For details, see [Intra-Core Synchronization Combinations](../overview.md#intra-core-synchronization-combinations), specifically [Table 2](../overview.md#aic_intra_core_sync_combinations) and [Table 3](../overview.md#aiv_intra_core_sync_combinations).
- Using `asc_sync_notify` consecutively with the same source pipeline, same destination pipeline, and same `id` causes undefined behavior.
- This API must be used in pairs with `asc_sync_wait`, and the `pipe`, `tpipe`, and `id` parameters of the paired calls must be exactly the same.
- Neither `pipe` nor `tpipe` can be `PIPE_ALL`; otherwise, an exception is triggered.
- Each `pipe` and `tpipe` combination has 8 independent synchronization event IDs. For example, the `PIPE_M` and `PIPE_V` combination and the `PIPE_V` and `PIPE_MTE3` combination can use the same `id` value simultaneously without interfering with each other.
- This API does not block subsequent instructions in either the `pipe` or `tpipe` pipeline.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(float);

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENTS;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

void print_data(const char* label, const std::vector<float>& data)
{
    std::cout << label << ":";
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}

__global__ __vector__ void asc_sync_notify_kernel(__gm__ float* output, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS];
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);
    // The computation reads the loaded data, so it must wait for MTE2 to complete to prevent PIPE_V from reading incomplete data.
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    add_vf(z, x, y);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, z, BYTES);
    asc_sync();
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS), src1(ELEMENTS), output(ELEMENTS, 0.0f), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS - i) * 0.5f;
        golden[i] = src0[i] + src1[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, BYTES, src0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, BYTES, src1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_notify_kernel<<<1, 0>>>(output_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_sync_notify passed." : "[Failed] asc_sync_notify failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
