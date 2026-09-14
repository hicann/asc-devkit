# GetDequantizeMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T12:05:26.725Z -->

## Description

The Dequantize API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetDequantizeMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor of the Dequantize API. |
| typeSize | Input | Data type size of the input srcTensor of the Dequantize API, in bytes. For example, if the input data type is int32_t, pass 4 here. |
| maxValue | Output | Maximum temporary space size required for the Dequantize API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Dequantize API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

```
// The input shape information is (2,1024).
// In the DequantizeParams passed to the Dequantize API, m = 2 and n = 1024; the input data type of the operator is int32_t.
std::vector<int64_t> shapeVec = {2, 1024};
ge::Shape srcShape(shapeVec);
uint32_t typeSize = 4;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetDequantizeMaxMinTmpSize(srcShape, typeSize, maxValue, minValue);
```
