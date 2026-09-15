# GetRuntimeUBSize

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T12:06:44.788Z -->

## Applicable Products

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products Vector Core: Not supported
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products: Not supported
<!-- end id15 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the size of the runtime UB space, in bytes. Developers calculate parameter values such as the number of loop iterations based on the UB size.

## Prototype

```cpp
__aicore__ inline uint32_t GetRuntimeUBSize()
```

## Parameter Description

None

## Return Value

Returns the size of the runtime UB space, in bytes.

<!-- npu="950" id1 -->
Under the Ascend 950PR/Ascend 950DT architecture, in a mixed SIMD and SIMT scenario, the upper limit of the obtained UB size is 216 KB; in a non-mixed SIMD and SIMT scenario, the return value is a fixed 248 KB.
<!-- end id1 -->

## Constraints

None

## Example

This example uses the UB space size obtained by `GetRuntimeUBSize` to calculate the value of `tileNum`. For the complete operator sample, see [get_ub_size sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/09_utils/get_ub_size).

```cpp
// totalLength is the total data length to be processed (number of elements).
this->totalLength = totalLength;
// GetRuntimeUBSize() / sizeof(half) -> Calculate how many half-type elements the UB can hold.
// Divide by 2 -> Reserve 50% of the UB space.
if (totalLength > AscendC::GetRuntimeUBSize() / sizeof(half) / 2) {
    this->tileLength = AscendC::GetRuntimeUBSize() / sizeof(half) / 2;
} else { // Prevent the tile size from exceeding the actual total data amount.
    this->tileLength = this->totalLength;
}
// Number of tiles to be iterated.
this->tileNum = this->totalLength / this->tileLength;
```
