# GetSwiGLUTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:56:47.778Z -->

## Description

The kernel-side SwiGLU API requires you to reserve/allocate temporary space. The maximum temporary space (**maxTmpBuffer**) and the space occupied by the input (**inputSize** * **typeSize**) have the following relationship:

**maxTmpBuffer = maxLiveNodeCount * inputSize * typeSize + extraBuffer**

Here, **maxLiveNodeCount** indicates how many times the maximum temporary space is relative to the space occupied by the input, and **extraBuffer** indicates the size of the additional temporary space used.

This API is used to obtain **maxLiveNodeCount** and **extraBuffer**. When the space size is fixed, the maximum number of elements that the operator can compute at a time can be derived from **maxLiveNodeCount** and **extraBuffer**.

The following is an example:

The operator implementation needs to call the SwiGLU API. Reserve a space of **currBuff** size for it, use the **GetSwiGLUTmpBufferFactorSize** API to obtain the output values of **maxLiveNodeCount** and **extraBuffer**, and then derive the maximum number of elements that the SwiGLU operator can compute at a time as follows:

**currentShapeSize = (currBuff - extraBuffer) / maxLiveNodeCount / typeSize**

## Prototype

```
void GetSwiGLUTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the maximum temporary space is relative to the space occupied by the input. |
| extraBuffer | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the **currentShapeSize** * **typeSize** derived from **maxLiveNodeCount** and **extraBuffer** is less than 256B, **currentShapeSize** is rounded up to the value of 256B/**typeSize**.

## Examples

```
uint32_t typeSize = sizeof(half);
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuffer = 0;

AscendC::GetSwiGLUTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuffer);
```

