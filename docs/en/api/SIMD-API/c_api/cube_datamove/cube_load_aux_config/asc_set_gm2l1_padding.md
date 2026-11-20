---
okftype: reference.api
description: "Configures the padding value for GM-to-L1 high-dimensional tiling data movement, supporting direct scalar input by dtype from int8_t to float or a 64-bit configuration register. This value is also used by the left-matrix 3D movement."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md
    kind: configures
    condition: "The padding mode of high-dimensional tiling movement uses the value set by this API"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_3d_arch_3510.md
    kind: related
    condition: "Because the register is shared, the value set by this API is also the padding value used by this 3D cube movement"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l12l0a_3d_padding.md
    kind: related
    condition: "The two APIs use the same register at the underlying level, and the values set by each overwrite the other"
version: ">=9.2.0"
version_changes: []
---
# asc_set_gm2l1_padding

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:32:55.732Z pushedAt=2026-11-18T08:15:34.332Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Used to set the padding value for the padding mode used by the [asc_copy_gm2l1 (NPU architecture version 3510, high-dimensional tiling data movement)](../cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) interface during data movement.

This interface takes effect only on AIC.

## Prototype

```c
__aicore__ inline void asc_set_gm2l1_padding(<dtype> padding_value)
__aicore__ inline void asc_set_gm2l1_padding(uint64_t config)
```

### Data Types Supported by dtype

The value range of `dtype` is: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Prototype Example

```c
// Example: configure the padding value of the half type.
__aicore__ inline void asc_set_gm2l1_padding(half padding_value)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| padding_value | Input | The padding value used during data movement. |
| config | Input | The padding value configuration register used during data movement.<br>&nbsp;&nbsp;&bull; If the padding value type is b32, the value represented by `[0:31]` is used as the padding value; if the padding value type is b16, the value represented by `[0:15]` is used as the padding value; if the padding value type is b8, `[15:8]` and `[7:0]` must have the same value, and the value represented by `[7:0]` is used as the padding value; if the padding value type is b4, `[15:12]`, `[11:8]`, `[7:4]`, and `[3:0]` must have the same value, and the value represented by `[3:0]` is used as the padding value;<br>&nbsp;&nbsp;&bull; Set the unused bits `[32:63]` to `0`. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- If this interface is not called on AIC, it returns directly.
- The register used by the current interface at the underlying layer is the same register as that of [asc_set_l12l0a_3d_padding](asc_set_l12l0a_3d_padding.md). Therefore, the value set here is also the padding value used by [asc_copy_l12l0a (NPU architecture version 3510, 3D cube movement)](../cube_compute_load/asc_copy_l12l0a_3d_arch_3510.md) during data movement.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

The following example moves 16B of input data to the L1 Buffer with `pad_func_mode=5`. It calls `asc_set_gm2l1_padding` to set the padding value to `0xA5`, then reads back the complete 32B L1 data block through L1→UB→GM, and verifies byte by byte on the host side that the first 16B are the input and the last 16B are the padding value.

```cpp
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t INPUT_BYTES = 16;
constexpr uint32_t OUTPUT_BYTES = 32;
constexpr uint8_t PAD_VALUE = 0xA5;
constexpr int64_t CROSS_CORE_FLAG_ID = 0x8;
constexpr uint64_t AIV_TO_AIC_SYNC_ID = 0x1;
constexpr uint64_t AIC_TO_AIV_SYNC_ID = 0x2;

__global__ __mix__(1, 2) void asc_set_gm2l1_padding_kernel(  // __mix__: AIC configures and reads back L1, AIV writes back to GM.
    __gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __cbuf__ uint8_t* l1 = (__cbuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* staging_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* result_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(OUTPUT_BYTES);

    if ASC_IS_AIV {
        const bool is_output_core = asc_get_sub_block_id() == 0;
        if (is_output_core) {
            // Initialize the AIV-side physical UB before AIC returns the L1 result.
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
        // EXPAND_16B groups by 16B valid input, and the padding value is configured by this API.
        asc_set_gm2l1_padding(PAD_VALUE);
        asc_copy_gm2l1(l1, input, 1, 1, asc_channel_pad_mode::EXPAND_16B, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(result_ub, l1, 0, 1, OUTPUT_BYTES / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_intra_arrive(PIPE_MTE1, AIC_TO_AIV_SYNC_ID);
    }
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS) return 1;
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_gm2l1_padding_kernel<<<1, 0>>>(output_device, input_device);
    const bool runtime_ok = aclrtSynchronizeDevice() == ACL_SUCCESS &&
        aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    std::vector<uint8_t> golden(OUTPUT_BYTES, PAD_VALUE);
    std::memcpy(golden.data(), input.data(), INPUT_BYTES);
    const bool passed = runtime_ok && output == golden;
    std::cout << "RESULT status=" << (passed ? "PASS" : "FAIL")
              << " input_bytes=" << INPUT_BYTES << " output_bytes=" << OUTPUT_BYTES
              << " pad_value=" << static_cast<uint32_t>(PAD_VALUE) << std::endl;
    aclrtFree(output_device);
    aclrtFree(input_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
