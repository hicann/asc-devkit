# GetDataBlockSizeInBytes

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T05:41:33.762Z pushedAt=2026-11-18T01:25:31.212Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetDataBlockSizeInBytes_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the size of one DataBlock on the current chip version, in bytes.

Developers calculate the parameter values to be passed to API instructions, such as [repeatTime, dstBlkStride, srcblkStride, dstRepStride, and srcRepStride](../../memory_vector_compute/SIMD_compute/high_dim_split.md), based on the DataBlock size.

## Function Prototype

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
// Each repeat has 8 DataBlocks, so 8 * dataBlockSize / sizeof(half) elements can be computed. The mask is configured so that all elements in the iteration participate in the computation.
uint64_t mask = 8 * dataBlockSize / sizeof(half);
// A total of 512 elements are computed. Divide by the number of elements participating in each repeat to obtain repeatTime.
uint8_t repeatTime = 512 / mask;
// dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
// dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
AscendC::Add(dstLocal, src0Local, src1Local, mask, repeatTime, { 1, 1, 1, 8, 8, 8 });
```
