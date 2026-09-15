# ResetCtrlSpr(ISASI)

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T13:21:24.575Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Resets specific bits of the CTRL register (control register). The bits specified by the template parameter \[startBit, endBit\] are reset to their default values. For the default values, see [Common CTRL Register Bit Description](SetCtrlSpr_ISASI.md#tab-950).

$\text{CTRL}[\,\text{startBit} : \text{endBit}\,] \leftarrow \text{defaultCtrl}[\,\text{startBit} : \text{endBit}\,]$

## Function Prototype

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline void ResetCtrlSpr()
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| **startBit** | Start bit index. |
| **endBit** | End bit index. |

## Data Types

This API is a register access API and does not involve tensor data type operations.

## Return Value

None

## Constraints

Only the CTRL[8:6], CTRL[10:9], CTRL[48], CTRL[50], CTRL[53], CTRL[59], and CTRL[60] bits are supported. For details about each bit, see [SetCtrlSpr(ISASI) Parameter Description](./SetCtrlSpr_ISASI.md#parameters).

## Example

In the following example, the **CTRL[48]** bit is reset to enable saturation mode.

```cpp
AscendC::SetCtrlSpr<48, 48>(1);
...
AscendC::ResetCtrlSpr<48, 48>();
```

For more examples, see [CtrlSpr sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/09_utils/ctrl_spr).
