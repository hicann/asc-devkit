# GetPowerTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T09:11:23.714Z -->

## Description

This API obtains **maxLiveNodeCount** and **extraBuf**. Given a fixed space size, **maxLiveNodeCount** and **extraBuf** can be used to derive the maximum number of elements that an operator can compute in a single computation. **maxLiveNodeCount** indicates how many times larger the temporary space is than the space occupied by the data of a single computation; **extraBuf** indicates the size of the additional temporary space used.

The derivation example is as follows:

The operator implementation needs to call the **Power** API. You reserve a space of **currBuff** size for it, and use the **GetPowerTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCount** and **extraBuf**. The maximum number of elements that the operator can compute in a single computation can then be derived as:

**currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

Note that **currBuff** in the preceding text indicates the space available for API computation, from which the space for user input and output must be excluded. In addition, the value of **maxLiveNodeCount** obtained by the API may be 0. When computing, you must check that this value is not 0 to avoid a division-by-zero error.

## Prototype

```
void GetPowerTmpBufferFactorSize(const bool baseIsTensor, const bool expIsTensor, const bool typeIsInt, const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **baseIsTensor** | Input | Whether the base is a tensor. Pass true if the base is a tensor; otherwise, pass false. |
| **expIsTensor** | Input | Whether the exponent is a tensor. Pass true if the exponent is a tensor; otherwise, pass false. |
| **typeIsInt** | Input | Whether the data type is int. Pass true if the data type is int; otherwise, pass false. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2. |
| **maxLiveNodeCount** | Output | Maximum number of live nodes, indicating the ratio of the temporary space to the space occupied by the data volume of a single computation. |
| **extraBuffer** | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize**  \* **typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

For a complete example, see [More Samples](../more_samples_83.md).

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetPowerTmpBufferFactorSize(true, true, true, 4, maxLiveNodeCount, extraBuf);
```

