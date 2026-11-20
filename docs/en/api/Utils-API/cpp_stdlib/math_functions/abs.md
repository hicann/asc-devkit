# abs

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:09:40.511Z pushedAt=2026-11-13T08:46:49.204Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/math_functions/abs_res.md#id1 -->

## Description

Obtains the absolute value of the input.

## Prototype

```
template <typename T>
__aicore__ inline T abs(const T src)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the input data src. The currently supported data types are int8_t, int16_t, int32_t, float, and int64_t. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand. |

## Constraints

None

## Return Value

Returns the absolute value of the input data.

## Examples

```
int64_t src = -2;

int64_t result = AscendC::Std::abs(src);
// result: 2
```
