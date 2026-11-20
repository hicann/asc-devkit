# GetPowerTmpBufferFactorSize

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-09T11:16:05.481Z pushedAt=2026-11-18T08:54:29.589Z nl=2 -->

## Description

This API is used to obtain maxLiveNodeCount and extraBuf. Given a fixed space size, you can use maxLiveNodeCount and extraBuf to derive the maximum number of elements that can be computed in a single pass. maxLiveNodeCount indicates how many times the temporary space is the size of the space occupied by the data volume of a single computation; extraBuf indicates the size of the additional temporary space used.

The derivation examples are as follows:

The operator implementation needs to call the Power API. You reserve a buffer of currBuff size for it, and use the GetPowerTmpBufferFactorSize API to obtain the output values maxLiveNodeCount and extraBuf. The maximum number of elements that can be computed in a single pass can then be derived as follows:

**currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCount / typeSize**

Note that currBuff in the preceding text indicates the space available for API computation, from which the space for user input and output must be excluded. In addition, the maxLiveNodeCount value obtained by the API may be 0, so you need to check that this value is not 0 during computation to avoid a division-by-zero error.

## Prototype

```
void GetPowerTmpBufferFactorSize(const bool baseIsTensor, const bool expIsTensor, const bool typeIsInt, const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## Parameters

**Table 1**  Parameter List

| Parameter | Input/Output | Description |
| --- | --- | --- |
| baseIsTensor | Input | Whether the base is a tensor input. Pass true if it is, and false otherwise. |
| expIsTensor | Input | Whether the exponent is a tensor input. Pass true if it is, and false otherwise. |
| typeIsInt | Input | Whether the data type is int. Pass true if it is, and false otherwise. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxLiveNodeCount | Output | Maximum number of live nodes, indicating how many times the temporary space is the space occupied by the data volume of a single computation. |
| extraBuffer | Output | Size of the additional temporary space used, in bytes. |

## Return Value

None

## Constraints

When the currentShapeSize  \* typeSize derived from maxLiveNodeCount and extraBuf is less than 256B, currentShapeSize is rounded up to the value of 256B/typeSize.

## Examples

For the complete call example, see [More Examples](../more_examples.md).

```
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::GetPowerTmpBufferFactorSize(true, true, true, 4, maxLiveNodeCount, extraBuf);
```
