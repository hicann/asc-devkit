# GetAcoshTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T12:34:27.786Z -->

## Description

The kernel-side Acosh API requires you to reserve/apply for temporary space. The maximum temporary space (**maxTmpBuffer**) and the space occupied by the input (**inputSize** \* **typeSize**) have the following relationship:

**maxTmpBuffer = maxLiveNodeCount \* inputSize \* typeSize + extraBuffer**

Here, **maxLiveNodeCount** indicates how many times the maximum temporary space is relative to the space occupied by the input, and **extraBuffer** indicates the size of the additional temporary space used.

This API is used to obtain **maxLiveNodeCount** and **extraBuffer**. With a fixed space size, the maximum number of elements that the operator can compute at a time can be derived from **maxLiveNodeCount** and **extraBuffer**. In addition, the value of **maxLiveNodeCount** obtained by this API may be 0. When performing the calculation, check that this value is non-zero to avoid a division-by-zero error.

The following is an example:

The operator implementation needs to call the Acosh API. Reserve a space of **currBuff** size for it, use the **GetAcoshTmpBufferFactorSize** API to obtain the output values of **maxLiveNodeCount** and **extraBuffer**, and then derive the maximum number of elements that the Acosh operator can compute at a time as follows:

**currentShapeSize = (currBuff - extraBuffer) / maxLiveNodeCount / typeSize**

## Prototype

```
void GetAcoshTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the maximum temporary space is relative to the space occupied by the input. |
| extraBuf | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** \* **typeSize** derived from **maxLiveNodeCount** and **extraBuf** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

```
// Obtain maxLiveNodeCount and extraBuffer for the Acosh operation with the input type half.
uint32_t typeSize = sizeof(half);
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuffer = 0;

AscendC::GetAcoshTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuffer);
```

