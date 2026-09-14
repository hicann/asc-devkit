# GetXorTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T10:59:10.622Z -->

## Description

This API obtains **maxLiveNodeCount** and **extraBuf**. Given a fixed buffer size, you can use **maxLiveNodeCount** and **extraBuf** to derive the maximum number of elements that the operator can compute in a single pass. **maxLiveNodeCount indicates how many times the temporary buffer size exceeds the space occupied by the data volume of a single computation**, and **extraBuf** indicates the size of the additional temporary buffer used.

The derivation examples are as follows:

-   The operator implementation needs to call the **Xor** API. You reserve a buffer of **currBuff** size for it and use the **GetXorTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCount** and **extraBuf**. The maximum number of elements that the operator can compute in a single pass is derived as follows:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs, **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from two **GetXxxTmpBufferFactorSize** APIs (where Xxx is the two high-level APIs to be called) and the currently available temporary buffer, the maximum number of elements for a single computation, that is, **currentShapeSize**, is derived as:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text indicates the space available for API computation, from which the user input and output spaces must be excluded. In addition, the **maxLiveNodeCount** value obtained by the API may be 0. When performing the computation, check that this value is not 0 to avoid a division-by-zero error.

## Prototype

```
void GetXorTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameter Description

**Table 1** Parameter list

| Parameter Name | Input/Output | Function |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the size of the space occupied by a single computation's data volume. |
| extraBuffer | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLiveNodeCount** and **extraBuffer** is less than 256B, **currentShapeSize** is rounded up to the value of 256B / **typeSize**.

## Examples

For a complete call example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetXorTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
```

