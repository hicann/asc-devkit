# CountBitsCntSameAsSignBit<a name="ZH-CN_TOPIC_0000001834069645"></a>

<!-- md-trans-meta sourceCommit=b75a0dc7dc91a8e352541f98f46fc736d8863e1a translatedAt=2026-08-27T11:12:56.319Z pushedAt=2026-09-01T01:50:42.900Z -->

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

Counts the number of consecutive bits that are identical to the sign bit, starting from the most significant value bit, in the binary representation of an int64\_t value.

Take the following CountBitsCntSameAsSignBit call as an example:

```cpp
int64_t valueOut = AscendC::CountBitsCntSameAsSignBit(0x0100000000000020ULL); // Returns 6.
```

The calculation result is shown in the following figure:

**Figure 1**  CountBitsCntSameAsSignBit calculation diagram<a name="fig1"></a>

![CountBitsCntSameAsSignBit calculation diagram](../../../figures/CountBitsCntSameAsSignBit.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline int64_t CountBitsCntSameAsSignBit(int64_t valueIn)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| valueIn | Input | Binary number to be counted. |

## Return Value

Returns the number of consecutive bits that are the same as the sign bit, starting from the most significant value bit.

## Constraints<a name="section633mcpsimp"></a>

When the input is -1 (all bits are 1) or 0 (all bits are 0), -1 is returned.

## Example<a name="section837496171220"></a>

```cpp
int64_t valueOut;
int64_t valueIn = 0;                                    // The binary representation is all 0s.
valueOut = AscendC::CountBitsCntSameAsSignBit(valueIn); // Returns -1.

valueIn = 0xFFFFFFFFFFFFFFFFLL;                         // -1, the binary representation is all 1s.
valueOut = AscendC::CountBitsCntSameAsSignBit(valueIn); // Returns -1.

valueIn = 0x7FFFFFFFFFFFFFFFLL;                         // Maximum positive value.
valueOut = AscendC::CountBitsCntSameAsSignBit(valueIn); // Returns 0.

valueIn = 0x8000000000000000LL;                         // Minimum negative value.
valueOut = AscendC::CountBitsCntSameAsSignBit(valueIn); // Returns 0.

valueIn = 1;
valueOut = AscendC::CountBitsCntSameAsSignBit(valueIn); // Returns 62.

valueIn = 0x00F0000000000000LL;
valueOut = AscendC::CountBitsCntSameAsSignBit(valueIn); // Returns 7.
```
