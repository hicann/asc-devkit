# GetSwiGLUTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T06:20:08.314Z pushedAt=2026-11-18T11:07:26.460Z nl=2 -->

## Description

The SwiGLU API on the kernel side requires you to reserve/allocate temporary space during computation. The maximum temporary space (maxTmpBuffer) and the input space (inputSize \* typeSize) have the following relationship:

**maxTmpBuffer = maxLiveNodeCount \* inputSize \* typeSize + extraBuffer**

Here, maxLiveNodeCount indicates how many times the maximum temporary space is the space occupied by the input, and extraBuffer indicates the size of the additional temporary space used.

This API is used to obtain maxLiveNodeCount and extraBuffer. With a fixed space size, you can use maxLiveNodeCount and extraBuffer to computate the maximum number of elements that the operator can compute in a single call.

The following is an example:

The operator implementation needs to call the SwiGLU API. You reserve a space of currBuff size for it, use the GetSwiGLUTmpBufferFactorSize API to obtain the maxLiveNodeCount and extraBuffer output values, and then derive the maximum number of elements that the SwiGLU operator can compute in a single call as follows:

**currentShapeSize = \(currBuff - extraBuffer\) / maxLiveNodeCount / typeSize**

## Prototype

```
void GetSwiGLUTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameters

**Table 1**  Parameter List

| API | Input/Output | Description |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the maximum temporary space is the space occupied by the input. |
| extraBuffer | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize \* typeSize derived from maxLiveNodeCount and extraBuffer is less than 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

```
uint32_t typeSize = sizeof(half);
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuffer = 0;

AscendC::GetSwiGLUTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuffer);
```
