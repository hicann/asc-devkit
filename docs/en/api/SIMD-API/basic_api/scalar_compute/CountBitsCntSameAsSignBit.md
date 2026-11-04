# CountBitsCntSameAsSignBit<a name="ZH-CN_TOPIC_0000001834069645"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:18:09.155Z pushedAt=2026-10-26T07:41:44.758Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/CountBitsCntSameAsSignBit_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Counts the number of consecutive bits that are the same as the sign bit, starting from the most significant value bit, in the binary representation of a value of the int64\_t type.

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
| **valueIn** | Input | Binary number to be counted. |

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
