# GetCumSumMaxMinTmpSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:07:52.319Z pushedAt=2026-11-18T08:54:29.340Z nl=2 -->

## Description

The CumSum API on the kernel side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

## Prototype

```
void GetCumSumMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isLastAxis, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. |
| isLastAxis | Input | Defines whether to process the first axis or the last axis. |
| maxValue | Output | Maximum temporary space size required for the Cumsum API to complete the computation, in bytes. Space exceeding this value is not used by the API.<br><br>Note that maxValue is only a reference value and may be larger than the remaining UB space. In this case, select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Cumsum API to complete the computation, in bytes. To ensure correct functionality, the reserved or allocated temporary space must not be smaller than this value. |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The input supports only a two-dimensional structure.

## Examples

```
// The input shape is a 32*32 matrix. The operator input data type is half. isLastAxis uses the default value true, and isReuseSource uses the default value false.
uint32_t firstDim = 32;
uint32_t lastDim = 32;
std::vector<int64_t> srcShapeDims = {firstDim, lastDim};
auto srcShape = AscendC::TensorShape(srcShapeDims);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetCumSumMaxMinTmpSize(srcShape, 2, true, false, maxValue, minValue);
```
