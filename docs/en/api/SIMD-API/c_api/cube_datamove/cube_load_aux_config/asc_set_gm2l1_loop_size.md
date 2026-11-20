---
okftype: reference.api
description: "Sets the inner and outer loop counts for the loop fill mode of asc_copy_gm2l1_align. It must be used together with the inner and outer loop stride interfaces. After the data transfer is complete, it should be reset to 1. It takes effect only on AIC."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md
    kind: configures
    condition: "Used only in loop fill mode, and this mode does not support left and right padding"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop1_stride.md
    kind: related
    condition: "The inner loop stride must be used together with it"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop2_stride.md
    kind: related
    condition: "The outer loop stride must be used together with it"
version: ">=9.0.0"
version_changes: []
---
# asc_set_gm2l1_loop_size

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:27:41.446Z pushedAt=2026-11-18T08:15:34.322Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

When moving data from Global Memory (GM) to the L1 Buffer, sets the inner and outer loop counts of the loop fill mode of [asc_copy_gm2l1_align](../cube_compute_load/asc_copy_gm2l1_align.md).

It must be used together with [asc_set_gm2l1_loop1_stride](asc_set_gm2l1_loop1_stride.md) and [asc_set_gm2l1_loop2_stride](asc_set_gm2l1_loop2_stride.md).

Take the source operand move scenario as an example, as shown in the following figure.

![Source operand move scenario example](../../../../figures/source_operand_move_example.png)

This API takes effect only on the AIC.

## Prototype

```cpp
__aicore__ inline void asc_set_gm2l1_loop_size(uint64_t loop1_size, uint64_t loop2_size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| loop1_size | Input | Loop count of the inner loop. The value range is [0, $2^{21}-1$]. |
| loop2_size | Input | Loop count of the outer loop. The value range is [0, $2^{23}-1$]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- When this API is called on a non-AIC, it returns directly.
- The configuration is used only by the [asc_copy_gm2l1_align](../cube_compute_load/asc_copy_gm2l1_align.md) loop fill mode, which does not support left and right padding.

## Examples

After saving the code as `examples.asc`, you can compile it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 128;
constexpr int64_t CROSS_CORE_FLAG_ID = 0x8;
constexpr uint64_t AIV_TO_AIC_SYNC_ID = 0x1;
constexpr uint64_t AIC_TO_AIV_SYNC_ID = 0x2;

__global__ __mix__(1, 2) void LoopKernel(  // __mix__: AIC moves data to L1, and AIV writes the readback result back to GM.
    __gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __cbuf__ uint8_t* l1 = (__cbuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* staging_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* result_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(OUTPUT_BYTES);
    if ASC_IS_AIV {
        const bool is_output_core = asc_get_sub_block_id() == 0;
        if (is_output_core) {
            asc_copy_gm2ub(staging_ub, input, INPUT_BYTES);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_intra_arrive(PIPE_MTE3, AIV_TO_AIC_SYNC_ID);
        }
        asc_sync_block_arrive(PIPE_MTE3, CROSS_CORE_FLAG_ID);
        if (is_output_core) {
            asc_sync_intra_wait(PIPE_MTE3, AIC_TO_AIV_SYNC_ID);
            asc_copy_ub2gm(output, result_ub, OUTPUT_BYTES);
        }
    }
    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, AIV_TO_AIC_SYNC_ID);
        asc_sync_block_wait(PIPE_MTE1, CROSS_CORE_FLAG_ID);
        // 2×2 loop: source offsets are 0, 64, 128, and 192B, and destination L1 offsets are 0, 32, 64, and 96B.
        asc_set_gm2l1_loop_size(2, 2);
        asc_set_gm2l1_loop1_stride(64, 32);
        asc_set_gm2l1_loop2_stride(128, 64);
        asc_copy_gm2l1_align(
            l1, input, 1, 32, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 32);
        // Avoid the loop configuration affecting the next GM→L1 move.
        asc_set_gm2l1_loop_size(1, 1);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(result_ub, l1, 0, 1, OUTPUT_BYTES / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_intra_arrive(PIPE_MTE1, AIC_TO_AIV_SYNC_ID);
    }
    asc_sync();
}

bool IsGolden(const std::vector<uint8_t>& output)
{
    for (uint32_t block = 0; block < 4; ++block) {
        const uint32_t source = (block / 2) * 128 + (block % 2) * 64;
        for (uint32_t i = 0; i < 32; ++i) {
            if (output[block * 32 + i] != static_cast<uint8_t>(source + i)) return false;
        }
    }
    return true;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE) != ACL_SUCCESS) return 1;
    LoopKernel<<<1, 0>>>(output_device, input_device);
    const bool passed = aclrtSynchronizeDevice() == ACL_SUCCESS &&
        aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS && IsGolden(output);
    std::cout << "RESULT loop_2x2=" << (passed ? "PASS" : "FAIL") << std::endl;
    aclrtFree(output_device); aclrtFree(input_device); aclrtResetDevice(0); aclFinalize();
    return passed ? 0 : 1;
}
```
