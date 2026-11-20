# GetMeanTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:22:31.286Z pushedAt=2026-11-18T08:54:29.756Z nl=2 -->

## Description

This API is used to obtain maxLiveNodeCnt and extraBuf. With a fixed buffer size, maxLiveNodeCnt and extraBuf can be used to derive the maximum number of elements that can be computed in a single pass. maxLiveNodeCnt indicates how many times the temporary space size is relative to the space occupied by the data of a single computation; extraBuf indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the Mean API. You reserve a buffer of currBuff size for it. By using the GetMeanTmpBufferFactorSize API to obtain the output values maxLiveNodeCnt and extraBuf, you can derive the maximum number of elements that can be computed in a single pass as follows:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCnt / typeSize**

-   The operator implementation needs to call two kernel-side APIs, KernelIntf1 and KernelIntf2. Using the two sets of output values \(maxLiveNodeCnt and extraBuf\) from the two GetXxxTmpBufferFactorSize APIs (where Xxx represents the two high-level APIs to be called), together with the currently available temporary space, you can derive the maximum number of elements currentShapeSize that can be computed in a single call as follows:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCnt1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCnt2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1 , currentShapeSize2\)****

Note that currBuff in the preceding text indicates the space available for API computation, from which the space for user input and output must be excluded.

## Prototype

```
void GetMeanTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameters

**Table 1**  Parameter List

| Parameter | Input/Output | Description |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| extraBuffer | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize \* typeSize derived from maxLiveNodeCount and extraBuffer is less than 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

For the complete call example, see [More Examples](../../math_compute/more_examples.md).

```
uint32_t maxLiveNodeCnt = 0;
uint32_t extraBuf = 0;
AscendC::GetMeanTmpBufferFactorSize(typeSize, maxLiveNodeCnt, extraBuf);
```
