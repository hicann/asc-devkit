# GetUBSizeInBytes

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T12:11:45.964Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the size of the **UB** space in bytes. Developers calculate parameters such as the number of loop iterations based on the **UB** size.

## Prototype

```cpp
__aicore__ inline constexpr uint32_t GetUBSizeInBytes()
```

## Parameters

None

## Return Value

Returns the size of the **UB** space, in bytes.

## Constraints

None

## Example

This example uses the UB space size obtained by **GetUBSizeInBytes** to calculate the value of **tileNum**. For the complete operator sample, see [get\_ub\_size sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/09_utils/get_ub_size).

```cpp
// totalLength is the total length of data to be processed (number of elements).
this->totalLength = totalLength;
// GetUBSizeInBytes() / sizeof(half) -> Calculate how many half-type elements the UB can hold.
// Divide by 2 -> Reserve 50% of the UB space.
if (totalLength > AscendC::GetUBSizeInBytes() / sizeof(half) / 2) {
    this->tileLength = AscendC::GetUBSizeInBytes() / sizeof(half) / 2;
} else { // Prevent the tile size from exceeding the actual total amount of data.
    this->tileLength = this->totalLength;
}
// Number of tiles to be iterated.
this->tileNum = this->totalLength / this->tileLength;
```
