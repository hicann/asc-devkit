# GetBitCount<a name="ZH-CN_TOPIC_0000001787630010"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:20:15.817Z pushedAt=2026-10-26T07:41:44.761Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/GetBitCount_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Counts the occurrences of a specified bit value (0 or 1) in the binary representation of a **uint64_t** value.

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
