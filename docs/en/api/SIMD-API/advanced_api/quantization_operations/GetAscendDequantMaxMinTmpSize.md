# GetAscendDequantMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T11:59:27.816Z -->

## Description

The AscendDequant API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetAscendDequantMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. |
| typeSize | Input | Data type size of the input srcTensor, in bytes. For example, if the input data type is int32_t, pass 4 here. |
| maxValue | Output | Maximum temporary space size required for the AscendDequant API to complete computation. Space exceeding this value will not be used by this API. Within the range between the minimum and maximum temporary space sizes, as the temporary space size increases, the kernel-side API computation performance will be optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation. <br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the AscendDequant API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

```
// Input shape information is (2,1024)
// In the dequantParams passed to the AscendDequant API, m = 2 and n = 1024; the input data type of the operator is int32_t
std::vector<int64_t> shape_vec = {2, 1024};
ge::Shape srcShape(shape_vec);
uint32_t typeSize = 4;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetAscendDequantMaxMinTmpSize(srcShape, typeSize, maxValue, minValue);
```
