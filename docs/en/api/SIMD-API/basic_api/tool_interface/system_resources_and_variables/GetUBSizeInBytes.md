# GetUBSizeInBytes

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:27:30.164Z pushedAt=2026-10-26T07:41:44.878Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetUBSizeInBytes_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the size of the Unified Buffer (UB) in bytes. Developers calculate parameter values such as the loop count based on the UB size.

## Prototype

```cpp
__aicore__ inline constexpr uint32_t GetUBSizeInBytes()
```

## Parameters

None

## Return Value

Returns the size of the UB space.

<!-- npu="950" id8 -->
Under the Ascend 950PR&950DT products architecture, returns the constant 248 KB, in bytes.
<!-- end id8 -->

## Constraints

None

## Example

This example uses the UB size obtained by **GetUBSizeInBytes** to calculate the value of **tileNum**. For the complete operator sample, see [Sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/09_utils/get_ub_size).

```cpp
// totalLength is the total data length to be processed (number of elements).
this->totalLength = totalLength;
// GetUBSizeInBytes() / sizeof(half) -> Calculate how many half-type elements the UB can hold.
// Divide by 2 -> Reserve 50% of the UB space.
if (totalLength > AscendC::GetUBSizeInBytes() / sizeof(half) / 2) {
    this->tileLength = AscendC::GetUBSizeInBytes() / sizeof(half) / 2;
} else { // Prevent the tile size from exceeding the total data amount.
    this->tileLength = this->totalLength;
}
// Number of tiles to be iterated.
this->tileNum = this->totalLength / this->tileLength;
```
