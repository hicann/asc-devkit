# GetDataBlockSizeInBytes

<!-- md-trans-meta sourceCommit=53d6dd141dd5063557396f6918eca149e02f93ce translatedAt=2026-08-27T12:05:12.922Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the size of one DataBlock on the current chip version, in bytes.

Developers calculate the parameter values to be passed to API instructions, such as [repeatTime, dstBlkStride, srcblkStride, dstRepStride, and srcRepStride](../../memory_vector_compute/SIMD_compute/high_dimension_slicing.md), based on the DataBlock size.

## Prototype

```cpp
__aicore__ inline constexpr int16_t GetDataBlockSizeInBytes()
```

## Parameters

None

## Return Value

Returns the size of one **DataBlock** on the current chip version, in bytes.

## Constraints

None

## Example

The following sample obtains the size of a DataBlock through `GetDataBlockSizeInBytes` and calculates the value of `repeatTime`:

```cpp
int16_t dataBlockSize = AscendC::GetDataBlockSizeInBytes();
// Each repeat contains 8 DataBlocks. Calculate the number of elements as 8 * dataBlockSize / sizeof(half), and configure mask so that all elements in the iteration participate in the computation.
uint64_t mask = 8 * dataBlockSize / sizeof(half);
// A total of 512 numbers are computed. Divide by the number of elements participating in each repeat to obtain repeatTime.
uint8_t repeatTime = 512 / mask;
// dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
// dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
AscendC::Add(dstLocal, src0Local, src1Local, mask, repeatTime, { 1, 1, 1, 8, 8, 8 });
```
