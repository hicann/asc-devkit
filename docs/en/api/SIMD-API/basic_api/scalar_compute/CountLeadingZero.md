# CountLeadingZero<a name="ZH-CN_TOPIC_0000001834229569"></a>

<!-- md-trans-meta sourceCommit=b75a0dc7dc91a8e352541f98f46fc736d8863e1a translatedAt=2026-08-27T11:13:43.476Z pushedAt=2026-09-01T01:51:02.727Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Counts the number of leading zeros in the binary representation of a `uint64_t` value (the number of `0`s from the most significant bit to the first `1` in the binary representation; returns 64 when the input is 0).

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
| valueIn | Input | Binary number whose leading zeros are counted. |

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
