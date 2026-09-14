# GetSinhTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T10:17:32.908Z -->

## Description

This API obtains **maxLiveNodeCount** and **extraBuf**. Given a fixed space size, **maxLiveNodeCount** and **extraBuf** can be used to derive the maximum number of elements that an operator can compute in a single computation. **maxLiveNodeCount** indicates the ratio of the temporary space to the space occupied by the data computed in a single computation; **extraBuf** indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the **Sinh** API. The developer reserves a space of **currBuff** size for it, and uses the **GetSinhTmpBufferFactorSize** API to obtain the output values of **maxLiveNodeCount** and **extraBuf**. The maximum number of elements that the operator can compute in a single computation can then be derived as:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx is the two high-level APIs to be called), together with the currently available temporary space, the maximum number of elements computed in a single computation, **currentShapeSize**, is derived as:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text indicates the space available for API computation, excluding the space for user input and output. In addition, the **maxLiveNodeCount** value obtained by the API may be 0. When computing, check that this value is non-zero to avoid a division-by-zero error.

## Prototype

```
void GetSinhTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1** Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the size of the data volume for a single computation. |
| extraBuf | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When **currentShapeSize * typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/typeSize.

## Examples

For a complete call example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetSinhTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
```

