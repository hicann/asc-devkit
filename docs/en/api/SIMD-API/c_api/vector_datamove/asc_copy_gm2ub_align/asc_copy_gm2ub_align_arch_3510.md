# asc_copy_gm2ub_align (NPU architecture version 3510)

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T01:40:04.069455+00:00 pushedAt=2026-11-16T02:10:59.435Z nl=2 -->

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

Copies data from Global Memory (GM) to the Unified Buffer (UB), supports data lengths that are not integer multiples of 32 bytes, and pads the data to 32-byte alignment during the copy. This API supports copying 8-bit/16-bit/32-bit data types. The copied data length must still meet the byte alignment requirement of the corresponding data type. For the alignment requirements of the source and destination start addresses, see [Parameters](#parameters).

This API supports the following two data movement modes:

- Contiguous data movement

    If the copied data length is not 32-byte aligned, the copied data is padded to 32-byte alignment. For this prototype, the hardware automatically pads with dummy data, and the value of the dummy data is the value of the first element of the data block. To use constant padding, use the high-dimensional slicing prototype, set `burst_count` to 1 and `enable_constant_pad` to `true`, and call [asc_set_copy_pad_val](../asc_set_copy_pad_val.md) to configure the padding value before the copy.

- High-dimensional slicing data movement

    If the copied data length is not 32-byte aligned, the data is padded to 32-byte alignment. You can select Normal mode or Compact mode by configuring the `dst_stride` parameter. The following two padding methods are supported for non-32-byte-aligned scenarios:

    - Manual padding: Call `asc_set_copy_pad_val` to set the padding value before the copy, and set `enable_constant_pad` to `true`.
    - Automatic padding: Set `enable_constant_pad` to `false`, and the hardware automatically pads with dummy data. The value of the dummy data is the value of the first element of the data block.

    Data movement modes are described as follows:

    - Compact mode: Set `dst_stride` to `burst_len` and set `left_padding_num` and `right_padding_num` to 0. Multiple data blocks are tightly arranged in the UB and treated as a whole for 32-byte padding. If the total length of all data blocks is not 32-byte aligned, padding to 32-byte alignment is performed only at the end of the entire copy using the manual or automatic padding method described above.
    - Normal mode: Set `dst_stride` to a value that is not equal to `burst_len` and is 32-byte aligned. The address interval between adjacent destination data blocks is specified by `dst_stride`. Intervals can be reserved between data blocks, and the data blocks do not need to be tightly arranged.
        - When both `left_padding_num` and `right_padding_num` are 0, each data block is padded to 32-byte alignment separately using the manual or automatic padding method described above.
        - When `left_padding_num` or `right_padding_num` is non-zero, `enable_constant_pad` does not take effect, and `asc_set_copy_pad_val` must be called to configure the padding value before the copy. If the total number of bytes of the data block length and the left and right padding is not 32-byte aligned, the hardware continues to pad with dummy data on the right side to 32-byte alignment. The value of the dummy data is the value configured by `asc_set_copy_pad_val`.

    For example, when copying 3 data blocks of 48 bytes each without left or right padding:
    - In Compact mode, set `dst_stride` to 48. The start offsets of the data blocks in the UB are 0, 48, and 96, and 16 bytes are padded only at the end of the 144 bytes of valid data, as shown in Figure 1.
    - In Normal mode, set `dst_stride` to 64. The start offsets of the data blocks in the UB are 0, 64, and 128, and each data block is padded with 16 bytes separately, as shown in Figure 2.

    **Figure 1** 32-byte padding in Compact mode

    ![](../../../../figures/asc_copy_gm2ub_align_compact_padding.png "32-byte padding in Compact mode")

    **Figure 2** 32-byte padding in Normal mode

    ![](../../../../figures/asc_copy_gm2ub_align_normal_padding.png "32-byte padding in Compact mode")

    When copying only 1 data block, or when `burst_len` is already 32-byte aligned and there is no left or right padding, the copy results of the two modes are the same.

    When using loop copy mode, use [asc_set_gm2ub_loop_size](../asc_set_gm2ub_loop_size.md) to set the iteration counts of the two loop levels, and use [asc_set_gm2ub_loop1_stride](../asc_set_gm2ub_loop1_stride.md) and [asc_set_gm2ub_loop2_stride](../asc_set_gm2ub_loop2_stride.md) to set the start address offsets of the source and destination operand data blocks between adjacent iterations in the inner loop `loop1` and outer loop `loop2`, respectively. For product support information, the descriptions in the respective API documents prevail.

This API takes effect only on AIV.

## Prototype

### Contiguous Data Movement

```c
// Placeholder form.
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t size)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```c
// Example: The source and destination data type is bfloat16_t.
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst,
                                            __gm__ bfloat16_t* src,
                                            uint32_t size)
```

### High-Dimensional Slicing Data Movement

```c
// Placeholder form.
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_num,
                                            uint8_t right_padding_num,
                                            bool enable_constant_pad,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```c
// Example: The source and destination data type is bfloat16_t.
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst,
                                            __gm__ bfloat16_t* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_num,
                                            uint8_t right_padding_num,
                                            bool enable_constant_pad,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

## Parameters

### Contiguous Data Movement

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination UB. Must be 32-byte aligned. |
| src | Input | Start address of the source GM. Must be 1-byte aligned. |
| size | Input | Size of data to be copied, in bytes. Value range: [1, $2^{21}−1$]. |

### High-Dimensional Slicing Data Movement

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination UB. Must be 32-byte aligned. |
| src | Input | Start address of the source GM. Must be 1-byte aligned. |
| burst_count | Input | Number of consecutive data blocks to be copied. Value range: [1, $2^{21}−1$]. |
| burst_len | Input | Length of each consecutive data block to be copied, in bytes. Value range: [1, $2^{21}−1$]. |
| left_padding_num | Input | Number of elements to be padded on the left side of the consecutive data block. The padding data size corresponding to this parameter cannot exceed 32 bytes. In Compact mode, set this parameter to 0. |
| right_padding_num | Input | Number of elements to be padded on the right side of the consecutive data block. The padding data size corresponding to this parameter cannot exceed 32 bytes. In Compact mode, set this parameter to 0. |
| enable_constant_pad | Input | When both `left_padding_num` and `right_padding_num` are 0, configures the padding method for unaligned scenarios. The values are described as follows: <br>&bull; `true`: Manual padding. The padding value is the value set by the `asc_set_copy_pad_val` API. <br>&bull; `false`: Automatic padding. The hardware pads with dummy data, and the value of the dummy data is the value of the first element of the data block.<br>When `left_padding_num` or `right_padding_num` is non-zero, this parameter does not take effect. |
| l2_cache_mode | Input | Enum value of type [asc_load_l2_cache_mode](../../defs/enum/asc_load_l2_cache_mode.md), which configures the management policy of data in L2 Cache. |
| src_stride | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the head of the preceding data block and the head of the following data block), in bytes. Value range: [0, $2^{40}−1$].<br>When only 1 data block is copied, that is, `burst_count` is set to 1, this parameter can be set to 0. |
| dst_stride | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the head of the preceding data block and the head of the following data block), in bytes, used to select the data copy mode. Value range: [0, $2^{21}−1$].<br>&bull; Equal to `burst_len`: Compact mode. Destination data blocks are tightly arranged in the UB, and `dst_stride` supports byte alignment.<br>&bull; Not equal to `burst_len`: Normal mode. `dst_stride` must meet the 32-byte alignment requirement.<br>When only 1 data block is copied, that is, `burst_count` is set to 1, this parameter can be set to 0. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### General Constraints

- Calling this API on a non-AIV returns directly.
- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If there is a data dependency between this instruction and other instructions, you must insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Contiguous Data Movement Constraints

- If `size` is not 32-byte aligned, the copied data is padded to 32-byte alignment, and the destination UB must reserve space for the padded size. For this contiguous data movement prototype, the hardware always pads with dummy data, and the value of the dummy data is the value of the first element of the data block.
- `size` must satisfy dtype byte alignment: when dtype is b16, it must be a multiple of 2; when dtype is b32, it must be a multiple of 4.

### High-Dimensional Slicing Data Movement Constraints

- `len_burst` must meet the dtype byte alignment requirement: when dtype is b16, it must be a multiple of 2; when dtype is b32, it must be a multiple of 4.
- When `left_padding_num` or `right_padding_num` is non-0, `enable_constant_pad` does not take effect, and `asc_set_copy_pad_val` must be called to configure the padding value before the copy. The padding data size corresponding to `left_padding_num` and `right_padding_num` must not exceed 32 bytes.
- When `dst_stride` is not equal to `burst_len`, `dst_stride` requires 32-byte alignment.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 256;

__global__ __vector__ void asc_copy_gm2ub_align_arch3510_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    // Use 256 bytes to cover a full register of data, allowing the aligned copy result to be checked byte by byte.
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    // PIPE_MTE3 waits for the PIPE_MTE2 load to complete before copying UB data to GM.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, local, INPUT_BYTES);
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
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    for (uint32_t i = 0; i < 256; ++i) golden[i] = input[i];
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_gm2ub_align_arch3510_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_gm2ub_align passed." : "[Failed] asc_copy_gm2ub_align failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
