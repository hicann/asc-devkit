---
okftype: reference.api
description: "Performs synchronization control within the same pipeline. When PIPE_ALL is passed, it is equivalent to asc_sync. When another valid pipeline is passed, it blocks only subsequent instructions of that pipeline."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Functionally equivalent when PIPE_ALL is used"
version: ">=9.0.0"
version_changes: []
---
# asc_sync_pipe

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.559362+00:00 pushedAt=2026-11-16T02:10:57.808Z nl=2 -->

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

Completes synchronization control within the same pipeline, used to constrain the execution order within the same pipeline:

- When `PIPE_ALL` is passed, it waits for all preceding instructions in all pipelines to complete before releasing the block on subsequent instructions. In this case, its function is equivalent to [asc_sync](asc_sync.md).
- When another valid pipeline value is passed, it blocks only subsequent instructions in that pipeline until all preceding instructions in that pipeline are complete, without affecting instruction execution in other pipelines.

As shown in Figure 1, it ensures that only after all data reads and writes of the preceding instructions in the specified pipeline are complete can subsequent instructions in that pipeline begin execution.

**Figure 1**  Functional diagram of the `asc_sync_pipe` API    
![](../../../../figures/asc_sync_pipe.png)

## Prototype

```c
__aicore__ inline void asc_sync_pipe(pipe_t pipe)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| pipe | Input | Pipeline category to block. Supported values are `PIPE_V`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_FIX`, and `PIPE_ALL`; `PIPE_S` is not supported. If you do not care about the pipeline category and want to block all pipelines, pass `PIPE_ALL`.<br>The parameter type is the `pipe_t` enumeration. For the meaning of each enumeration value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Synchronization between Scalar pipelines is automatically guaranteed by the hardware. The `asc_sync_pipe` API does not support synchronization of the single `PIPE_S` pipeline.
<!-- npu="950" id9 -->
- For Ascend 950PR&950DT products, synchronization between `PIPE_V` is automatically guaranteed by the hardware.
<!-- end id9 -->
- `asc_sync_pipe(PIPE_ALL)` waits for all previously submitted APIs in all pipelines to complete, which impacts performance. If blocking only a single pipeline can solve the problem, avoid calling `asc_sync_pipe(PIPE_ALL)` arbitrarily.
- For `PIPE_MTE2`/`PIPE_MTE3`, developers need to insert synchronization when the destination addresses of the data movement operations overlap to ensure that the data movement instructions execute serially and prevent abnormal data from occurring.

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

__global__ __vector__ void asc_sync_pipe_kernel(__gm__ float* output, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS];
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);
    asc_sync_pipe(PIPE_ALL);
    add_vf(z, x, y);
    asc_sync_pipe(PIPE_ALL);
    asc_copy_ub2gm_align(output, z, BYTES);
    asc_sync_pipe(PIPE_ALL);
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
    asc_sync_pipe_kernel<<<1, 0>>>(output_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_sync_pipe passed." : "[Failed] asc_sync_pipe failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
