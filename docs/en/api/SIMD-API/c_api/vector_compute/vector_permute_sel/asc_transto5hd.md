---
okftype: reference.api
description: "API family for NCHW to NC1HWC0 data format conversion, which can also be used for two-dimensional matrix data block transpose. It is divided into b8, b16, and b32 prototypes by data bit width, processes 512 bytes, that is, 16 DataBlocks, per Repeat, and has pipeline type PIPE_V."
tag:
  - roles: [data_movement]
  - operations: [layout_transform]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For address alignment, address overlap, and repeatStride descriptions, see the corresponding sections on this page"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_permute_sel/asc_set_va_reg.md
    kind: requires
    condition: "The dst/src address registers must first be associated with address sequences through this API"
  - path: docs/zh/guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md
    kind: related
    condition: "NCHW and NC1HWC0 data format description"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "asc_transto5hd_b8, asc_transto5hd_b16, and asc_transto5hd_b32 are converted from macro form to formal function declarations and added to the public header file (function implementations moved into impl/c_api/memory_base_impl/vector_permute_sel_intf_impl.h, and declarations entered the finest-grained public header file include/c_api/vector_compute/compute/vector_permute_sel.h); based on CANN_9_2_0 Release Notes line 31 \"formally added to the public header file\" and the first appearance of uncommented function declarations in upstream/9.2.0."
---
# asc_transto5hd

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.952120+00:00 pushedAt=2026-11-16T02:10:59.147Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_permute_sel.h"`.

Performs [data format](../../../../../guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) conversion, generally used to convert the NCHW format to the NC1HWC0 format. In particular, it can also be used to transpose two-dimensional matrix data blocks. This API can process 512 bytes of data (16 DataBlocks) in a single Repeat. Depending on the data type, it supports matrix transposition of different shapes (for example, when the data type is half, a single Repeat can complete the transposition of a 16*16 matrix), and it also supports multiple Repeat operations.

The conversion rules in a single repeat are as follows:

- When the input data type bit width is 16 bits, each DataBlock contains 16 numbers. The instruction internally loops 16 times, and in each loop, values are taken from the corresponding positions in the specified 16 DataBlocks to form a new DataBlock unit, which is then placed into the destination address. As shown in the following figure, srcList[0]-srcList[15] in the figure represent the 16 DataBlocks of the source operand. ![](../../../../figures/asc_transto5hd_b16.png "Figure 1 Conversion rule when the input data type bit width is 16 bits")

- When the data type bit width is 32 bits, each DataBlock contains 8 numbers. The instruction internally loops 8 times, and in each loop, values are taken from the corresponding positions in the specified 16 DataBlocks to form 2 new DataBlocks, which are then placed into the destination address. As shown in the following figure: ![](../../../../figures/asc_transto5hd_b32.png "Figure 2 Conversion rule when the input data type bit width is 32 bits")

- When the data type bit width is 8 bits, each DataBlock contains 32 numbers. The instruction internally loops 16 times, and in each loop, values are taken from the corresponding positions in the specified 16 DataBlocks to form half a DataBlock, which is then placed into the destination address. Whether the read and storage occur in the high half or the low half of the DataBlock is determined by the srcHighHalf and dstHighHalf parameters. As shown in the following figure: ![](../../../../figures/asc_transto5hd_b8.png "Figure 3 Conversion rule when the input data type bit width is 8 bits")

Based on the conversion rules above, use this API for NC1HWC0 format conversion or matrix transposition. NC1HWC0 format conversion is relatively complex, and its specific conversion method is provided here:

When converting the NCHW format to the NC1HWC0 format, if the data type bit width is 32 bits or 16 bits, C0=16; if the data type bit width is 8 bits, C0=32. The following figure uses C0=16 as an example: ![](../../../../figures/asc_transto5hd_NCHW2NC1HWC0.png)

## Prototype

```c
__aicore__ inline void asc_transto5hd_b32(ub_addr8_t dst,
                                          ub_addr8_t src,
                                          uint8_t repeat,
                                          uint16_t dst_stride,
                                          uint16_t src_stride)
__aicore__ inline void asc_transto5hd_b16(ub_addr8_t dst,
                                          ub_addr8_t src,
                                          uint8_t repeat,
                                          uint16_t dst_stride,
                                          uint16_t src_stride)
__aicore__ inline void asc_transto5hd_b8(ub_addr8_t dst,
                                         ub_addr8_t src,
                                         uint8_t repeat,
                                         uint16_t dst_stride,
                                         uint16_t src_stride,
                                         bool dst_high_half,
                                         bool src_high_half)
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Destination address register.|
|src|Input|Source address register.|
|repeat|Input|Number of repeated iterations, repeat∈[0,255].<br>Notes:<br>&bull;When repeat is 1, the effective start position of the destination operand/source operand is the start position of the dst/src sequence input plus dst_stride/src_stride. To make the effective start position of the destination operand/source operand the start position of the dst/src sequence input, set dst_stride/src_stride to 0.<br>&bull;When repeat is greater than 1, in the first repeat, the effective start position of the destination operand/source operand is the start position of the dst/src sequence input; in the second repeat, dst_stride/src_stride is added. And so on.|
|dst_stride|Input|Stride between the same DataBlock addresses of the destination operand across adjacent iterations, in DataBlocks.<br>For details about the address stride parameter of the same DataBlock across adjacent iterations, see [repeatStride](../../general_description_and_constraints.md#repeatStride).|
|src_stride|Input|Stride between the same DataBlock addresses of the source operand across adjacent iterations, in DataBlocks.<br>For details about the address stride parameter of the same DataBlock across adjacent iterations, see [repeatStride](../../general_description_and_constraints.md#repeatStride).|
|dst_high_half|Input|Specifies whether the data at each dst address is stored in the upper or lower half of a DataBlock. This setting is supported only when the operand data type is int8_t/uint8_t.<br>This parameter is of type bool and has two values:<br>&bull; true: Stored in the upper half of a DataBlock.<br>&bull; false: Stored in the lower half of a DataBlock.|
|src_high_half|Input|Specifies whether the data at each src address is stored in the upper or lower half of a DataBlock. This setting is supported only when the operand data type is int8_t/uint8_t.<br>This parameter is of type bool and has two values:<br>&bull; true: Stored in the upper half of a DataBlock.<br>&bull; false: Stored in the lower half of a DataBlock.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address alignment constraints, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- `dst` and `src` cannot be passed the same start address register position, that is, the destination address register pair and the source address register pair cannot overlap.
- `dst` and `src` support only `VA0`, `VA2`, `VA4`, and `VA6`, and they cannot be the same. For the configuration method, see [asc_set_va_reg](./asc_set_va_reg.md). Each address register stores 8 addresses. The API combines the passed starting register with the adjacent register into a group to process 16 addresses.
- For better performance, it is recommended to pass compile-time constants for `dst`, `src`, `dst_high_half`, and `src_high_half`, for example, directly passing `VA0`, `VA2`, `true`, or `false`, or passing local variables initialized with these constants. If runtime variables are passed, the compiler retains branching logic for address registers or Boolean parameters, which affects performance.
- When converting the NCHW format to the NC1HWC0 format, the general usage is to configure each element in `src` and `dst` as the start point of each HW plane.
- When the data type bit width is 8 bits, it is recommended to fix `dst_high_half` and `src_high_half` first, complete Repeat processing along the HW direction, and then modify `dst_high_half` and `src_high_half` for better performance.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

One run verifies the following two scenarios:

- Matrix transposition in a single Repeat. The input is a 16×16 matrix stored contiguously row by row, with each row occupying exactly one DataBlock of the 16-bit data type. The code writes the start address of each row into `src_list` and the start address of each row of the destination matrix into `dst_list`; the two groups of addresses are associated with adjacent address registers through `asc_set_va_reg`. When calling `asc_transto5hd_b16`, set `repeat=1`, `dst_stride=0`, and `src_stride=0` to complete one transposition. Finally, check element by element whether the output at row `col` and column `row` equals the input at row `row` and column `col`.
- Multiple Repeats combined with an outer loop complete the NCHW to NC1HWC0 format conversion. The input shape is N=2, C=32, H=W=16, divided into groups of 16 channels, for a total of 4 groups. The outer loop updates 16 source addresses and 16 destination addresses for one group of channels each time: the source addresses point to the HW plane start points of each channel, and the destination addresses point to the start points of the corresponding channels in the target layout. Each group calls `asc_transto5hd_b16` once, traversing 16 segments of data along the HW direction through `repeat=16`; `src_stride=1` makes each iteration read the next DataBlock of the source plane, and `dst_stride=16` makes the output advance according to the NC1HWC0 layout. Finally, verify the results element by element in the index order of `[N, C1, H, W, C0]`.

```cpp
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t MATRIX_SIDE = 16;
constexpr uint32_t MATRIX_ELEMENTS = MATRIX_SIDE * MATRIX_SIDE;
constexpr uint32_t TOTAL_ELEMENTS = 2 * 32 * 16 * 16;
constexpr uint32_t OFFSET = 16 * 16 * 16;

__global__ __vector__ void transto5hd_kernel(__gm__ uint16_t* matrix_out,
                                             __gm__ uint16_t* matrix_in,
                                             __gm__ uint16_t* layout_out,
                                             __gm__ uint16_t* layout_in)
{
    asc_init();
    __ubuf__ uint16_t src[TOTAL_ELEMENTS];
    __ubuf__ uint16_t dst[TOTAL_ELEMENTS];
    __ubuf__ half* src_list[16];
    __ubuf__ half* dst_list[16];

    // Scenario 1: Complete a 16x16 matrix transposition in one repeat. When stride is 0, read from the start of the address list.
    asc_copy_gm2ub_align(src, matrix_in, MATRIX_ELEMENTS * sizeof(uint16_t));
    // Wait for the matrix data load to complete before transposition.
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    for (int i = 0; i < 16; ++i) {
        src_list[i] = reinterpret_cast<__ubuf__ half*>(src + i * 16);
        dst_list[i] = reinterpret_cast<__ubuf__ half*>(dst + i * 16);
    }
    asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0);
    asc_set_va_reg(VA0, dst_list);
    asc_set_va_reg(VA1, dst_list + 8);
    asc_set_va_reg(VA2, src_list);
    asc_set_va_reg(VA3, src_list + 8);
    asc_transto5hd_b16(VA0, VA2, 1, 0, 0);
    // Wait for transposition to complete before storing the matrix.
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(matrix_out, dst, MATRIX_ELEMENTS * sizeof(uint16_t));

    // Scenario 2: N=2, C=32, H=W=16. Four groups of C0=16 channels are processed separately.
    // Wait for the preceding transposition to complete before reusing src and the address list.
    asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0);
    asc_copy_gm2ub_align(src, layout_in, TOTAL_ELEMENTS * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    // Before writing dst again, wait for the preceding matrix store to finish reading dst.
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 16; ++i) {
            src_list[i] = reinterpret_cast<__ubuf__ half*>(src + OFFSET * j + i * 16 * 16);
            dst_list[i] = reinterpret_cast<__ubuf__ half*>(dst + OFFSET * j + i * 16);
        }
        asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0);
        asc_set_va_reg(VA0, dst_list);
        asc_set_va_reg(VA1, dst_list + 8);
        asc_set_va_reg(VA2, src_list);
        asc_set_va_reg(VA3, src_list + 8);
        // 16 repeats advance along the HW direction, with src_stride=1 and dst_stride=16, both in DataBlocks.
        asc_transto5hd_b16(VA0, VA2, 16, 16, 1);
        if (j < 3) {
            // The next group modifies the address list. Wait until PIPE_V has consumed the addresses of this group.
            asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0);
            asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0);
        }
    }
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(layout_out, dst, TOTAL_ELEMENTS * sizeof(uint16_t));
    asc_sync();
}

void check(aclError status, const char* operation)
{
    if (status != ACL_SUCCESS) {
        std::cerr << operation << " failed: " << status << std::endl;
        std::exit(1);
    }
}

} // namespace

int main()
{
    std::vector<uint16_t> matrix_in(MATRIX_ELEMENTS), matrix_out(MATRIX_ELEMENTS);
    std::vector<uint16_t> layout_in(TOTAL_ELEMENTS), layout_out(TOTAL_ELEMENTS);
    for (uint32_t i = 0; i < MATRIX_ELEMENTS; ++i) matrix_in[i] = i;
    for (uint32_t i = 0; i < TOTAL_ELEMENTS; ++i) layout_in[i] = i;

    check(aclInit(nullptr), "aclInit");
    check(aclrtSetDevice(0), "aclrtSetDevice");
    uint16_t *matrix_in_device = nullptr, *matrix_out_device = nullptr;
    uint16_t *layout_in_device = nullptr, *layout_out_device = nullptr;
    check(aclrtMalloc(reinterpret_cast<void**>(&matrix_in_device), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc matrix_in");
    check(aclrtMalloc(reinterpret_cast<void**>(&matrix_out_device), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc matrix_out");
    check(aclrtMalloc(reinterpret_cast<void**>(&layout_in_device), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc layout_in");
    check(aclrtMalloc(reinterpret_cast<void**>(&layout_out_device), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc layout_out");
    check(aclrtMemcpy(matrix_in_device, MATRIX_ELEMENTS * sizeof(uint16_t), matrix_in.data(), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE), "copy matrix_in");
    check(aclrtMemcpy(layout_in_device, TOTAL_ELEMENTS * sizeof(uint16_t), layout_in.data(), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE), "copy layout_in");
    transto5hd_kernel<<<1, 0>>>(matrix_out_device, matrix_in_device, layout_out_device, layout_in_device);
    check(aclrtSynchronizeDevice(), "aclrtSynchronizeDevice");
    check(aclrtMemcpy(matrix_out.data(), MATRIX_ELEMENTS * sizeof(uint16_t), matrix_out_device, MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_DEVICE_TO_HOST), "copy matrix_out");
    check(aclrtMemcpy(layout_out.data(), TOTAL_ELEMENTS * sizeof(uint16_t), layout_out_device, TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_DEVICE_TO_HOST), "copy layout_out");

    bool matrix_passed = true;
    for (uint32_t row = 0; row < 16; ++row) {
        for (uint32_t col = 0; col < 16; ++col) {
            matrix_passed &= matrix_out[col * 16 + row] == matrix_in[row * 16 + col];
        }
    }
    bool layout_passed = true;
    for (uint32_t n = 0; n < 2; ++n) {
        for (uint32_t c = 0; c < 32; ++c) {
            for (uint32_t hw = 0; hw < 256; ++hw) {
                uint32_t src_index = (n * 32 + c) * 256 + hw;
                uint32_t dst_index = ((n * 2 + c / 16) * 256 + hw) * 16 + c % 16;
                layout_passed &= layout_out[dst_index] == layout_in[src_index];
            }
        }
    }
    std::cout << "16x16 transpose: " << (matrix_passed ? "PASS" : "FAIL") << std::endl;
    std::cout << "NCHW to NC1HWC0: " << (layout_passed ? "PASS" : "FAIL") << std::endl;
    check(aclrtFree(matrix_in_device), "free matrix_in");
    check(aclrtFree(matrix_out_device), "free matrix_out");
    check(aclrtFree(layout_in_device), "free layout_in");
    check(aclrtFree(layout_out_device), "free layout_out");
    check(aclrtResetDevice(0), "aclrtResetDevice");
    check(aclFinalize(), "aclFinalize");
    return matrix_passed && layout_passed ? 0 : 1;
}
```
