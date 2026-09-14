# GetReduceMeanMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T12:49:43.697Z -->

## Description

The **ReduceMean** API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetReduceMeanMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern, bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter value is consistent with the **srcShape** parameter of the **ReduceMean** API. |
| dataType | Input | Input data type, of the **ge::DataType** type. The currently supported data types are consistent with the template parameter **T** of the **ReduceMean** API. |
| pattern | Input | Specifies the computation axis of **ReduceMean**. It is of the **ReducePattern** type, whose definition is shown in the following code, including the Reduce axis and the Normal axis. **pattern** is formed by a combination of the letters A and R, the number of which is the same as the dimension of the input vector. The letter A indicates the Normal axis, and the letter R indicates the Reduce axis. The parameter value is consistent with the **pattern** parameter of the **ReduceMean** API. Currently, only **AscendC::ReducePattern::AR** and **AscendC::ReducePattern::RA** are supported. |
| isSrcInnerPad | Input | Indicates whether the innermost axis data to be actually computed is 32-byte aligned. The parameter value is consistent with the **isSrcInnerPad** parameter of the **ReduceMean** API. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. The parameter value is consistent with the **isReuseSource** parameter of the **ReduceMean** API. |
| maxValue | Output | Maximum temporary space size required for the **ReduceMean** API to complete computation. Space exceeding this value is not used by this API.<br> Note: <br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the **ReduceMean** API to complete computation. To ensure correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. |

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
// Input shape information is 1024; the operator input data type is float; do not modify the source operand.
auto shape = ge::Shape({ 16, 32 });
uint32_t maxValue = 0;
uint32_t minValue = 0;
bool isSrcInnerPad = true;
bool isReuseSource = false;
AscendC::GetReduceMeanMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, AscendC::ReducePattern::AR, isSrcInnerPad, isReuseSource, maxValue, minValue);
```
