# GetSigmoidMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:15:30.632Z pushedAt=2026-11-18T11:07:26.393Z nl=2 -->

## Description

The Sigmoid API on the kernel-side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel-side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

## Prototype

```
void GetSigmoidMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand, consistent with the Sigmoid API. |
| maxValue | Output | Maximum temporary space size required for the Sigmoid API to complete computation, in bytes. Space beyond this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining space of the UB. |
| minValue | Output | Minimum temporary space size required for the Sigmoid API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

For the complete call example, see [More Examples](../../math_compute/more_examples.md).

```
// Input shape information is 1024; the input data type of the operator is half; modifying the source operand is not allowed.
std::vector<int64_t> shape_vec = {1024};
AscendC::TensorShape shape(shape_vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetSigmoidMaxMinTmpSize(shape, 2, false, maxValue, minValue);
```
