# GetWhereTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T10:53:44.610Z -->

## Description

This API is used to obtain **maxLivedNodeCount** and **extraBuf**. Given a fixed space size, you can derive the maximum number of elements that the operator can compute in a single computation based on **maxLivedNodeCount** and **extraBuf**. **maxLivedNodeCount** indicates the ratio of the temporary space to the space occupied by the data of a single computation. **extraBuf** indicates the size of the additional temporary space used.

The following shows an example of the derivation:

-   The operator implementation needs to call the **Where** API. You reserve a space of **currBuff** size for it, and use the **GetWhereTmpBufferFactorSize** API to obtain the output values **maxLivedNodeCount** and **extraBuf**. The maximum number of elements that the operator can compute in a single computation can then be derived as follows:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLivedNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLivedNodeCount and extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-level APIs to be called) and the currently available temporary space, the maximum number of elements **currentShapeSize** that can be computed in a single computation is derived as follows:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLivedNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLivedNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text indicates the space available for API computation, from which the space for user input and output must be excluded. In addition, the **maxLivedNodeCount** value obtained by the API may be 0. When performing the computation, you must check that this value is not 0 to avoid a division-by-zero error.

## Prototype

```
void GetWhereTmpBufferFactorSize(const platform_ascendc::PlatformAscendC& ascendcPlatform,const uint32_t typeSize, uint32_t& maxLivedNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1** Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Input platform information. For the definition of **PlatformAscendC**, see [Constructor and Destructor](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is **half**, pass 2. |
| maxLivedNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the size of the data volume for a single computation. |
| extraBuf | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize  \* typeSize derived from **maxLivedNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/typeSize.

## Examples

For a complete example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLivedNodeCount = 0;
uint32_t extraBuf = 0;
auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
AscendC::GetWhereTmpBufferFactorSize(plat, typeSize, maxLivedNodeCount, extraBuf);
```

