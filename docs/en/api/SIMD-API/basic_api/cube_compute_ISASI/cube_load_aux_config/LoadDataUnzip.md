# LoadDataUnzip<a name="ZH-CN_TOPIC_0000001913015096"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:46:15.212Z pushedAt=2026-10-26T07:41:44.135Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/LoadDataUnzip_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Decompresses data on GM and moves it to the L1 Buffer (A1/B1) or L0B Buffer (B2). Before executing this API, execute [LoadUnzipIndex](LoadUnzipIndex.md) to load the compressed index table.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadDataUnzip(const LocalTensor<T>& dst, const GlobalTensor<T>& src)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Destination operand, of type **LocalTensor**. Supported **TPosition** values are **A1**/**B1**/**B2**.<br>The start address of **LocalTensor** must satisfy the following requirements:<br>&nbsp;&nbsp;&bull; When **TPosition** is **A1**/**B1**, it must be 32-byte aligned.<br>&nbsp;&nbsp;&bull; When **TPosition** is **B2**, it must be 512B aligned. |
| src | Input | Source operand, of type **GlobalTensor**. The data type must be consistent with that of **dst**. |

## Data Types

The supported data type is **int8_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section6461234123118"></a>

The example runs on the Atlas inference products AI Core.

```cpp
uint32_t srcLen = 896, dstLen = 1024, numOfIndexTabEntry = 1;
AscendC::LocalTensor<int8_t> weightB1 = inQueueB1.AllocTensor<int8_t>();
AscendC::LoadUnzipIndex(indexGlobal, numOfIndexTabEntry); // Load the index data, loading the compressed index table from GM into internal registers.
AscendC::LoadDataUnzip(weightB1, weGlobal); // Load data according to the index table in the internal registers.
```
