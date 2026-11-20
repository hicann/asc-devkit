# GetBesselI0MaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:30:54.205Z pushedAt=2026-11-18T11:07:26.767Z nl=2 -->

## Description

This is an experimental API and may be adjusted or improved in later versions, with no guarantee of backward compatibility. You are advised to keep track of updates in later versions when using this API.

The BesselI0 API on the kernel side does not require reserving or applying for temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes that need to be reserved or allocated for on the host side. For the BesselI0 API, both the maximum and minimum temporary space sizes obtained are 0, which indicates that no temporary space is required for computation.

## Prototype

```cpp
void GetBesselI0MaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is float, pass 4 here. |
| isReuseSource | Input | Whether the source operand can be modified. Keep it consistent with the isReuseSource parameter of the BesselI0 API. |
| maxValue | Output | Maximum temporary space size, in bytes, required for the BesselI0 API to complete computation. Space exceeding this value is not used by this API. A maximum space size of 0 indicates that no temporary space is required for computation. |
| minValue | Output | Minimum temporary space size, in bytes, required for the BesselI0 API to complete computation. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

- Before using this API, include the header file. The header file path is `adv_api/experimental/bessel_i0_tiling.h`.
- Before use, add the compilation option `-DCANN_ASC_USE_EXPERIMENTAL` to enable experimental features.

## Examples

```cpp
// Input shape information is 1024; the operator input data type is float; the source operand cannot be modified.
std::vector<int64_t> shape_vec = {1024};
AscendC::TensorShape shape(shape_vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::experimental::GetBesselI0MaxMinTmpSize(shape, 4, false, maxValue, minValue);
// maxValue = 0, minValue = 0, no temporary space needs to be reserved.
```
