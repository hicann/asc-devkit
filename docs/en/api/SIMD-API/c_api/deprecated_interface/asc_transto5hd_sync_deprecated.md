---
okftype: reference.api
description: "Deprecated API page: asc_transto5hd_b8_sync, b16_sync, and b32_sync convert NCHW data to NC1HWC0 layout by address register sequence. A single repeat processes 512 bytes and can be used for 2D matrix transpose. It is recommended to use asc_transto5hd plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [layout_transform, transpose]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Alignment, address overlap, and repeatStride descriptions follow their general constraints"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles synchronization wait in the alternative"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_permute_sel/asc_set_va_reg.md
    kind: requires
    condition: "You must first use this interface to associate the preceding and following 8 address sequences with the address registers"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transto5hd.md
    kind: replaced_by
    condition: "It is recommended to use the asc_transto5hd interface instead and explicitly call asc_sync"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 lists the _sync composite interfaces such as vector data movement as deprecated. The replacement path is asc_transto5hd plus asc_sync. The interface has not been removed."
---
# asc_transto5hd_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:13:08.701Z pushedAt=2026-11-18T03:12:16.689Z nl=2 -->

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

The header file path is: `"c_api/vector_compute/compute/vector_permute_sel.h"`.

**The `asc_transto5hd_b8_sync`, `asc_transto5hd_b16_sync`, and `asc_transto5hd_b32_sync` interfaces are deprecated. Use the [asc_transto5hd](../vector_compute/vector_permute_sel/asc_transto5hd.md) interface and the [asc_sync](../sync/intra_core_sync/asc_sync.md) interface instead.**

Data format conversion, generally used to convert the NCHW format to the NC1HWC0 format. In particular, it can also be used to transpose two-dimensional matrix data blocks. This interface can process 512 bytes of data (16 DataBlocks) in a single Repeat. Depending on the data type, it supports matrix transposition of different shapes (for example, when the data type is half, a single Repeat can complete the transposition of a 16*16 matrix), and it also supports multiple Repeat operations.

The conversion rule within a single repeat is as follows:

- When the input data type bit width is 16 bits, each DataBlock contains 16 numbers. The instruction internals loop 16 times, and each iteration takes values from the corresponding positions of the specified 16 DataBlocks to form a new DataBlock unit and place it into the destination address. As shown in the following figure, srcList[0]-srcList[15] in the figure represent the 16 DataBlocks of the source operand. ![](../../../figures/asc_transto5hd_b16.png "Figure 1 Conversion rule when the input data type bit width is 16 bits")

- When the data type bit width is 32 bits, each DataBlock contains 8 numbers. The instruction internals loop 8 times, and each iteration takes values from the corresponding positions of the specified 16 DataBlocks to form 2 new DataBlocks and place them into the destination address. As shown in the following figure: ![](../../../figures/asc_transto5hd_b32.png "Figure 2 Conversion rule when the input data type bit width is 32 bits")

- When the data type bit width is 8 bits, each DataBlock contains 32 numbers. The instruction internals loop 16 times, and each iteration takes values from the corresponding positions of the specified 16 DataBlocks to form half a DataBlock and place it into the destination address. Whether reading and storing are in the high half or low half of the DataBlock is determined by the parameters srcHighHalf and dstHighHalf. As shown in the following figure: ![](../../../figures/asc_transto5hd_b8.png "Figure 3 Conversion rule when the input data type bit width is 8 bits")

Based on the conversion rules above, use this interface for NC1HWC0 format conversion or matrix transposition. NC1HWC0 format conversion is relatively complex, and its specific conversion method is given here:

When converting the NCHW format to the NC1HWC0 format, if the data type bit width is 32 bits or 16 bits, then C0=16; if the data type bit width is 8 bits, then C0=32. The following figure uses C0=16 as an example for introduction: ![](../../../figures/asc_transto5hd_NCHW2NC1HWC0.png)

## Prototype

- Synchronous computation

```cpp
__aicore__ inline void asc_transto5hd_b32_sync(ub_addr8_t dst, ub_addr8_t src, uint8_t repeat,
                                               uint16_t dst_stride, uint16_t src_stride)
__aicore__ inline void asc_transto5hd_b16_sync(ub_addr8_t dst, ub_addr8_t src, uint8_t repeat,
                                               uint16_t dst_stride, uint16_t src_stride)
__aicore__ inline void asc_transto5hd_b8_sync(ub_addr8_t dst, ub_addr8_t src, uint8_t repeat,
                                              uint16_t dst_stride, uint16_t src_stride, bool dst_high_half, bool src_high_half)
```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Destination address register.|
|src|Input|Source address register.|
|repeat|Input|Number of repeated iterations, repeat∈[0,255].<br>Notes:<br>&bull;When repeat is 1, the effective starting position of the destination operand/source operand is the starting position of the dst/src sequence input plus dst_stride/src_stride. To make the effective starting position of the destination operand/source operand be the starting position of the dst/src sequence input, set dst_stride/src_stride to 0.<br>&bull;When repeat is greater than 1, in the first repeat the effective starting position of the destination operand/source operand is the starting position of the dst/src sequence input, and in the second repeat dst_stride/src_stride is added. And so on.|
|dst_stride|Input|Address stride of the same DataBlock of the destination operand between adjacent iterations, unit: DataBlock.<br>For details about the address stride parameter of the same DataBlock between adjacent iterations, see [repeatStride](../general_description_and_constraints.md#repeatStride).|
|src_stride|Input|Address stride of the same DataBlock of the source operand between adjacent iterations, unit: DataBlock.<br>For details about the address stride parameter of the same DataBlock between adjacent iterations, see [repeatStride](../general_description_and_constraints.md#repeatStride).|
|dst_high_half|Input|Specifies whether the data at each dst address is stored in the high half or the low half of the DataBlock. This configuration is supported only when the operand data type is int8_t/uint8_t.<br>This parameter is of bool type and has the following two values:<br>&bull; true: indicates storage in the high half of the DataBlock.<br>&bull; false: indicates storage in the low half of the DataBlock.|
|src_high_half|Input|Specifies whether the data at each src address is stored in the high half or the low half of the DataBlock. This configuration is supported only when the operand data type is int8_t/uint8_t.<br>This parameter is of bool type and has the following two values:<br>&bull; true: indicates storage in the high half of the DataBlock.<br>&bull; false: indicates storage in the low half of the DataBlock.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address alignment constraints, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- When performing conversion from the NCHW format to the NC1HWC0 format, the general usage is to configure each element in src/dst as the starting point of each HW plane.
- For better performance, when the data type bit width is 8 bits, it is recommended to first fix dst_high_half/src_high_half, then repeat in the HW direction, and then change dst_high_half/src_high_half.

## Examples

```cpp
// Example 1:
constexpr uint64_t total_length = 256;    // total_length refers to the data length involved in the computation
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
const uint32_t STEP = 16;
// src_list and dst_list are arrays of 16 DataBlock addresses
__ubuf__ half* src_list[16] = {
    (__ubuf__ half*)(src + 0 * STEP),
    (__ubuf__ half*)(src + 1 * STEP),
    (__ubuf__ half*)(src + 2 * STEP),
    (__ubuf__ half*)(src + 3 * STEP),
    (__ubuf__ half*)(src + 4 * STEP),
    (__ubuf__ half*)(src + 5 * STEP),
    (__ubuf__ half*)(src + 6 * STEP),
    (__ubuf__ half*)(src + 7 * STEP),
    (__ubuf__ half*)(src + 8 * STEP),
    (__ubuf__ half*)(src + 9 * STEP),
    (__ubuf__ half*)(src + 10 * STEP),
    (__ubuf__ half*)(src + 11 * STEP),
    (__ubuf__ half*)(src + 12 * STEP),
    (__ubuf__ half*)(src + 13 * STEP),
    (__ubuf__ half*)(src + 14 * STEP),
    (__ubuf__ half*)(src + 15 * STEP)
};
__ubuf__ half* dst_list[16] = {
    (__ubuf__ half*)(dst + 0 * STEP),
    (__ubuf__ half*)(dst + 1 * STEP),
    (__ubuf__ half*)(dst + 2 * STEP),
    (__ubuf__ half*)(dst + 3 * STEP),
    (__ubuf__ half*)(dst + 4 * STEP),
    (__ubuf__ half*)(dst + 5 * STEP),
    (__ubuf__ half*)(dst + 6 * STEP),
    (__ubuf__ half*)(dst + 7 * STEP),
    (__ubuf__ half*)(dst + 8 * STEP),
    (__ubuf__ half*)(dst + 9 * STEP),
    (__ubuf__ half*)(dst + 10 * STEP),
    (__ubuf__ half*)(dst + 11 * STEP),
    (__ubuf__ half*)(dst + 12 * STEP),
    (__ubuf__ half*)(dst + 13 * STEP),
    (__ubuf__ half*)(dst + 14 * STEP),
    (__ubuf__ half*)(dst + 15 * STEP)
};
const int32_t VA_REG_ARRAY_LEN = 8;
uint8_t repeat = 1;
uint16_t dst_stride = 0;
uint16_t src_stride = 0;

// The asc_set_va_reg interface requires the first 8 and last 8 address sequences to be associated with address registers respectively
asc_set_va_reg(VA0, dst_list);
asc_set_va_reg(VA1, dst_list + VA_REG_ARRAY_LEN);
asc_set_va_reg(VA2, src_list);
asc_set_va_reg(VA3, src_list + VA_REG_ARRAY_LEN);

asc_transto5hd_b16_sync(VA0, VA2, repeat, dst_stride, src_stride);

// Example 2:
constexpr uint64_t total_length = 2 * 32 * 16 * 16;    // total_length refers to the data length involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
const uint32_t DST_STEP = 16;
const uint32_t SRC_STEP = 16 * 16;
const uint32_t OFFSET = 16 * 16 * 16;
const int32_t VA_REG_ARRAY_LEN = 8;
uint8_t repeat = 16;
uint16_t dst_stride = 16;
uint16_t src_stride = 1;

// Define src_list and dst_list outside the loop.
__ubuf__ half* src_list[16];
__ubuf__ half* dst_list[16];

for (int j = 0; j < 4; ++j) {
    // Update the address list inside the loop.
    for (int i = 0; i < 16; ++i) {
        src_list[i] = (__ubuf__ half*)(src + OFFSET * j + i * SRC_STEP);
        dst_list[i] = (__ubuf__ half*)(dst + OFFSET * j + i * DST_STEP);
    }

    // Set the registers and perform the conversion in each iteration.
    asc_set_va_reg(VA0, dst_list);
    asc_set_va_reg(VA1, dst_list + VA_REG_ARRAY_LEN);
    asc_set_va_reg(VA2, src_list);
    asc_set_va_reg(VA3, src_list + VA_REG_ARRAY_LEN);

    asc_transto5hd_b16_sync(VA0, VA2, repeat, dst_stride, src_stride);
}
```
