# GetQuantizeMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:38:14.385Z pushedAt=2026-11-18T11:07:26.911Z nl=2 -->

## Description

The Quantize API on the kernel-side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel-side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

## Prototype

```
void GetQuantizeMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor of the Quantize API. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| typeSize | Input | Data type size of the input srcTensor of the Quantize API, in bytes. For example, if the input data type is half, pass 2 here. |
| maxValue | Output | Maximum temporary space size required for the Quantize API to complete computation, in bytes. Space exceeding this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be greater than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Quantize API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved or allocated during API computation must not be less than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

```
// Input shape information is (2,1024)
// In the QuantizeParams passed to the Quantize API, m = 2 and n = 1024; the input data type of the operator is int32_t
std::vector<int64_t> shapeVec = {2, 1024};
AscendC::TensorShape srcShape(shapeVec);
uint32_t typeSize = 4;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetQuantizeMaxMinTmpSize(srcShape, typeSize, maxValue, minValue);
```
