# GetAntiQuantizeTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T11:54:40.292Z -->

## Description

This API obtains **maxLiveNodeCount** and **extraBuf**. Given a fixed space size, the maximum number of elements that the operator can compute at a time can be derived from **maxLiveNodeCount** and **extraBuf**. **maxLiveNodeCount** indicates how many times the temporary space is the size of the space occupied by the data volume of a single computation; **extraBuf** indicates the size of the additional temporary space used.

The derivation examples are as follows:

-   The operator implementation needs to call the **AntiQuantize** API. Reserve a space of size **currBuff** for it and use the **GetAntiQuantizeTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCount** and **extraBuf**. The maximum number of elements that the operator can compute at a time can then be derived as:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-level APIs to be called) and the currently available temporary space, the maximum number of elements **currentShapeSize** that can be computed at a time is derived as:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text represents the space available for API computation, from which the user input and output spaces must be excluded. In addition, the **maxLiveNodeCount** value obtained by the API may be 0. When performing the calculation, check that this value is non-zero to avoid a division-by-zero error.

## Prototype

```
void GetAntiQuantizeTmpBufferFactorSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, ge::DataType inputDataType, ge::DataType outputDataType, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input **srcTensor**. |
| **scaleShape** | Input | Shape information of the input **scale**. |
| **inputDataType** | Input | Input data type, which is the **ge::DataType** type. |
| **outputDataType** | Input | Output data type, which is the **ge::DataType** type. |
| **maxLiveNodeCount** | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| **extraBuf** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
std::vector<int64_t> srcDims = { 64, 512 };
auto srcShape = ge::Shape(srcDims);
std::vector<int64_t> scaleDims = { 1, 512 };
auto scaleShape = ge::Shape(scaleDims);
bool isTranspose = false;
AscendC::GetAntiQuantizeTmpBufferFactorSize(srcShape, scaleShape, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCount, extraBuf);
```

