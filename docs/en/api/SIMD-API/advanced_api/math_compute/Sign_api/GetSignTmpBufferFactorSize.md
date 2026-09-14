# GetSignTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T09:38:14.225Z -->

## Function Description

This API is used to obtain **maxLiveNodeCount** and **extraBuf**. Given a fixed space size, the maximum number of elements that can be computed in a single computation can be derived from **maxLiveNodeCount** and **extraBuf**. **maxLiveNodeCount** indicates how many times the temporary space is the size of the space occupied by the data volume of a single computation; **extraBuf** indicates the size of the additional temporary space used.

The derivation example is as follows:

-   The operator implementation needs to call the Sign API. The developer reserves a space of currBuff size for it, and uses the **GetSignTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCount** and **extraBuf**, from which the maximum number of elements that can be computed in a single computation is derived as:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs KernelIntf1 and KernelIntf2. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from the two GetXxxTmpBufferFactorSize APIs (where Xxx is the two high-level APIs to be called) and the currently available temporary space, the maximum number of elements that can be computed in a single computation, **currentShapeSize**, is derived as:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that currBuff in the above text indicates the space available for API computation, from which spaces such as user input and output need to be excluded. In addition, the maxLiveNodeCount value obtained by the API may be 0, so it is necessary to check that this value is non-zero during computation to avoid division-by-zero errors.

## Function Prototype

```
void GetSignTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameters

**Table 1** Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **typeSize** | Input | Input data type size, in bytes. For example, if the input data type is half, pass 2. |
| **maxLiveNodeCount** | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| **extraBuffer** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** derived from **maxLiveNodeCount** and **extraBuffer** satisfies currentShapeSize \* typeSize < 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

For a complete call example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetSignTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
```

