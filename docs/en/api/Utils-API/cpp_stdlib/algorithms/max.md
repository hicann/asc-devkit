# max

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:03:27.114Z pushedAt=2026-11-13T08:46:49.189Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/algorithms/max_res.md#id1 -->

## Description

Compares two numbers of the same data type and returns the maximum value.

## Prototype

```
template <typename T, typename U>
__host__ __aicore__ inline constexpr auto max(const T& src0, const U& src1)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the input data src0. Currently supported data types are bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |
| U | Data type of the input data src1. Currently supported data types are bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, float, int64_t, and uint64_t.<br><br>Reserved type, which must be the same as T. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Source operand. Input that participates in the comparison. |
| src1 | Input | Source operand. Input that participates in the comparison. |

## Constraints

The two source operands must have the same data type.

## Return Value

Returns the maximum value of the two input items.

## Examples

```
int64_t src0 = 1;
int64_t src1 = 2;

int64_t result = AscendC::Std::max(src0, src1);
// result: 2
```
