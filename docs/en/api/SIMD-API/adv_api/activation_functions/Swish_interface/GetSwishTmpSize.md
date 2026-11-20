# GetSwishTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:20:55.496Z pushedAt=2026-11-18T11:07:26.481Z nl=2 -->

## Description

Obtains the Swish Tiling parameters: the maximum and minimum temporary space sizes required for the Swish API to complete computation.

## Prototype

```
inline void GetSwishTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2, that is, sizeof(half). |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the kernel-side API. |
| maxValue | Output | Maximum temporary space size required for the Swish API to complete computation, in bytes. Space exceeding this value is not used by this API. Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Swish API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

```
// Input shape information is 1024; the operator input data type is half; the source operand cannot be modified.
std::vector<int64_t> shape_vec = {1024};
AscendC::TensorShape shape(shape_vec);
uint32_t maxValue;
uint32_t minValue;
AscendC::GetSwishTmpSize(shape, 2, false, maxValue, minValue);
```
