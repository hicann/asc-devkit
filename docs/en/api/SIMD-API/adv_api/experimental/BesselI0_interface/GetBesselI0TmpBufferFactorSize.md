# GetBesselI0TmpBufferFactorSize

<!-- md-trans-meta sourceCommit=f35aa1aa07ddfe8424151ecc1171c821c6d33b37 translatedAt=2026-11-10T06:31:05.000Z pushedAt=2026-11-18T11:07:26.776Z nl=2 -->

## Description

This API is experimental and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. You are advised to keep track of updates in later versions.

This API is used to obtain maxLiveNodeCount and extraBuf. Given a fixed space size, you can use maxLiveNodeCount and extraBuf to derive the maximum number of elements that can be computed in a single pass. maxLiveNodeCount indicates how many times the temporary space is the size of the space occupied by the data volume of a single computation; extraBuf indicates the size of the additional temporary space used.

For the BesselI0 API, the obtained maxLiveNodeCount and extraBuf are both 0, indicating that no temporary space is required for the computation. The maxLiveNodeCount value obtained by the API may be 0. When estimating the maximum number of elements computed in a single computation, check that this value is not 0 to avoid division by zero.

## Prototype

```cpp
void GetBesselI0TmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is float, pass 4 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| extraBuffer | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

- Before using this API, include the header file. The header file path is `adv_api/experimental/bessel_i0_tiling.h`.
- Before use, add the compilation option `-DCANN_ASC_USE_EXPERIMENTAL` to enable the experimental feature.

## Examples

```cpp
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::experimental::GetBesselI0TmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
// maxLiveNodeCount = 0, extraBuf = 0. No temporary space needs to be reserved.
```
