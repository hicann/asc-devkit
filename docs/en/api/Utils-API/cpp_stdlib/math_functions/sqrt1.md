# sqrt

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:36:23.588Z pushedAt=2026-09-11T04:55:22.060Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: not supported
- Atlas A2 training products/Atlas A2 inference products: not supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

Computes the square root of the input data.

## Prototype

```
template <typename T>
__aicore__ inline T sqrt(const T src)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the input data **src**. The currently supported data types are float and int64_t. |

**Table 2** API parameter description

| Name | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand. |

## Constraints

The input must be a non-negative number. If the input is a negative number, the return value of the API is meaningless.

## Return Value

Returns the square root of the input data.

For data of the int64\_t data type, the computation result is truncated to an integer.

## Example

```
int64_t src = 4;

int64_t result = AscendC::Std::sqrt(src);
// result: 2
```

