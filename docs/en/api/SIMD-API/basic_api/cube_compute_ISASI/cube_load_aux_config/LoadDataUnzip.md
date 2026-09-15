# LoadDataUnzip<a name="ZH-CN_TOPIC_0000001913015096"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:37:29.126Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

Decompresses the data on **GM** and moves it to **A1**/**B1**/**B2**. Before executing this API, call [LoadUnzipIndex](LoadUnzipIndex.md) to load the compression index table.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadDataUnzip(const LocalTensor<T>& dst, const GlobalTensor<T>& src)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| dst | Output | Destination operand, of type **LocalTensor**, with supported **TPosition** values of A1/B1/B2.<br>The start address of **LocalTensor** must meet the following requirements:<br>&nbsp;&nbsp;&bull; When **TPosition** is A1/B1, it must be 32-byte aligned.<br>&nbsp;&nbsp;&bull; When **TPosition** is B2, it must be 512B aligned. |
| src | Input | Source operand, of type **GlobalTensor**. The data type must be consistent with that of **dst**. |

## Data Types

The supported data type is **int8_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section6461234123118"></a>

The running platform supported by this example is the AI Core of Atlas inference products.

```cpp
uint32_t srcLen = 896, dstLen = 1024, numOfIndexTabEntry = 1;
AscendC::LocalTensor<int8_t> weightB1 = inQueueB1.AllocTensor<int8_t>();
AscendC::LoadUnzipIndex(indexGlobal, numOfIndexTabEntry); // Load the index data, and load the compressed index table on the GM to the internal registers.
AscendC::LoadDataUnzip(weightB1, weGlobal); // Load data based on the index table in the internal registers.
```
