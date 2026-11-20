# GetAcoshTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:02:13.875Z pushedAt=2026-11-18T08:54:29.153Z nl=2 -->

## Description

The computation of the Acosh API on the kernel side requires you to reserve/allocate temporary space. The maximum temporary space (maxTmpBuffer) and the space occupied by the input (inputSize \* typeSize) have the following relationship:

**maxTmpBuffer = maxLiveNodeCount \* inputSize \* typeSize + extraBuffer**

Here, maxLiveNodeCount indicates how many times the maximum temporary space is the space occupied by the input, and extraBuffer indicates the size of the additional temporary space used.

This API is used to obtain maxLiveNodeCount and extraBuffer. With a fixed space size, you can use maxLiveNodeCount and extraBuffer to derive the maximum number of elements that can be computed in a single pass. In addition, the maxLiveNodeCount value obtained by the API may be 0, so you need to check that this value is non-zero during computation to avoid a division-by-zero error.

The following is an example:

The operator implementation needs to call the Acosh API. You reserve a space of currBuff size for it, use the GetAcoshTmpBufferFactorSize API to obtain the output values maxLiveNodeCount and extraBuffer, and then derive the maximum number of elements that the Acosh operator can compute in a single call as follows:

**currentShapeSize = \(currBuff - extraBuffer\) / maxLiveNodeCount / typeSize**

## Prototype

```
void GetAcoshTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
```

## Parameters

**Table 1**  Parameter List

| Parameter | Input/Output | Description |
| --- | --- | --- |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the maximum temporary space is the space occupied by the input. |
| extraBuf | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize  \* typeSize derived from maxLiveNodeCount and extraBuf is less than 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

```
// Obtain the maxLiveNodeCount and extraBuffer of the Acosh operation with the half input type.
uint32_t typeSize = sizeof(half);
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuffer = 0;

AscendC::GetAcoshTmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuffer);
```
