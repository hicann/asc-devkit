# CountLeadingZero<a name="ZH-CN_TOPIC_0000001834229569"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T04:46:52.820Z pushedAt=2026-11-18T01:25:31.108Z nl=2 -->

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
- Atlas training products: not supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/CountLeadingZero_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Counts the number of leading zeros in the binary representation of a uint64\_t value (the number of '0's from the most significant bit to the first '1' in the binary representation; returns 64 when the input is 0).

Take the following CountLeadingZero call as an example:

```cpp
int64_t leadingZeros = AscendC::CountLeadingZero(0x0100000000000020ULL); // Returns 7.
```

The calculation result is shown in the following figure:

**Figure 1** CountLeadingZero calculation diagram<a name="fig1"></a>

![CountLeadingZero calculation diagram](../../../figures/CountLeadingZero.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline int64_t CountLeadingZero(uint64_t valueIn)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| valueIn | Input | Binary number to be counted. |

## Return Value<a name="section640mcpsimp"></a>

Returns the number of leading zeros in **valueIn**.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

```cpp
int64_t leadingZeros;
uint64_t valueIn = 0x000000000000FFFFULL;          // The binary representation has 48 leading zeros.
leadingZeros = AscendC::CountLeadingZero(valueIn); // Returns 48.

valueIn = 0;                                       // The binary representation is all zeros.
leadingZeros = AscendC::CountLeadingZero(valueIn); // Returns 64.

valueIn = 0xFFFFFFFFFFFFFFFFULL;                   // The binary representation is all ones.
leadingZeros = AscendC::CountLeadingZero(valueIn); // Returns 0.
```
