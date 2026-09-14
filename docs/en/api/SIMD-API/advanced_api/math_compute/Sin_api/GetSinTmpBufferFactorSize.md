# GetSinTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T09:56:39.053Z -->

## Function

This API is used to obtain **maxLiveNodeCount** and **extraBuf**. Given a fixed space size, **maxLiveNodeCount** and **extraBuf** can be used to derive the maximum number of elements that an operator can compute at a time. **maxLiveNodeCount** indicates the ratio of the temporary space to the space occupied by the data volume of a single computation; **extraBuf** indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the Sin API. The developer reserves space of size currBuff for it, and uses the **GetSinTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCount** and **extraBuf**. The maximum number of elements that the operator can compute at a time can be derived as follows:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs KernelIntf1 and KernelIntf2. Using the two sets of output values \(**maxLiveNodeCount** and **extraBuf**\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-order APIs to be called) and the currently available temporary space, the maximum number of elements that can be computed at a time, **currentShapeSize**, is derived as follows:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that currBuff in the preceding text indicates the space available for API computation, from which the user input and output spaces need to be excluded. In addition, the **maxLiveNodeCount** value obtained by the API may be 0. When performing the calculation, you need to check that this value is not 0 to avoid division by zero.

## Prototype

```
void GetSinTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

```
void GetSinTmpBufferFactorSize(const SinConfig& config, const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1** Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **config** | Input | Related configuration information of the Sin API. The configuration of this parameter must be consistent with that of the **config** template parameter of the Sin Kernel API. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2. |
| **maxLiveNodeCount** | Output | Maximum number of live nodes, indicating the ratio of the temporary space to the space occupied by the data volume of a single computation. |
| **extraBuf** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize  \* typeSize derived from maxLiveNodeCount and extraBuf is less than 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

For the complete call example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetSinTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
AscendC::SinConfig config;
config.algo = AscendC::SinAlgo::RADIAN_REDUCTION;
AscendC::GetSinTmpBufferFactorSize(config, typeSize, maxLiveNodeCount, extraBuf);
```

