# GetAscendQuantTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T06:37:46.268Z pushedAt=2026-11-18T11:07:26.898Z nl=2 -->

## Description

This API is used to obtain maxLiveNodeCount and extraBuf. Given a fixed space size, you can use maxLiveNodeCount and extraBuf to derive the maximum number of elements that can be computed in a single pass. maxLiveNodeCount indicates how many times the temporary space is the size of the space occupied by the data volume of a single computation; extraBuf indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the AscendQuant API. You reserve a space of currBuff size for it, and use the GetAscendQuantTmpBufferFactorSize API to obtain the output values maxLiveNodeCount and extraBuf. The maximum number of elements that the operator can compute in a single pass can then be derived as:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs, KernelIntf1 and KernelIntf2. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from the two GetXxxTmpBufferFactorSize APIs (where Xxx represents the two high-order APIs to be called) and the currently available temporary space, derive the maximum number of elements that can be computed in a single pass, currentShapeSize, as follows:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that currBuff in the preceding text indicates the space available for API computation, from which the space for user input and output must be excluded. In addition, the maxLiveNodeCount value obtained by the API may be 0, so you need to check that this value is not 0 during computation to avoid a division-by-zero error.

## Prototype

```
void GetAscendQuantTmpBufferFactorSize(uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter List

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| extraBuf | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize  \* typeSize derived from maxLiveNodeCount and extraBuf is less than 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetAscendQuantTmpBufferFactorSize(maxLiveNodeCount, extraBuf);
```
