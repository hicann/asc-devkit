---
okftype: reference.api
description: "Deprecated synchronous unaligned copy interface that pads non-32-byte-aligned data during GM-to-L1 Buffer copy and pads it in Normal, Compact, or left-right mode. The replacement is asc_copy_gm2l1_align used together with asc_sync."
tag:
  - roles: [data_movement]
  - operations: [copy, synchronization]
  - traits: [padding, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md
    kind: constrained_by
    condition: "When data_select_bit is true or the left-right padding mode is used, the padding value must be configured first"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous non-aligned copy interface is deprecated. Use asc_copy_gm2l1_align with asc_load_l2_cache_mode and additionally call asc_sync; all 10 declarations in include/c_api/composite/cube_datamove_composite.h carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table lists the corresponding _sync overloads."
---
# asc_copy_gm2l1_align_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:28:24.078Z pushedAt=2026-11-18T03:12:16.384Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_gm2l1_align_sync` interface is deprecated. Use the [corresponding asynchronous interface](../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md) and the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from Global Memory to the L1 Buffer, providing non-32B-aligned data movement capability and supporting data padding during the movement.

If the length of the data to be moved is not 32-byte aligned, the moved data is padded to 32-byte alignment. You can select Normal mode or Compact mode by configuring the parameter `burst_dst_stride`. The following two padding modes are supported in non-32-byte-aligned scenarios:

- Manual padding: Before the movement, call [asc_set_gm2l1_pad](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md) to set the padding value, and set `data_select_bit` to `true`.
- Automatic padding: Set `data_select_bit` to `false`, and the hardware automatically pads dummy data. The value of the dummy data is the value of the first element of the data block.

The data movement modes are described as follows:

- Compact mode: Set `burst_dst_stride` to `len_burst`, and set `left_padding_count` and `right_padding_count` to 0. Multiple data blocks are tightly arranged in L1 and are treated as a whole for 32-byte padding. If the total length of all data blocks, that is, `n_burst *len_burst`, is not 32-byte aligned, padding is performed only at the end of the whole to 32-byte alignment through the manual or automatic padding method described above. The amount of padding data is `32 - n_burst * len_burst % 32`, in bytes.
- Normal mode: Set `burst_dst_stride` to a value that is not equal to `len_burst` and satisfies 32-byte alignment. The interval between the start addresses of adjacent destination data blocks is specified by `burst_dst_stride`. Intervals can be reserved between data blocks, and the data blocks do not need to be tightly arranged.
  - When both `left_padding_count` and `right_padding_count` are 0, each data block is padded to 32-byte alignment through the manual or automatic padding method described above. The amount of padding data for each data block is `32 - len_burst % 32`, in bytes.
  - When `left_padding_count` or `right_padding_count` is non-zero, it is the left-right padding mode. `data_select_bit` does not take effect, and `asc_set_gm2l1_pad` must be called to configure the padding value before the copy. If the data block length plus the total bytes of left and right padding is not 32-byte aligned, the hardware continues to pad dummy data on the right to 32-byte alignment, and the value of the dummy data is the value configured by `asc_set_gm2l1_pad`.

For example, when copying 3 data blocks with `len_burst` of 48 bytes:

- In Normal mode, set `burst_dst_stride` to 64. The starting offsets of the data blocks in L1 are 0, 64, and 128 in sequence, and each data block is padded with 16 bytes of padding data.

  **Figure 1** Normal padding mode schematic diagram

  ![Normal padding mode schematic diagram](../../../figures/asc_copy_gm2l1_align_normal_padding.png "Normal padding mode schematic diagram")

- In Compact mode, set `burst_dst_stride` to 48. The starting offsets of the data blocks in L1 are 0, 48, and 96 in sequence, and 16 bytes of padding data are padded only at the end of the 144 bytes of valid data.

  **Figure 2** Compact padding mode schematic diagram

  ![Compact padding mode schematic diagram](../../../figures/asc_copy_gm2l1_align_compact_padding.png "Compact padding mode schematic diagram")

When only 1 data block is copied, or `len_burst` is already 32-byte aligned and there is no left-right padding, the copy results of the two modes are the same.

The circular padding mode supports multiple loop calls of the Normal padding mode and the Compact padding mode, but does not support multiple loop calls of the left-right padding mode. It must be used together with [asc_set_gm2l1_loop_size](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop_size.md), [asc_set_gm2l1_loop1_stride](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop1_stride.md), and [asc_set_gm2l1_loop2_stride](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop2_stride.md). Configure the transfer count and source/destination stride of loop1/loop2 through the preceding APIs.

This API takes effect only when executed on the AIC.

## Prototype

```c
// Placeholder form
__aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ <dtype>* dst,
                                                 __gm__ <dtype>* src,
                                                 uint32_t n_burst,
                                                 uint32_t len_burst,
                                                 uint8_t left_padding_count,
                                                 uint8_t right_padding_count,
                                                 bool data_select_bit,
                                                 uint8_t l2_cache_ctl,
                                                 uint64_t burst_src_stride,
                                                 uint32_t burst_dst_stride)
```

### Data Types Supported by dtype

The value of `dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand, stored in the L1 Buffer. The destination address must be 32-byte aligned. |
| src | Input | Source operand, stored in GM. The source address must be 1-byte aligned. |
| n_burst | Input | Number of consecutive data blocks to be transferred. Value range: [1, $2^{21}-1$]. |
| len_burst | Input | Length of each consecutive data block to be transferred, in bytes. Value range: [1, $2^{21}-1$]. It must satisfy the dtype byte alignment: when dtype is of the `b16` type, len_burst must be a multiple of 2; when dtype is `b32`, len_burst must be a multiple of 4. |
| left_padding_count | Input | Number of left padding elements. The padding byte count must satisfy `left_padding_count × sizeof(dtype) ≤ 32` bytes. The corresponding dtype upper limits are: when dtype is `b8`, value range: [0, 32]; when dtype is `b16`, value range: [0, 16]; when dtype is `b32`, value range: [0, 8]. |
| right_padding_count | Input | Number of right padding elements. The padding byte count must satisfy `right_padding_count × sizeof(dtype) ≤ 32` bytes. The corresponding dtype upper limits are: when dtype is `b8`, value range: [0, 32]; when dtype is `b16`, value range: [0, 16]; when dtype is `b32`, value range: [0, 8]. |
| data_select_bit | Input | Padding data source selection.<br>&nbsp;&nbsp;&bull; `false`: first-element padding mode, where the padding data is taken from the first element of each burst;<br>&nbsp;&nbsp;&bull; `true`: constant padding mode, where the padding data is taken from the padding value preconfigured by [asc_set_gm2l1_pad](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md). When at least one of `left_padding_count` and `right_padding_count` is non-zero, the hardware forcibly uses the preconfigured constant padding value, and this parameter is invalid. |
| l2_cache_ctl | Input | Configures the data management policy in the L2 Cache. For the value description, see [Table 2](#l2_cache_ctl_values). |
| burst_src_stride | Input | Stride of adjacent consecutive data blocks on the source side (Global Memory), in bytes. Value range: [1, $2^{40}-1$]. |
| burst_dst_stride | Input | Stride of adjacent consecutive data blocks on the destination side (L1 Buffer), in bytes. Value range: [1, $2^{21}-1$]. When `burst_dst_stride` is not equal to `len_burst` (that is, when padding exists), `burst_dst_stride` must be 32-byte aligned; otherwise, an exception is triggered (even if `n_burst` is 1). |

**Table 2** `l2_cache_ctl` value description <a id="l2_cache_ctl_values"></a>

| Value | Mode | Meaning |
|------|------|------|
| 0 | Normal mode | Enables the L2 Cache and marks the allocated Cache Lines as high replacement priority. |
| 1 | Last mode | &bull; Enables the L2 Cache and marks the allocated Cache Lines as low replacement priority.<br>&bull; **The Last mode is not supported yet.**|
| 2 | Persistent mode | &bull; Enables the L2 Cache. Data already stored in the L2 Cache may be replaced. To ensure that data of specific GM always remains in the L2 Cache, the resident mode can be used.<br>&bull; Cache Lines marked as resident mode can only be replaced by other Cache Lines also marked as resident mode.<br>&bull; **The Persistent mode is not supported yet.**|
| 4 | Disable mode | Does not enable the L2 Cache. Each read is performed directly from GM, and the state of existing Cache Lines remains unchanged. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- This API is not an AIC call and returns directly.
- The `dst` start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, an exception is triggered.
- The `src` start address must be 1-byte aligned (Global Memory alignment requirement); otherwise, an exception is triggered.
- If the destination address of this instruction overlaps with that of other instructions, synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) must be inserted to serialize multiple instructions and prevent abnormal data.
- L1 Buffer capacity limit: the total L1 Buffer capacity is 512 KB. The sum of the dst offset and the transfer size must not exceed the boundary; otherwise, a destination address out-of-bounds exception is triggered.
- `len_burst`, `n_burst`, `burst_src_stride`, `burst_dst_stride`, `left_padding_count`, and `right_padding_count` must satisfy the value ranges in the parameter description; otherwise, the transfer result may not meet expectations.
- When `data_select_bit` is set to true, or in left-right padding mode (when either `left_padding_count` or `right_padding_count` is non-zero), [asc_set_gm2l1_pad](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md) must be called first to configure the padding value.

### Normal Padding Mode Constraints

- When `burst_dst_stride` is not equal to `len_burst`, `burst_dst_stride` must be 32-byte aligned; otherwise, an exception is triggered. This constraint takes effect even when `n_burst` is 1.

### Left-Right Padding Mode Constraints

- In left-right padding mode, `left_padding_count×sizeof(dtype) + right_padding_count×sizeof(dtype) + len_burst + pad` must be 32-byte aligned, where `pad` is the portion padded to satisfy 32-byte alignment. The padding value is not controlled by `data_select_bit`, and you must first call [asc_set_gm2l1_pad](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md) to configure the padding value.

### Circular Padding Mode Constraints

- Circular padding mode cannot be enabled at the same time as left-right padding mode.
- In circular padding mode, `loop1 dst stride` and `loop2 dst stride` must be 32-byte aligned. This constraint still applies even when the loop size is 1.

## Examples

The following example covers all three padding modes at once. All three scenarios move three 48-byte data blocks: in the Normal scenario, 16 bytes are padded at the tail of each block; in the Compact scenario, 16 bytes are padded only at the overall tail of the 144-byte valid data; in the left-right padding scenario, 4 bytes are padded on the left and 12 bytes on the right of each block. The example uses physical L1/UB addresses to pass results between the AIC and AIV, and compares the results byte by byte on the Host side.

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
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
constexpr uint32_t PAD_DWORD = 0xA5A5A5A5;
constexpr uint8_t LEFT_PADDING = 4;
constexpr uint8_t RIGHT_PADDING = 12;
constexpr int64_t CROSS_CORE_FLAG_ID = 0x8;
constexpr uint64_t AIV_TO_AIC_SYNC_ID = 0x1;
constexpr uint64_t AIC_TO_AIV_SYNC_ID = 0x2;

enum class PaddingMode : uint32_t { NORMAL, COMPACT, LEFT_RIGHT };

__global__ __mix__(1, 2) void asc_copy_gm2l1_align_modes_kernel(  // __mix__: one AIC and two AIVs execute cooperatively.
    __gm__ uint8_t* output, __gm__ uint8_t* input, uint32_t mode)
{
    asc_init();
    // AIC writes L1, AIV writes GM; cross-core shared buffers use physical addresses and cannot be mixed with array-form local buffers.
    __cbuf__ uint8_t* l1 = (__cbuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* staging_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(0);
    __ubuf__ uint8_t* result_ub = (__ubuf__ uint8_t*)asc_get_phy_buf_addr(NORMAL_OUTPUT_BYTES);
    uint32_t output_bytes = NORMAL_OUTPUT_BYTES;

    if ASC_IS_AIV {
        const bool is_output_core = asc_get_sub_block_id() == 0;
        if (is_output_core) {
            // Initialize the AIV-side physical UB, and notify the AIC only after its MTE2 operations are complete.
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
        asc_set_gm2l1_pad(PAD_DWORD);
        if (mode == static_cast<uint32_t>(PaddingMode::NORMAL)) {
            // Normal: dst stride is 64B, and the tail of each 48B burst is padded with 16B using the first byte of that burst.
            asc_copy_gm2l1_align_sync(l1, input, BURST_COUNT, BURST_BYTES, 0, 0, false,
                4, BURST_BYTES, NORMAL_STRIDE);
        } else if (mode == static_cast<uint32_t>(PaddingMode::COMPACT)) {
            // Compact: dst stride equals 48B; only at the end of the overall 144B data, 16B is padded with 0xA5.
            output_bytes = COMPACT_OUTPUT_BYTES;
            asc_copy_gm2l1_align_sync(l1, input, BURST_COUNT, BURST_BYTES, 0, 0, true,
                4, BURST_BYTES, BURST_BYTES);
        } else {
            // Left-right padding: each burst consists of 4B left padding + 48B data + 12B right padding, forcibly using 0xA5.
            asc_copy_gm2l1_align_sync(l1, input, BURST_COUNT, BURST_BYTES, LEFT_PADDING, RIGHT_PADDING, false,
                4, BURST_BYTES, NORMAL_STRIDE);
        }
        // GM-to-L1 runs on MTE2, and L1-to-UB runs on MTE1; establish cross-pipeline dependency through events.
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        // Read the L1 result back to another physical UB, and let AIV write it back to GM for host verification.
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
