# GetCtrlSpr(ISASI)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:26:55.127Z pushedAt=2026-10-26T07:41:44.772Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/GetCtrlSpr_ISASI_res.md#id1 -->

## Description

The header file path is: `"basic_api/kernel_operator_common_intf.h"`.

Reads the value of a specific bit in the **CTRL** register (control register).

## Function Prototype

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline int64_t GetCtrlSpr()
```

## Parameter Description

**Table 1** Template parameter description

| Parameter Name | Description |
| --- | --- |
| startBit | Starting bit index. |
| endBit | Ending bit index. |

## Data Type

This API is a register access API and does not involve tensor data type operations. The return value type is fixed to `int64_t`.

## Return Value

Returns the value of the corresponding bit in the **CTRL** register.

## Constraints

- The value range of **startBit** and **endBit** is [0, 63].
- **startBit** is less than or equal to **endBit**.
- For the description of each bit, see [SetCtrlSpr(ISASI) Parameter Description](./SetCtrlSpr_ISASI.md#parameters).

## Example

The following is an example of reading the value of the CTRL[48] bit.

```cpp
int64_t valueCtrlAfterSet = AscendC::GetCtrlSpr<48, 48>();
```

For more examples, see [CtrlSpr sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/09_utils/ctrl_spr).
