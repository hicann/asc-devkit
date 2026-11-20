---
okftype: reference.api
description: "A no-parameter cache maintenance API used to flush the NDDMA DataCache to prevent multiple cores from hitting stale data when reading the same GM address. It should be called before asc_ndim_copy_gm2ub, takes effect only on AI Vector, and returns directly when called from non-AI Vector."
tag:
  - roles: [control]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md
    kind: requires
    condition: "When multiple cores may read the same GM address and the data may be updated by other cores, call this movement API after this API"
version: ">=9.1.0"
version_changes: []
---
# asc_ndim_copy_dci

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.109000+00:00 pushedAt=2026-11-16T02:10:59.531Z nl=2 -->

## Applicable Products

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

## Description

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

Flushes the NDDMA DataCache. In multi-core scenarios, when multiple AI Cores read the same GM (Global Memory) address, stale data may be cached in the NDDMA DataCache, causing some cores to read incorrect data. Therefore, before using [asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md), call this API to flush the NDDMA DataCache and ensure that the cached data is up to date.

This API takes effect only on AIV.

## Prototype

```cpp
__aicore__ inline void asc_ndim_copy_dci()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- Calling this API on a non-AIV returns directly.
- This API is called only before using [asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md) to flush the NDDMA DataCache.
- In multi-core scenarios, if multiple AI Cores may read the same GM address and the data at that GM address may be updated by other cores, call this API before calling [asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md) to ensure data consistency among multiple cores.

## Examples

2D Padding scenario, using the fixed-value padding method
![ ](../../../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png)

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 12;
constexpr uint32_t OUTPUT_COLUMNS = 6;
constexpr uint32_t OUTPUT_BYTES = OUTPUT_COLUMNS * 9;
constexpr uint8_t PAD_VALUE = 88;

__global__ __vector__ void asc_ndim_copy_dci_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[64];
    // Each input row contains 4 bytes. Padding 2 bytes on the left and 3 bytes on the right extends each output row to 9 bytes.
    asc_set_ndim_loop0_stride(1, 1);
    asc_set_ndim_loop1_stride(6, 3);
    asc_set_ndim_loop2_stride(0, 0);
    asc_set_ndim_loop3_stride(0, 0);
    asc_set_ndim_loop4_stride(0, 0);
    asc_set_ndim_pad_value(PAD_VALUE);
    asc_ndim_pad_count_config config;
    config.loop1_lp_count = 2;
    config.loop1_rp_count = 3;
    asc_set_ndim_pad_count(config);
    asc_ndim_copy_gm2ub(local, input, 3, 4, 1, 1, 1, 2, 1, true,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    // PIPE_MTE3 waits for the PIPE_MTE2 load to complete before copying UB data to GM.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, local, OUTPUT_BYTES);
    asc_sync();
}

void print_data(const char* name, const std::vector<uint8_t>& data)
{
    std::cout << name << ":";
    const uint32_t count = data.size() < 32 ? data.size() : 32;
    for (uint32_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, PAD_VALUE);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i);
    for (uint32_t row = 0; row < 4; ++row) {
        for (uint32_t column = 0; column < 3; ++column) {
            golden[(row + 2) * OUTPUT_COLUMNS + column + 2] = input[row * 3 + column];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_ndim_copy_dci_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_ndim_copy_dci passed." : "[Failed] asc_ndim_copy_dci failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
