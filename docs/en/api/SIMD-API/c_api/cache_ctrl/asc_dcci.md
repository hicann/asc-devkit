---
okftype: reference.api
description: "Maintains cache coherence between scalar access and Global Memory: writes back the dirty data modified in DCache to GM, and invalidates the cache line where the incoming address resides or the cache line of the specified target domain, so that subsequent accesses reload the latest data from GM."
tag:
  - roles: [control]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/scalar_compute/scalar_load/asc_load_dev.md
    kind: related
    condition: "Reading GM with this interface bypasses DCache, so no call to asc_dcci is required"
  - path: docs/zh/api/SIMD-API/c_api/scalar_compute/scalar_store/asc_store_dev.md
    kind: related
    condition: "Writing GM with this interface bypasses DCache, so no call to asc_dcci is required"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: constrained_by
    condition: "Whether scalar read/write of UB bypasses DCache is configured by CTRL[49], which defaults to 1'b0, meaning bypass"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md
    kind: related
    condition: "DMA transfers between UB and GM bypass DCache and do not cause the consistency issues described on this page"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm.md
    kind: related
    condition: "DMA transfers between UB and GM bypass DCache and do not cause the consistency issues described on this page"
  - path: docs/zh/guide/programming_guide/advanced_programming/memory_model/cache_coherence.md
    kind: related
    condition: "Source of the causes of DCache and GM cache coherence"
version: ">=9.0.0"
version_changes:
  - since: "9.0.0"
    kind: parameter_changed
    scope: "param:dst"
    scope_basis: resolved
    note: "In 9.0.0-beta.1, the prototypes of asc_dcci_entire_all/asc_dcci_entire_ub/asc_dcci_entire_out/asc_dcci_entire_atomic were (__gm__ void* dst). Starting from 9.0.0, they are changed to a parameterless form; asc_dcci_single and asc_ub_dcci_single still retain the dst parameter."
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.0.0, cache_ctrl.h reuses the guard macros from the atomic operation header file. When the include order is incorrect, the asc_dcci_* declarations are skipped. The CANN 9.1.0 Release Notes record that this issue has been fixed, effective from 9.1.0."
---
# asc_dcci

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:48:59.379Z pushedAt=2026-11-18T08:15:34.193Z nl=2 -->

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

The header file path is: `"c_api/cache_ctrl/cache_ctrl.h"`.

As shown in Figure 1, inside the AI Core, there are two ways to access Global Memory (GM) and Unified Buffer (UB):

- The DMA transfer unit reads and writes GM. Data is exchanged between Local Memory such as UB and GM through interfaces such as [asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm.md) and [asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md), bypassing the Data Cache (DCache), so there is no need to consider cache coherence.
- The Scalar unit accesses GM. If it goes through the DCache, it first accesses the DCache in each core, so there is a cache coherence issue between the DCache and GM. For the specific reason, see [Cache Coherence](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md).

**Figure 1**  DCache memory hierarchy diagram

![](../../../figures/dcci.png "DCache memory hierarchy diagram")

The asc_dcci interface is used to resolve the consistency issue described above. Its functionality can be divided into two parts:

- Clean: Writes back the modified data (dirty data) in the cache to GM to avoid data loss.
- Invalid:
    - Mark the cache lines in the specified address range as "invalid" so that they are removed from the cache.
    - Ensure that the next access to these memory addresses reloads data from GM instead of using possibly stale cached data.

Typical usage scenarios of this interface are as follows (using GM read/write as an example):

- When reading GM data that may have been modified externally by other cores, use the asc_dcci interface to access GM directly and obtain the latest data.
- When the user writes GM data through the Scalar unit and wants it to be written out immediately, the asc_dcci interface is also required.

> [!NOTE]Note
>
> - When the Scalar unit reads and writes GM, if the [asc_load_dev](../scalar_compute/scalar_load/asc_load_dev.md) and [asc_store_dev](../scalar_compute/scalar_store/asc_store_dev.md) interface pair is used, the read/write data bypasses the DCache, and no call to the asc_dcci interface is required.
> - Whether the Scalar unit bypasses the DCache when reading and writing UB is controlled by the CTRL[49] bit configured through the [asc_set_ctrl](../spr/asc_set_ctrl.md) interface. The default value of this bit is 1'b0, which means the DCache is bypassed by default. For details about CTRL[49], see the common CTRL register bit description in [asc_set_ctrl](../spr/asc_set_ctrl.md#ctrl_bit_desc).

## Prototype

```c
// Flush the single cache line where the passed GM address resides.
__aicore__ inline void asc_dcci_single(__gm__ void* dst)

// Flush the single cache line where the passed UB address resides.
__aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst)

// Flush all valid cache lines in the DCache according to the target domain. The target domain has the following four values:

// Flush all valid cache lines in the entire DCache.
__aicore__ inline void asc_dcci_entire_all()

// Flush all valid cache lines in the entire DCache that contain UB addresses.
__aicore__ inline void asc_dcci_entire_ub()

// Flush all valid cache lines in the entire DCache that contain GM addresses.
__aicore__ inline void asc_dcci_entire_out()

// Flush all valid cache lines in the entire DCache that contain GM addresses related to scalar atomic operations.
__aicore__ inline void asc_dcci_entire_atomic()
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Input | Address of the cache line to be cleaned and invalidated.<br>The hardware locates the target line by aligning to the cache line boundary. The passed address does not need to satisfy cache line alignment. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

The function prototype with `dst` of type `__ubuf__ void*` is supported only in AIV.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id9 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

```cpp
// This example verifies the writeback effect of asc_dcci_entire_out on GM dirty data. Scalar first writes 101 to two different cache lines,
// then calls asc_dcci_entire_out to write the dirty data in the DCache back to GM. The Host should read back 101.
#include <array>
#include <cstdint>
#include <iostream>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr int32_t GM_VALUE = 11;
constexpr int32_t DIRTY_VALUE = 101;
constexpr uint32_t SECOND_LINE_OFFSET = 32;
constexpr uint32_t DATA_COUNT = 64;

__global__ __vector__ void AscDcciEntireOutKernel(__gm__ int32_t* data)
{
    asc_init();
    volatile __gm__ int32_t* data_gm = data;
    // The two elements are 128B apart and located in different cache lines. A normal Scalar write to GM passes through DCache.
    data_gm[0] = DIRTY_VALUE;
    data_gm[SECOND_LINE_OFFSET] = DIRTY_VALUE;

    // Flush the cache lines containing the GM address in DCache, and wait for dirty data to be written back to GM.
    asc_dcci_entire_out();
    asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
}

bool CheckResult(const std::array<int32_t, DATA_COUNT>& output)
{
    return output[0] == DIRTY_VALUE && output[SECOND_LINE_OFFSET] == DIRTY_VALUE;
}
} // namespace

int main()
{
    std::array<int32_t, DATA_COUNT> input{};
    input.fill(GM_VALUE);
    aclInit(nullptr);
    aclrtSetDevice(0);

    int32_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(int32_t), input.data(), input.size() * sizeof(int32_t), ACL_MEMCPY_HOST_TO_DEVICE);
    AscDcciEntireOutKernel<<<1, 0>>>(input_device);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(input.data(), input.size() * sizeof(int32_t), input_device, input.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = ret == ACL_SUCCESS && CheckResult(input);
    std::cout << (passed ? "[Success] asc_dcci passed." : "[Failed] asc_dcci failed.") << std::endl;
    aclrtFree(input_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
