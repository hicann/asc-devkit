# GetAscendAntiQuantTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T11:57:54.416Z -->

## Description

This API obtains **maxLiveNodeCount** and **extraBuf**. With a fixed temporary space size, **maxLiveNodeCount** and **extraBuf** can be used to derive the maximum number of elements computed by the operator in a single pass. **maxLiveNodeCount** indicates how many times the temporary space is the size of the space occupied by the data computed in a single pass; **extraBuf** indicates the size of the additional temporary space used.

The derivation example is as follows:

-   The operator implementation needs to call the **AscendAntiQuant** API. Reserve a space of size **currBuff** for it and use the **GetAscendAntiQuantTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCount** and **extraBuf**, from which the maximum number of elements computed by the operator in a single pass can be derived as:

    **currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCount and extraBuf\) from two **GetXxxTmpBufferFactorSize** APIs (where Xxx represents the two high-level APIs to be called) together with the currently available temporary space, the maximum number of elements computed in a single pass, **currentShapeSize**, is derived as:

    **currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCount1 / typeSize**

    **currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCount2 / typeSize**

    **currentShapeSize = min\(currentShapeSize1, currentShapeSize2\)****

Note that **currBuff** in the preceding text indicates the space available for API computation, from which the user input and output spaces must be excluded. In addition, the **maxLiveNodeCount** value obtained by the API may be 0, so a non-zero check is required during computation to avoid a division-by-zero error.

## Prototype

```
void GetAscendAntiQuantTmpBufferFactorSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose, ge::DataType inputDataType, ge::DataType outputDataType, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input **srcTensor**. |
| scaleShape | Input | Shape information of the input **scale**. |
| isTranspose | Input | Whether to transpose. |
| inputDataType | Input | Input data type, of the **ge::DataType** type. |
| outputDataType | Input | Output data type, of the **ge::DataType** type. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| extraBuf | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

```
std::vector<int64_t> srcDims = { 64, 512 };
auto srcShape = ge::Shape(srcDims);
std::vector<int64_t> scaleDims = { 1, 512 };
auto scaleShape = ge::Shape(scaleDims);
bool isTranspose = false;
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetAscendAntiQuantTmpBufferFactorSize(srcShape, scaleShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCount, extraBuf);
```

