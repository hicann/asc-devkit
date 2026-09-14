# GetDequantizeTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T12:06:49.630Z -->

## Description

This API is used to obtain **maxLiveNodeCount** and **extraBuf**. With a fixed space size, the maximum number of elements that the operator can compute at a time can be derived from **maxLiveNodeCount** and **extraBuf**. **maxLiveNodeCount** indicates how many times the temporary space is the size of the space occupied by the data volume of a single computation; **extraBuf** indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the Dequantize API. Reserve a space of **currBuff** size for it, use the **GetDequantizeTmpBufferFactorSize** API to obtain the output values of **maxLiveNodeCount** and **extraBuf**, and then derive the maximum number of elements that the operator can compute at a time as follows:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-level APIs to be called) and the currently available temporary space, the maximum number of elements **currentShapeSize** that can be computed at a time is derived as follows:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text indicates the space available for API computation, from which the user input and output spaces must be excluded. In addition, the **maxLiveNodeCount** value obtained by the API may be 0. When performing the computation, check that this value is not 0 to avoid a division-by-zero error.

## Prototype

```
void GetDequantizeTmpBufferFactorSize(const ge::Shape& srcShape, uint32_t& maxLiveNodeCnt, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input **srcTensor**. |
| **maxLiveNodeCount** | Output | Maximum number of live nodes, indicating how many times the temporary space is the size of the space occupied by the data volume of a single computation. |
| **extraBuf** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

```
std::vector<int64_t> shapeVec = {2, 1024};
ge::Shape srcShape(shapeVec);
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetDequantizeTmpBufferFactorSize(srcShape, maxLiveNodeCount, extraBuf);
```

