# sqrt

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:13:34.711Z pushedAt=2026-11-13T08:46:49.211Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/math_functions/sqrt_res.md#id1 -->

## Description

Returns the square root of the input data.

## Prototype

```
template <typename T>
__aicore__ inline T sqrt(const T src)
```

## Parameters

**Table 1** Template parameters

| parameter name | Description |
| --- | --- |
| T | Data type of the input data src. Currently supported data types are float and int64_t. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand. |

## Constraints

The input must be a non-negative number. If the input is negative, the return value of the API is meaningless.

## Return Value

Returns the square root of the input data.

For data of type int64\_t, the computation result is truncated to an integer.

## Examples

```
int64_t src = 4;

int64_t result = AscendC::Std::sqrt(src);
// result: 2
```
