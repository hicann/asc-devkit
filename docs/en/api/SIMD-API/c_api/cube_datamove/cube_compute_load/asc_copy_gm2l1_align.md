---
okftype: reference.api
description: "Moves data from Global Memory to the L1 Buffer and supports data whose length is not an integer multiple of 32 bytes. Selects Normal or Compact Padding Mode via dst_stride. The unaligned portion can be padded manually or automatically, and Left/Right Padding and Cyclic Padding modes are supported."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [padding, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md
    kind: requires
    condition: "Source data must be arranged in fractal format before being moved in"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
    condition: "Address alignment and transfer granularity requirements"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop1_stride.md
    kind: requires
    condition: "Configure the inner loop stride when using Cyclic Padding mode"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop2_stride.md
    kind: requires
    condition: "Configure the outer loop stride when using Cyclic Padding mode"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop_size.md
    kind: requires
    condition: "Configure the loop count when using Cyclic Padding mode"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md
    kind: requires
    condition: "Configure the padding value first when enable_data_select is true or when using Left/Right Padding mode"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md
    kind: requires
    condition: "Value range and semantics of the l2_cache_mode parameter"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: requires
    condition: "Insert a synchronization instruction when there is a data dependency with other instructions"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_copy_gm2l1_align__PL1i8_PGi8_u32_u32_u8_u8_b_loadl2_u64_u32"
    scope_basis: resolved
    note: "9.2.0 adds an overload that configures L2 Cache using the asc_load_l2_cache_mode strongly-typed enumeration, with parameters burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select, src_stride, and dst_stride; the same form covers 10 dtypes in total: uint8_t, hifloat8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float, for 3510 only."
  - since: "9.2.0"
    kind: deprecated
    scope: "overload:asc_copy_gm2l1_align__PL1i8_PGi8_u32_u32_u8_u8_b_u8_u64_u32"
    scope_basis: resolved
    note: "The legacy parameter overload (the 7th parameter is the uint8_t l2_cache_ctl raw cache code) and the corresponding asc_copy_gm2l1_align_sync have been marked deprecated since 9.2.0 (header file ASC_DEPRECATED(9.2.0)), but the implementation is retained and can still be called; it is recommended to use the strongly-typed overload on this page together with asc_sync."
---
# asc_copy_gm2l1_align

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:27:12.148Z pushedAt=2026-11-18T08:15:34.249Z nl=2 -->

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

Moves data from Global Memory (GM) to the L1 Buffer. It supports moving data whose length is not an integer multiple of 32 bytes, and performs 32-byte alignment padding during the move. The moved data length must still meet the byte alignment requirement of the corresponding data type. For the alignment requirements of the source and destination start addresses, see [Parameters](#parameters).

If the moved data length is not 32-byte aligned, the moved data is padded to 32-byte alignment. You can select Normal mode or Compact mode by configuring the `dst_stride` parameter. The following two padding methods are supported in non-32-byte-aligned scenarios:

- Manual padding: Before the move, call [asc_set_gm2l1_pad](../cube_load_aux_config/asc_set_gm2l1_pad.md) to set the padding value, and set `enable_data_select` to `true`.
- Automatic padding: Set `enable_data_select` to `false`, and the hardware automatically pads dummy data. The value of the dummy data is the value of the first element of the data block.

The data move modes are described as follows:

- Compact mode: Set `dst_stride` to `burst_len`, and set `left_padding_count` and `right_padding_count` to 0. Multiple data blocks are tightly arranged in L1 and are treated as a whole in 32-byte padding. If the total length of all data blocks, that is, `burst_count * burst_len`, is not 32-byte aligned, padding is performed only at the end of the whole using the manual or automatic padding method described above. The amount of padding data is `32 - burst_count * burst_len % 32`, in bytes.
- Normal mode: Set `dst_stride` to a value that is not equal to `burst_len` and meets 32-byte alignment. The interval between the start addresses of adjacent destination data blocks is specified by `dst_stride`. Intervals can be reserved between data blocks, and tight arrangement is not required.
  - When both `left_padding_count` and `right_padding_count` are 0, each data block is padded to 32-byte alignment separately using the manual or automatic padding method described above. The amount of padding data for each data block is `32 - burst_len % 32`, in bytes.
  - When `left_padding_count` or `right_padding_count` is non-zero, it is the left/right padding mode. `enable_data_select` does not take effect, and `asc_set_gm2l1_pad` must be called before the copy to configure the padding value. If the total bytes of the data block length and the left/right padding are not 32-byte aligned, the hardware continues to pad dummy data on the right side to 32-byte alignment, and the value of the dummy data is the value configured by `asc_set_gm2l1_pad`.

For example, when copying 3 data blocks with a `burst_len` of 48 bytes:

- In Normal mode, set `dst_stride` to 64. The starting offsets of each data block in L1 are 0, 64, and 128 in sequence, and each data block is padded with 16 bytes of padding data respectively.

  **Figure 1** Normal padding mode schematic diagram

  ![Normal padding mode schematic diagram](../../../../figures/asc_copy_gm2l1_align_normal_padding.png "Normal padding mode schematic diagram")

- In Compact mode, set `dst_stride` to 48. The starting offsets of each data block in L1 are 0, 48, and 96 in sequence, and 16 bytes of padding data are padded only at the end of the 144 bytes of valid data.

  **Figure 2** Compact padding mode schematic diagram

  ![Compact padding mode schematic diagram](../../../../figures/asc_copy_gm2l1_align_compact_padding.png "Compact padding mode schematic diagram")

When only 1 data block is copied, or when `burst_len` is already 32-byte aligned and there is no left/right padding, the copy results of the two modes are the same.

The cyclic padding mode supports multiple cyclic calls of the Normal padding mode and the Compact padding mode, but does not support multiple cyclic calls of the left/right padding mode. It must be used together with [asc_set_gm2l1_loop_size](../cube_load_aux_config/asc_set_gm2l1_loop_size.md), [asc_set_gm2l1_loop1_stride](../cube_load_aux_config/asc_set_gm2l1_loop1_stride.md), and [asc_set_gm2l1_loop2_stride](../cube_load_aux_config/asc_set_gm2l1_loop2_stride.md). Through the above interfaces, configure the copy counts and source/destination strides of loop1/loop2.

This API is valid only when executed on the AIC.

For the data layout before moving in, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and move granularity requirements, see [Cube Compute Input Move Constraints](cube_compute_input_move_constraint.md), and use them in conjunction with the parameter and constraint descriptions of this API.

## Prototype

```c
// Placeholder form
__aicore__ inline void asc_copy_gm2l1_align(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_count,
                                            uint8_t right_padding_count,
                                            bool enable_data_select,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

### Data Types Supported by dtype

The value of `dtype` is one of the following: `int8_t`, `uint8_t`, `hifloat8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Prototype Example

```c
// Example: move of int8_t type
__aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int8_t* dst,
                                            __gm__ int8_t* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_count,
                                            uint8_t right_padding_count,
                                            bool enable_data_select,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand, stored in the L1 Buffer. The destination address must be 32-byte aligned. |
| src | Input | Source operand, stored in GM. The source address must be 1-byte aligned. |
| burst_count | Input | Number of consecutive data blocks to be moved. Value Range: [1, $2^{21}-1$]. |
| burst_len | Input | Length of each consecutive data block to be moved, Unit: Byte. Value Range: [1, $2^{21}-1$]. It must satisfy the dtype Byte Alignment: when dtype is `b16`, burst_len must be a multiple of 2; when dtype is `b32`, burst_len must be a multiple of 4. |
| left_padding_count | Input | Number of left padding elements. The padding byte count must satisfy `left_padding_count × sizeof(dtype) ≤ 32` bytes. The corresponding dtype upper limits are: when dtype is `b8`, Value Range: [0, 32]; when dtype is `b16`, Value Range: [0, 16]; when dtype is `b32`, Value Range: [0, 8]. |
| right_padding_count | Input | Number of right padding elements. The padding byte count must satisfy `right_padding_count × sizeof(dtype) ≤ 32` bytes. The corresponding dtype upper limits are: when dtype is `b8`, Value Range: [0, 32]; when dtype is `b16`, Value Range: [0, 16]; when dtype is `b32`, Value Range: [0, 8]. |
| enable_data_select | Input | Padding Data source selection.<br>&nbsp;&nbsp;&bull; `false`: first-element padding mode, where the Padding Data is taken from the first element of each burst;<br>&nbsp;&nbsp;&bull; `true`: constant padding mode, where the Padding Data is taken from the padding value preconfigured by [asc_set_gm2l1_pad](../cube_load_aux_config/asc_set_gm2l1_pad.md). When at least one of `left_padding_count` and `right_padding_count` is non-zero, the hardware forcibly uses the preconfigured constant padding value, and this parameter setting is invalid. |
| l2_cache_mode | Input | Enumeration value of type [asc_load_l2_cache_mode](../../defs/enum/asc_load_l2_cache_mode.md), which configures the management policy of the data in the L2 Cache. |
| src_stride | Input | Stride of adjacent consecutive data blocks on the source side (Global Memory), Unit: Byte. Value Range: [1, $2^{40}-1$]. |
| dst_stride | Input | Stride of adjacent consecutive data blocks on the destination side (L1 Buffer), Unit: Byte. Value Range: [1, $2^{21}-1$]. When `dst_stride` is not equal to `burst_len` (that is, when padding exists), `dst_stride` must be 32-byte aligned; Otherwise, an exception is triggered (even if `burst_count` is 1). |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- If this API is not called by AIC, it returns directly.
- The start address of `dst` must be 32-byte aligned (L1 Buffer alignment requirement). Otherwise, an exception is triggered.
- The start address of `src` must be 1-byte aligned (Global Memory alignment requirement). Otherwise, an exception is triggered.
- If this instruction has data dependency with other instructions, you need to insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the `dst` offset and the transfer size must not exceed the boundary. Otherwise, a destination address out-of-bounds exception is triggered.
- `burst_len`, `burst_count`, `src_stride`, `dst_stride`, `left_padding_count`, and `right_padding_count` must meet the value ranges in Parameter Description. Otherwise, the transfer result may not meet expectations.
- When `enable_data_select` is set to true, or in Left/Right Padding mode (when either `left_padding_count` or `right_padding_count` is not 0), you must first call [asc_set_gm2l1_pad](../cube_load_aux_config/asc_set_gm2l1_pad.md) to configure the padding value.

### Normal Padding Mode Constraints

- When `dst_stride` is not equal to `burst_len`, `dst_stride` must be 32-byte aligned. Otherwise, an exception is triggered. This constraint remains effective even when `burst_count` is 1.

### Left/Right Padding Mode Constraints

- In left/right padding mode, `left_padding_count×sizeof(dtype) + right_padding_count×sizeof(dtype) + burst_len + pad` must be 32-byte aligned, where `pad` is the portion padded to satisfy 32-byte alignment. The padding value is not controlled by `enable_data_select`, and you must first call [asc_set_gm2l1_pad](../cube_load_aux_config/asc_set_gm2l1_pad.md) to configure the padding value.

### Cyclic Padding Mode Constraints

- Cyclic padding mode cannot be enabled simultaneously with left/right padding mode.
- In cyclic padding mode, `loop1 dst stride` and `loop2 dst stride` must be 32-byte aligned. This constraint still applies even when the loop size is 1.

## Examples

The following example covers all three padding modes at once. All three scenarios move three 48-byte data blocks: in the Normal scenario, 16 bytes are padded at the tail of each block; in the Compact scenario, 16 bytes are padded only at the overall tail of the 144 bytes of valid data; in the left/right padding scenario, 4 bytes are padded on the left side and 12 bytes on the right side of each block. The example uses physical L1/UB addresses to pass results between AIC and AIV, and compares the results byte by byte on the Host side.

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t BURST_COUNT = 3;
constexpr uint32_t BURST_BYTES = 48;
constexpr uint32_t NORMAL_STRIDE = 64;
constexpr uint32_t INPUT_BYTES = BURST_COUNT * BURST_BYTES;
constexpr uint32_t NORMAL_OUTPUT_BYTES = BURST_COUNT * NORMAL_STRIDE;
constexpr uint32_t COMPACT_OUTPUT_BYTES = 160;
constexpr uint8_t PAD_BYTE = 0xA5;
constexpr uint8_t LEFT_PADDING = 4;
constexpr uint8_t RIGHT_PADDING = 12;
constexpr int64_t CROSS_CORE_FLAG_ID = 0x8;
constexpr uint64_t AIV_TO_AIC_SYNC_ID = 0x1;
constexpr uint64_t AIC_TO_AIV_SYNC_ID = 0x2;

enum class PaddingMode : uint32_t { NORMAL, COMPACT, LEFT_RIGHT };

__global__ __mix__(1, 2) void asc_copy_gm2l1_align_modes_kernel(  // __mix__: One AIC and two AIVs execute cooperatively.
    __gm__ uint8_t* output, __gm__ uint8_t* input, uint32_t mode)
{
    asc_init();
    // AIC writes L1 and AIV writes GM; cross-core shared buffers use physical addresses and cannot be mixed with array-form local buffers.
    __cbuf__ uint8_t* l1 = (__cbuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* staging_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* result_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(NORMAL_OUTPUT_BYTES);
    uint32_t output_bytes = NORMAL_OUTPUT_BYTES;

    if ASC_IS_AIV {
        const bool is_output_core = asc_get_sub_block_id() == 0;
        if (is_output_core) {
            // Initialize the AIV-side physical UB, and ensure its MTE2 operation completes before notifying AIC.
            asc_copy_gm2ub(staging_ub, input, INPUT_BYTES);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_intra_arrive(PIPE_MTE3, AIV_TO_AIC_SYNC_ID);
        }
        asc_sync_block_arrive(PIPE_MTE3, CROSS_CORE_FLAG_ID);
        if (is_output_core) {
            output_bytes = mode == static_cast<uint32_t>(PaddingMode::COMPACT) ?
                COMPACT_OUTPUT_BYTES : NORMAL_OUTPUT_BYTES;
            asc_sync_intra_wait(PIPE_MTE3, AIC_TO_AIV_SYNC_ID);
            asc_copy_ub2gm(output, result_ub, output_bytes);
        }
    }

    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, AIV_TO_AIC_SYNC_ID);
        asc_sync_block_wait(PIPE_MTE1, CROSS_CORE_FLAG_ID);
        uint8_t PAD_VALUE = 0xA5;
        asc_set_gm2l1_pad(*reinterpret_cast<uint8_t*>(&PAD_VALUE));
        if (mode == static_cast<uint32_t>(PaddingMode::NORMAL)) {
            // Normal: dst stride is 64B, and the tail of each 48B burst is padded with 16B using the first byte of that burst.
            asc_copy_gm2l1_align(l1, input, BURST_COUNT, BURST_BYTES, 0, 0, false,
                asc_load_l2_cache_mode::NOTALLOC_KEEP, BURST_BYTES, NORMAL_STRIDE);
        } else if (mode == static_cast<uint32_t>(PaddingMode::COMPACT)) {
            // Compact: dst stride equals 48B; only the tail of the overall 144B data is padded with 16B using 0xA5.
            output_bytes = COMPACT_OUTPUT_BYTES;
            asc_copy_gm2l1_align(l1, input, BURST_COUNT, BURST_BYTES, 0, 0, true,
                asc_load_l2_cache_mode::NOTALLOC_KEEP, BURST_BYTES, BURST_BYTES);
        } else {
            // Left/right padding: each burst consists of 4B left padding + 48B data + 12B right padding, forced to use 0xA5.
            asc_copy_gm2l1_align(l1, input, BURST_COUNT, BURST_BYTES, LEFT_PADDING, RIGHT_PADDING, false,
                asc_load_l2_cache_mode::NOTALLOC_KEEP, BURST_BYTES, NORMAL_STRIDE);
        }
        // GM to L1 is executed on MTE2, and L1 to UB is executed on MTE1; establish cross-pipeline dependencies through events.
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        // Read the L1 result back to another physical UB, and hand it over to AIV to write back to GM for host verification.
        asc_copy_l12ub(result_ub, l1, 0, 1, output_bytes / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_intra_arrive(PIPE_MTE1, AIC_TO_AIV_SYNC_ID);
    }
    asc_sync();
}

std::vector<uint8_t> MakeInput()
{
    std::vector<uint8_t> input(INPUT_BYTES);
    for (uint32_t burst = 0; burst < BURST_COUNT; ++burst) {
        for (uint32_t i = 0; i < BURST_BYTES; ++i) {
            input[burst * BURST_BYTES + i] = static_cast<uint8_t>(0x10 * (burst + 1) + i);
        }
    }
    return input;
}

std::vector<uint8_t> MakeGolden(const std::vector<uint8_t>& input, PaddingMode mode)
{
    const uint32_t output_bytes = mode == PaddingMode::COMPACT ? COMPACT_OUTPUT_BYTES : NORMAL_OUTPUT_BYTES;
    std::vector<uint8_t> golden(output_bytes, PAD_BYTE);
    for (uint32_t burst = 0; burst < BURST_COUNT; ++burst) {
        const uint32_t src_offset = burst * BURST_BYTES;
        if (mode == PaddingMode::NORMAL) {
            const uint32_t dst_offset = burst * NORMAL_STRIDE;
            std::memcpy(golden.data() + dst_offset, input.data() + src_offset, BURST_BYTES);
            std::memset(golden.data() + dst_offset + BURST_BYTES, input[src_offset], NORMAL_STRIDE - BURST_BYTES);
        } else if (mode == PaddingMode::COMPACT) {
            std::memcpy(golden.data() + src_offset, input.data() + src_offset, BURST_BYTES);
        } else {
            const uint32_t dst_offset = burst * NORMAL_STRIDE;
            std::memcpy(golden.data() + dst_offset + LEFT_PADDING, input.data() + src_offset, BURST_BYTES);
        }
    }
    return golden;
}

const char* ModeName(PaddingMode mode)
{
    return mode == PaddingMode::NORMAL ? "normal" :
        (mode == PaddingMode::COMPACT ? "compact" : "left_right");
}

bool RunMode(PaddingMode mode, const std::vector<uint8_t>& input,
    uint8_t* input_device, uint8_t* output_device)
{
    const std::vector<uint8_t> golden = MakeGolden(input, mode);
    std::vector<uint8_t> output(golden.size(), 0);
    asc_copy_gm2l1_align_modes_kernel<<<1, 0>>>(output_device, input_device, static_cast<uint32_t>(mode));
    if (aclrtSynchronizeDevice() != ACL_SUCCESS ||
        aclrtMemcpy(output.data(), output.size(), output_device, output.size(), ACL_MEMCPY_DEVICE_TO_HOST) != ACL_SUCCESS) {
        return false;
    }
    const bool passed = output == golden;
    std::cout << "RESULT mode=" << ModeName(mode) << " status=" << (passed ? "PASS" : "FAIL")
              << " output_bytes=" << output.size() << std::endl;
    return passed;
}
} // namespace

int main()
{
    const std::vector<uint8_t> input = MakeInput();
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&output_device), NORMAL_OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE) != ACL_SUCCESS) {
        return 1;
    }
    const bool passed = RunMode(PaddingMode::NORMAL, input, input_device, output_device) &&
        RunMode(PaddingMode::COMPACT, input, input_device, output_device) &&
        RunMode(PaddingMode::LEFT_RIGHT, input, input_device, output_device);
    std::cout << "SUMMARY total=3 passed=" << (passed ? 3 : 0) << std::endl;
    aclrtFree(output_device);
    aclrtFree(input_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
