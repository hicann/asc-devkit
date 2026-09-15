# GetCtrlSpr(ISASI)

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T13:17:54.600Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Reads the value of a specific bit in the CTRL register (control register).

## Prototype

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline int64_t GetCtrlSpr()
```

## Parameter Description

**Table 1** Template parameter description

| Parameter Name | Description |
| --- | --- |
| **startBit** | Start bit index. |
| **endBit** | End bit index. |

## Data Type

This API is a register access API and does not involve tensor data type operations. The return value type is fixed as **int64_t**.

## Return Value

Returns the value of the corresponding bit in the CTRL register.

## Constraints

- The value range of **startBit** and **endBit** is [0, 63].
- **startBit** must be less than or equal to **endBit**.
- For the description of each bit, see [SetCtrlSpr(ISASI) Parameters](./SetCtrlSpr_ISASI.md#parameters).

## Example

The following is an example of reading the value of the **CTRL[48]** bit.

```cpp
int64_t valueCtrlAfterSet = AscendC::GetCtrlSpr<48, 48>();
```

For more examples, see [CtrlSpr sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/09_utils/ctrl_spr).
