# GetGeGLUTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T11:06:29.695Z pushedAt=2026-09-09T07:16:16.876Z -->

## Description

The kernel-side GeGLU API requires you to reserve/apply for temporary space. The maximum temporary space (**maxTmpBuffer**) and the space occupied by the input (**inputSize** \* **typeSize**) have the following relationship:

**maxTmpBuffer = maxLiveNodeCount \* inputSize \* typeSize + extraBuf**

Here, **maxLiveNodeCount** indicates how many times the maximum temporary space is relative to the space occupied by the input, and **extraBuf** indicates the size of the additional temporary space used.

This API is used to obtain **maxLiveNodeCount** and **extraBuf**. When the space size is fixed, the maximum number of elements that the operator can compute at a time can be derived from **maxLiveNodeCount** and **extraBuf**.

The following is an example:

The operator implementation needs to call the GeGLU API. Reserve a space of **currBuff** size for it, use the **GetGeGLUTmpBufferFactorSize** API to obtain the output values of **maxLiveNodeCount** and **extraBuf**, and then derive the maximum number of elements that the GeGLU operator can compute at a time as follows:

**currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

## Prototype

```
void GetGeGLUTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1** Parameter list

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
uint32_t typeSize = sizeof(half);
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;

AscendC::GetGeGLUTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
```

