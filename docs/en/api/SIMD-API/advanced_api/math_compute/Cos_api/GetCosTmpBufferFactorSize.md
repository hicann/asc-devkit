# GetCosTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T14:20:16.289Z -->

## Description

This API is used to obtain **maxLiveNodeCount** and **extraBuf**. With a fixed space size, the maximum number of elements that the operator can compute at a time can be derived from **maxLiveNodeCount** and **extraBuf**. **maxLiveNodeCount** indicates how many times the temporary space is the size of the space occupied by the data computed at a time; **extraBuf** indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the Cos API. Reserve a space of **currBuff** size for it, use the **GetCosTmpBufferFactorSize** API to obtain the output values of **maxLiveNodeCount** and **extraBuf**, and then derive the maximum number of elements that the operator can compute at a time as follows:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-level APIs to be called) and the currently available temporary space, derive the maximum number of elements computed at a time, **currentShapeSize**, as follows:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text indicates the space available for API computation, from which the space for user input and output must be excluded. In addition, the **maxLiveNodeCount** value obtained by the API may be 0. When performing the calculation, check that this value is non-zero to avoid a division-by-zero error.

## Prototype

```
void GetCosTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

```
void GetCosTmpBufferFactorSize(const CosConfig& config, const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **config** | Input | Configuration information related to the Cos API. The configuration of this parameter must be consistent with that of the **config** template parameter of the Cos Kernel API. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| **maxLiveNodeCount** | Output | Maximum number of live nodes, indicating how many times the temporary space is the size of the space occupied by the data volume of a single computation. |
| **extraBuf** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

For the complete calling example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetCosTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
AscendC::CosConfig config;
config.algo = AscendC::CosAlgo::RADIAN_REDUCTION;
AscendC::GetCosTmpBufferFactorSize(config, typeSize, maxLiveNodeCount, extraBuf);
```

