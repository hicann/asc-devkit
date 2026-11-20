# GetMeanMaxMinTmpSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:22:23.092Z pushedAt=2026-11-18T08:54:29.749Z nl=2 -->

## Description

The Mean API on the Kernel side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the Kernel side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the Kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetMeanMaxMinTmpSize(const uint32_t n, const uint32_t srcTypeSize, const uint32_t accTypeSize, const bool isReuseSource, uint32_t& maxSize, uint32_t& minSize)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | Number of elements actually computed in each row of the input data. |
| srcTypeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| accTypeSize | Input | Size of the data type (accType) that actually participates in computation, in bytes. For details about the accType parameter, see the [Mean API parameter description](Mean.md). |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the Mean API. This is a reserved parameter. |
| maxSize | Output | Maximum temporary space size required for the Mean API to complete computation. Space exceeding this value is not used by this API.<br> Note: <br>maxSize is only a reference value and may be larger than the remaining UB space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minSize | Output | Minimum temporary space size required for the Mean API to complete computation. To ensure correct functionality, the temporary space reserved or allocated during API computation must not be smaller than this value. |

## Return Value

None

## Constraints

None

## Examples

```
// The input data type T of the operator is half, accType is float, and isReuseSource uses the default value false.
uint32_t n = 3;
uint32_t srcTypeSize = 2;
uint32_t accTypeSize = 4;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetMeanMaxMinTmpSize(n, srcTypeSize, accTypeSize, false, maxValue, minValue);
```
