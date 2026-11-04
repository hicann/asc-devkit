# GetSFFValue<a name="ZH-CN_TOPIC_0000001787630014"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:21:33.984Z pushedAt=2026-10-26T07:41:44.764Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/GetSFFValue_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Finds the position of the first specified bit value (0 or 1) starting from the least significant bit in the binary representation of a uint64\_t type value. Returns -1 if not found.

Take the following GetSFFValue call as an example:

```cpp
int64_t sffValue = AscendC::GetSFFValue<1>(0x1000000000000100ULL); // Returns 8.
```

The calculation result is shown in the following figure:

**Figure 1** Finding the position of the first 1 starting from the least significant bit using GetSFFValue<a name="fig1"></a>

![Finding the position of the first 1 starting from the least significant bit using GetSFFValue](../../../figures/GetSFFValue.png "Finding the position of the first 1 starting from the least significant bit using GetSFFValue")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <int countValue>
__aicore__ inline int64_t GetSFFValue(uint64_t valueIn)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| countValue | Specifies the bit value to search for, which is 0 or 1. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| valueIn | Input | Binary number to be searched. |

## Return Value<a name="section640mcpsimp"></a>

Returns the position where the first 0 or 1 appears in **valueIn**.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

```cpp
int64_t sffValue;
uint64_t valueIn = 0xFFFFFFFFFFFFFFFFULL;    // The binary representation is all 1s.
sffValue = AscendC::GetSFFValue<0>(valueIn); // Returns -1.

valueIn = 0;                                 // The binary representation is all 0s.
sffValue = AscendC::GetSFFValue<1>(valueIn); // Returns -1.

valueIn = 0xFF00ULL;
sffValue = AscendC::GetSFFValue<0>(valueIn); // Returns 0.
sffValue = AscendC::GetSFFValue<1>(valueIn); // Returns 8.

valueIn = 0x00FFULL;
sffValue = AscendC::GetSFFValue<0>(valueIn); // Returns 8.
sffValue = AscendC::GetSFFValue<1>(valueIn); // Returns 0.
```
