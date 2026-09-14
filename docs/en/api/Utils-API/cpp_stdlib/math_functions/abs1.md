# abs

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:35:00.827Z pushedAt=2026-09-11T04:55:16.187Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: not supported
- Atlas A2 training products/Atlas A2 inference products: not supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

Obtains the absolute value of the input data.

## Prototype

```
template <typename T>
__aicore__ inline T abs(const T src)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the input data **src**. Currently supported data types are int8_t, int16_t, int32_t, float, and int64_t. |

**Table 2**  API parameter description

| Name | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand. |

## Constraints

None

## Return Value

Absolute value of the input data.

## Example

```
int64_t src = -2;

int64_t result = AscendC::Std::abs(src);
// result: 2
```

