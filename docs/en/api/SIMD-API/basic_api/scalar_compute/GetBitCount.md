# GetBitCount<a name="ZH-CN_TOPIC_0000001787630010"></a>

<!-- md-trans-meta sourceCommit=b75a0dc7dc91a8e352541f98f46fc736d8863e1a translatedAt=2026-08-27T11:14:20.452Z -->

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

Counts the occurrences of a specified bit value (0 or 1) in the binary representation of a `uint64_t` value.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <int countValue>
__aicore__ inline int64_t GetBitCount(uint64_t valueIn)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| countValue | Specifies the bit value to be counted. The value is 0 or 1. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | ----- | ----- |
| valueIn | Input | Binary number to be counted. |

## Return Value

Returns the number of 0s or 1s in **valueIn**.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

```cpp
uint64_t valueIn = 0xFFFF;    // The binary representation contains 16 ones and 48 zeros.

int64_t oneCount = AscendC::GetBitCount<1>(valueIn); // Count the number of 1s in the binary format of valueIn. The binary format contains 16 ones.

int64_t zeroCount = AscendC::GetBitCount<0>(valueIn); // Count the number of 0s in the binary format of valueIn. The binary format contains 48 zeros.
```
