# GetIsNanTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:12:31.439Z pushedAt=2026-11-18T08:54:29.484Z nl=2 -->

## Description

This API is used to obtain maxLivedNodeCount and extraBuf. Given a fixed space size, maxLivedNodeCount and extraBuf can be used to derive the maximum number of elements that can be computed in a single pass. maxLivedNodeCount indicates how many times the temporary space is the size of the space occupied by the data of a single computation; extraBuf indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the IsNan API. You reserve a space of currBuff size for it, and use the GetIsNanTmpBufferFactorSize API to obtain the output values maxLivedNodeCount and extraBuf. The maximum number of elements that can be computed in a single pass can then be derived as:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLivedNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs KernelIntf1 and KernelIntf2. Using the two sets of output values \(maxLivedNodeCount and extraBuf\) from the two GetXxxTmpBufferFactorSize APIs (where Xxx is the two high-level APIs to be called) and the currently available temporary space, the maximum number of elements that can be computed in a single pass, currentShapeSize, is derived as:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLivedNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLivedNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that currBuff in the preceding text indicates the space available for API computation, from which the space for user input and output must be excluded. In addition, the maxLivedNodeCount value obtained by the API may be 0. When performing the computation, ensure that this value is non-zero to avoid a division-by-zero error.

## Prototype

```
void GetIsNanTmpBufferFactorSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize, uint32_t& maxLivedNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter List

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Platform information input. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLivedNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| extraBuf | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize \* typeSize derived from maxLivedNodeCount and extraBuf is less than 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

For the complete call example, see [More Examples](../more_examples.md).

```
uint32_t maxLivedNodeCount = 0;
uint32_t extraBuf = 0;
auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
AscendC::GetIsNanTmpBufferFactorSize(plat, typeSize, maxLivedNodeCount, extraBuf);
```
