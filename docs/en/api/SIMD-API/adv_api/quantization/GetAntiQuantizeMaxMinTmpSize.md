# GetAntiQuantizeMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:36:37.396Z pushedAt=2026-11-18T11:07:26.873Z nl=2 -->

## Description

The AntiQuantize API on the kernel side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

## Prototype

```
void GetAntiQuantizeMaxMinTmpSize(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& scaleShape, bool isTranspose, AscendC::TensorDataType inputDataType, AscendC::TensorDataType outputDataType, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor of the AntiQuantize API. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| scaleShape | Input | Shape information of the input scale of the AntiQuantize API. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| isTranspose | Input | Reserved parameter. Currently, only false is supported. |
| inputDataType | Input | Data type of the input srcTensor. The parameter type is [AscendC::TensorDataType](../data_structures/TensorDataType.md). |
| outputDataType | Input | Data type of the output dstTensor. The parameter type is [AscendC::TensorDataType](../data_structures/TensorDataType.md). |
| maxValue | Output | Maximum temporary space size, in bytes, required for the AntiQuantize API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the API on the kernel side improves to a certain extent. For better performance, you can reserve or allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size, in bytes, required for the AntiQuantize API to complete computation. To ensure functional correctness, the temporary space reserved or allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

```
uint32_t maxValue = 0;
uint32_t minValue = 0;
std::vector<int64_t> srcDims = {64, 512};
auto srcShape = AscendC::TensorShape(srcDims);
std::vector<int64_t> scaleDims = {1, 512};
auto scaleShape = AscendC::TensorShape(scaleDims);
bool isTranspose = false;
AscendC::GetAntiQuantizeMaxMinTmpSize(srcShape, scaleShape, isTranspose, AscendC::TensorDataType::DT_INT8, AscendC::TensorDataType::DT_BF16, maxValue, minValue);
```
