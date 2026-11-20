# GetReduceAllMaxMinTmpSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:22:48.847Z pushedAt=2026-11-18T08:54:29.762Z nl=2 -->

## Description

The ReduceAll API on the kernel side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetReduceAllMaxMinTmpSize(const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern, bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). The value must be consistent with the srcShape parameter of the ReduceAll API. |
| dataType | Input | Input data type. The parameter type is [AscendC::TensorDataType](../../data_structures/TensorDataType.md). The currently supported data types must be consistent with the template parameter T of the ReduceAll API. |
| pattern | Input | Specifies the computation axes of ReduceAll. The type is ReducePattern, which is defined in the following code and includes the Reduce axis and the Normal axis. pattern is formed by a combination of the letters A and R, with the number of letters equal to the number of dimensions of the input vector. The letter A indicates the Normal axis, and R indicates the Reduce axis. The value must be consistent with the pattern parameter of the ReduceAll API. Currently, only AscendC::ReducePattern::AR and AscendC::ReducePattern::RA are supported. |
| isSrcInnerPad | Input | Whether the data of the innermost axis to be computed is 32-byte aligned. The value must be consistent with the isSrcInnerPad parameter of the ReduceAll API. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. The value must be consistent with the isReuseSource parameter of the ReduceAll API. |
| maxValue | Output | Maximum temporary space size required for the ReduceAll API to complete computation. Space exceeding this value is not used by this API.<br> Note: <br>maxValue is for reference only and may be larger than the remaining UB space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the ReduceAll API to complete computation. To ensure correct functionality, the reserved or allocated temporary space during computation must not be smaller than this value. |

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
// Input shape is a 16*32 matrix; the operator input data type is float; the source operand cannot be modified.
auto shape = AscendC::TensorShape({16, 32});
uint32_t maxValue = 0;
uint32_t minValue = 0;
bool isSrcInnerPad = true;
bool isReuseSource = false;
AscendC::GetReduceAllMaxMinTmpSize(
    shape, AscendC::TensorDataType::DT_FLOAT, AscendC::ReducePattern::AR, isSrcInnerPad, isReuseSource, maxValue, minValue);
```
