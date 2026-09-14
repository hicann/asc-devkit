# GetReduceAllMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T12:29:48.868Z -->

## Description

The ReduceAll API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetReduceAllMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern, bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter value is consistent with the **srcShape** parameter of the ReduceAll API. |
| dataType | Input | Input data type, of the **ge::DataType** type. The currently supported data types are consistent with the template parameter **T** of the ReduceAll API. |
| pattern | Input | Specifies the computation axes of ReduceAll. It is of the **ReducePattern** type, whose definition is shown in the following code, including the Reduce axis and the Normal axis. **pattern** is formed by combining the letters A and R, with the number of letters equal to the dimension count of the input vector. The letter A indicates the Normal axis, and R indicates the Reduce axis. The parameter value is consistent with the **pattern** parameter of the ReduceAll API. Currently, only **AscendC::ReducePattern::AR** and **AscendC::ReducePattern::RA** are supported. |
| isSrcInnerPad | Input | Indicates whether the innermost axis data that actually needs to be computed is 32-byte aligned. The parameter value is consistent with the **isSrcInnerPad** parameter of the ReduceAll API. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. The parameter value is consistent with the **isReuseSource** parameter of the ReduceAll API. |
| maxValue | Output | Maximum temporary space size required for the ReduceAll API to complete computation. Space exceeding this value is not used by this API.<br> Note: <br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the ReduceAll API to complete computation. To ensure correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. |

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

For the complete call example, see [More Samples](../../math_compute/more_samples_83.md).

```
// The input shape is a 16*32 matrix; the operator input data type is float; modifying the source operand is not allowed.
auto shape = ge::Shape({ 16, 32 });
uint32_t maxValue = 0;
uint32_t minValue = 0;
bool isSrcInnerPad = true;
bool isReuseSource = false;
AscendC::GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, AscendC::ReducePattern::AR, isSrcInnerPad, isReuseSource, maxValue, minValue);
```
