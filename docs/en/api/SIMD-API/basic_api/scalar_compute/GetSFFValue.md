# GetSFFValue<a name="ZH-CN_TOPIC_0000001787630014"></a>

<!-- md-trans-meta sourceCommit=b75a0dc7dc91a8e352541f98f46fc736d8863e1a translatedAt=2026-08-27T11:15:32.650Z -->

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
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not Supported
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Finds the position of the first specified bit value (0 or 1) from the least significant bit in the binary representation of a **uint64_t** value, and returns -1 if it is not found.

Take the following **GetSFFValue** call as an example:

```cpp
int64_t sffValue = AscendC::GetSFFValue<1>(0x1000000000000100ULL); // Returns 8.
```

The calculation result is shown in the following figure:

**Figure 1** Finding the position of the first 1 from the least significant bit using GetSFFValue<a name="fig1"></a>

![Finding the position of the first 1 from the least significant bit using GetSFFValue](../../../figures/GetSFFValue.png "Finding the position of the first 1 from the least significant bit using GetSFFValue")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <int countValue>
__aicore__ inline int64_t GetSFFValue(uint64_t valueIn)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| countValue | Specifies the bit value to find, which is 0 or 1. |

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
