# GetRuntimeUBSize

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T05:42:28.887Z pushedAt=2026-11-18T01:25:31.213Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetRuntimeUBSize_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the size of the runtime Unified Buffer (UB) space, in bytes. Developers calculate parameters such as the number of loops based on the UB size.

## Function Prototype

```cpp
__aicore__ inline uint32_t GetRuntimeUBSize()
```

## Parameters

None

## Return Value

Returns the size of the runtime UB space, in bytes.

<!-- npu="950" id8 -->
Under the Ascend 950PR&950DT product architecture, in the [SIMD and SIMT hybrid scenario](../../../../SIMT-API/SIMD_SIMT_hybrid_programming_intro/SIMD_SIMT_hybrid_programming_intro.md), the maximum UB size that can be obtained is 216KB; in non-SIMD and SIMT hybrid scenarios, the return value is a fixed value of 248KB.
<!-- end id8 -->

## Constraints

None

## Example

This example uses the UB space size obtained by `GetRuntimeUBSize` to calculate the value of `tileNum`. For a complete operator sample, see [get\_ub\_size sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/09_utils/get_ub_size).

```cpp
// totalLength is the total length of data to be processed (number of elements).
this->totalLength = totalLength;
// GetRuntimeUBSize() / sizeof(half) -> calculates how many half-type elements the UB can hold.
// Divide by 2 -> reserve 50% of the UB space.
if (totalLength > AscendC::GetRuntimeUBSize() / sizeof(half) / 2) {
    this->tileLength = AscendC::GetRuntimeUBSize() / sizeof(half) / 2;
} else { // Prevent the fragment size from exceeding the total amount of actual data.
    this->tileLength = this->totalLength;
}
// Number of fragments to iterate over.
this->tileNum = this->totalLength / this->tileLength;
```
