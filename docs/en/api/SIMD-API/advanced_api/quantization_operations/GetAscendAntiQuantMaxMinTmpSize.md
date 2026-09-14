# GetAscendAntiQuantMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T11:56:29.531Z -->

## Description

The AscendAntiQuant API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetAscendAntiQuantMaxMinTmpSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose, ge::DataType inputDataType, ge::DataType outputDataType, uint32_t& maxValue, uint32_t& minValue)
```

```
uint32_t GetAscendAntiQuantMaxTmpSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose, ge::DataType inputDataType, ge::DataType outputDataType)
```

```
uint32_t GetAscendAntiQuantMinTmpSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose, ge::DataType inputDataType, ge::DataType outputDataType)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input src. |
| scaleShape | Input | Shape information of the input scale. |
| isTranspose | Input | Whether to transpose. |
| inputDataType | Input | Input data type, of the ge::DataType type. |
| outputDataType | Input | Output data type, of the ge::DataType type. |
| maxValue | Output | Maximum temporary space size required for the AscendAntiQuant API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the AscendAntiQuant API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

**GetAscendAntiQuantMaxMinTmpSize**: None

**GetAscendAntiQuantMaxTmpSize**: Maximum temporary space size required for the AscendAntiQuant API to complete computation.

**GetAscendAntiQuantMinTmpSize**: Minimum temporary space size required for the AscendAntiQuant API to complete computation.

## Constraints

None

## Examples

```
uint32_t maxValue = 0;
uint32_t minValue = 0;
std::vector<int64_t> srcDims = { 64, 512 };
auto srcShape = ge::Shape(srcDims);
std::vector<int64_t> scaleDims = { 1, 512 };
auto scaleShape = ge::Shape(scaleDims);
bool isTranspose = false;
AscendC::GetAscendAntiQuantMaxMinTmpSize(srcShape, scaleShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxValue, minValue);
```
