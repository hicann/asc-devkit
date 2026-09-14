# GetAscendDequantTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T12:00:55.547Z -->

## Description

This API is used to obtain **maxLiveNodeCount** and **extraBuf**. With a fixed buffer size, the maximum number of elements that can be computed in a single operator invocation can be derived from **maxLiveNodeCount** and **extraBuf**. **maxLiveNodeCount** indicates how many times the temporary buffer size is the size of the space occupied by the data computed in a single call, and **extraBuf** indicates the size of the additional temporary buffer used.

The derivation examples are as follows:

-   The operator implementation needs to call the **AscendDequant** API. The developer reserves a buffer of size **currBuff** for it and uses the **GetAscendDequantTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCount** and **extraBuf**. The maximum number of elements that can be computed in a single operator invocation can then be derived as:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-level APIs to be called) and the currently available temporary buffer, the maximum number of elements that can be computed in a single invocation, **currentShapeSize**, is derived as:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text represents the space available for API computation, from which the space for user input and output must be excluded. In addition, the **maxLiveNodeCount** value obtained by the API may be 0. When performing the calculation, check that this value is non-zero to avoid a division-by-zero error.

## Prototype

```
void GetAscendDequantTmpBufferFactorSize(const ge::Shape& srcShape, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input **srcTensor**. |
| **maxLiveNodeCount** | Output | Maximum number of live nodes, indicating how many times the temporary space is relative to the space occupied by the data volume of a single computation. |
| **extraBuf** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

```
std::vector<int64_t> shape_vec = {2, 1024};
ge::Shape srcShape(shape_vec);
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetAscendDequantTmpBufferFactorSize(maxLiveNodeCount, extraBuf);
```

