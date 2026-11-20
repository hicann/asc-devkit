# ResetCtrlSpr(ISASI)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T04:55:41.272Z pushedAt=2026-11-18T01:25:31.126Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/ResetCtrlSpr_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Resets specific bits of the **CTRL** register (control register). The bits specified by the template parameter \[startBit, endBit\] are reset to their default values. For the default values, see [Table Common CTRL Register Bit Descriptions](SetCtrlSpr_ISASI.md#tab-950).

$\text{CTRL}[\,\text{startBit} : \text{endBit}\,] \leftarrow \text{defaultCtrl}[\,\text{startBit} : \text{endBit}\,]$

## Function Prototype

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline void ResetCtrlSpr()
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| startBit | Start bit index. |
| endBit | End bit index. |

## Data Types

This API is a register access API and does not involve tensor data type operations.

## Return Value

None

## Constraints

Only the **CTRL[8:6]**, **CTRL[10:9]**, **CTRL[48]**, **CTRL[50]**, **CTRL[53]**, **CTRL[59]**, and **CTRL[60]** bits are supported. For details about each bit, see [SetCtrlSpr(ISASI) Parameter Description](./SetCtrlSpr_ISASI.md#parameters).

## Example

In the following example, the **CTRL[48]** bit is reset to enable saturation mode.

```cpp
AscendC::SetCtrlSpr<48, 48>(1);
...
AscendC::ResetCtrlSpr<48, 48>();
```

For more examples, see [CtrlSpr sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/09_utils/ctrl_spr).
