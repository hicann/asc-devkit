# GetReduceProdMaxMinTmpSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:24:25.156Z pushedAt=2026-11-18T08:54:29.816Z nl=2 -->

## Description

The ReduceProd API on the kernel-side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel-side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetReduceProdMaxMinTmpSize(const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern, bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape of the input data. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). The parameter value is consistent with the srcShape parameter of the ReduceProd API. |
| dataType | Input | Data type of the input. The parameter type is [AscendC::TensorDataType](../../data_structures/TensorDataType.md). The currently supported data types are consistent with the template parameter T of the ReduceProd API. |
| pattern | Input | Specifies the computation axis of ReduceProd. The type is ReducePattern, which is defined in the following code and includes the Reduce axis and the Normal axis. pattern is formed by a combination of the letters A and R, the number of which is the same as the dimension of the input vector. The letter A indicates the Normal axis, and R indicates the Reduce axis. The parameter value is consistent with the pattern parameter of the ReduceProd API. Currently, only AscendC::ReducePattern::AR and AscendC::ReducePattern::RA are supported. |
| isSrcInnerPad | Input | Whether the data on the innermost axis that actually needs to be computed is 32-byte aligned. The parameter value is consistent with the isSrcInnerPad parameter of the ReduceProd API. |
| isReuseSource | Input | Whether to reuse the space of the source operand. The parameter value is consistent with the isReuseSource parameter of the ReduceProd API. |
| maxValue | Output | Maximum temporary space size required for the ReduceProd API to complete the computation. Space exceeding this value is not used by this API.<br> Note: <br>maxValue is only a reference value and may be larger than the remaining UB space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the ReduceProd API to complete the computation. To ensure correct functionality, the temporary space reserved or allocated during API computation must not be smaller than this value. |

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
// The input shape is a 16*32 matrix. The operator input data type is float. isReuseSource is passed with the default value false.
uint32_t maxSize;
uint32_t minSize;
auto shape = AscendC::TensorShape({16, 32});
AscendC::GetReduceProdMaxMinTmpSize(
    shape, AscendC::TensorDataType::DT_FLOAT, AscendC::ReducePattern::AR, true, false, maxSize, minSize);
```
