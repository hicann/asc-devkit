# GetReduceSumMaxMinTmpSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:24:37.286Z pushedAt=2026-11-18T08:54:29.826Z nl=2 -->

## Description

The ReduceSum API on the kernel-side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel-side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetReduceSumMaxMinTmpSize(const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern, bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). The parameter value is consistent with the srcShape parameter of the ReduceSum API. |
| dataType | Input | Input data type. The parameter type is [AscendC::TensorDataType](../../data_structures/TensorDataType.md). The currently supported data types are consistent with the template parameter T of the ReduceSum API. |
| pattern | Input | Specifies the computation axis of ReduceSum. The type is ReducePattern, whose definition is shown in the following code, including the Reduce axis and the Normal axis. pattern is formed by a combination of A and R letters, the number of which equals the number of dimensions of the input vector. A indicates the Normal axis and R indicates the Reduce axis. The value of this parameter is consistent with the pattern parameter of the ReduceSum API. Currently, only AscendC::ReducePattern::AR and AscendC::ReducePattern::RA are supported. |
| isSrcInnerPad | Input | Whether the innermost axis data to be computed is 32-byte aligned. The parameter value is consistent with the isSrcInnerPad parameter of the ReduceSum API. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. The parameter value is consistent with the isReuseSource parameter of the ReduceSum API. |
| maxValue | Output | Maximum temporary space size required for the ReduceSum API to complete computation. Space exceeding this value is not used by this API.<br> Note: <br>maxValue is only a reference value and may be larger than the remaining UB space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the ReduceSum API to complete computation. To ensure correct functionality, the temporary space reserved or allocated during API computation must not be smaller than this value. |

```
enum class ReducePattern : uint32_t {
    AR = 0,
    RA = 1,
    R,
    ARA,
    ARAR,
    ARARA,
    ARARAR,
    ARARARA,
    ARARARAR,
    ARARARARA,
    RAR,
    RARA,
    RARAR,
    RARARA,
    RARARAR,
    RARARARA,
};
```

## Return Value

None

## Constraints

None

## Examples

For the complete call example, see [More Examples](../../math_compute/more_examples.md).

```
// Input shape information is 1024; the operator input data type is float; do not modify the source operand.
auto shape = AscendC::TensorShape({16, 32});
uint32_t maxValue = 0;
uint32_t minValue = 0;
bool isSrcInnerPad = true;
bool isReuseSource = false;
AscendC::GetReduceSumMaxMinTmpSize(
    shape, AscendC::TensorDataType::DT_FLOAT, AscendC::ReducePattern::AR, isSrcInnerPad, isReuseSource, maxValue, minValue);
```
