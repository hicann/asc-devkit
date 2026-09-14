# GetBitwiseAndTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T13:13:41.500Z -->

## Description

This API is used to obtain **maxLivedNodeCount** and **extraBuf**. With a fixed space size, the maximum number of elements that the operator can compute at a time can be derived from **maxLivedNodeCount** and **extraBuf**. **maxLivedNodeCount** indicates how many times the temporary space is the size of the space occupied by the data volume of a single computation; **extraBuf** indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the **BitwiseAnd** API. Reserve a space of **currBuff** size for it, use the **GetBitwiseAndTmpBufferFactorSize** API to obtain the output values of **maxLivedNodeCount** and **extraBuf**, and then derive the maximum number of elements that the operator can compute at a time as follows:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLivedNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLivedNodeCount and extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-level APIs to be called) and the currently available temporary space, derive the maximum number of elements **currentShapeSize** that can be computed at a time as follows:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLivedNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLivedNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text indicates the space available for API computation, excluding the space for user input and output. In addition, the **maxLivedNodeCount** value obtained by the API may be 0. When performing the calculation, check that this value is non-zero to avoid division-by-zero errors.

## Prototype

```
void GetBitwiseAndTmpBufferFactorSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize, uint32_t& maxLivedNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **ascendcPlatform** | Input | Input platform information. For the definition of **PlatformAscendC**, see [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is uint16_t, pass 2 here. |
| **maxLivedNodeCount** | Output | Maximum number of live nodes, indicating how many times the temporary space is the size of the data volume computed in a single computation. |
| **extraBuf** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLivedNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

For the complete calling example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLivedNodeCount = 0;
uint32_t extraBuf = 0;
auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
AscendC::GetBitwiseAndTmpBufferFactorSize(plat, typeSize, maxLivedNodeCount, extraBuf);
```

