# LoadUnzipIndex<a name="ZH-CN_TOPIC_0000001913174992"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:26:20.436Z -->

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

Loads the compressed index tables on GM into internal registers.

The index table contains the compression information required by **LoadDataUnzip**, such as the compression length, which is used to obtain the compressed data.

The index table is generated offline by the compression tool based on the corresponding weight data. A single **LoadUnzipIndex** instruction can load multiple index tables, whereas each **LoadDataUnzip** instruction can consume only one index table. Therefore, the order of the index tables should be determined by the user to ensure their correspondence with the compressed data.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = int8_t, typename Std::enable_if<Std::is_same<PrimT<T>, int8_t>::value, bool>::type = true> 
__aicore__ inline void LoadUnzipIndex(const GlobalTensor<T>& src, uint32_t numOfIndexTabEntry)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of **src**.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull;When **src** uses a basic data type, its data type must be **int8_t**; otherwise, compilation fails.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull;When **src** uses the **TensorTrait** type, the **LiteType** of the **src** data type **T** must be **int8_t**; otherwise, compilation fails.<br>The last template parameter is used only for the data type check described above and does not require user attention. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand, the index table address, of the **GlobalTensor** type.<br>&nbsp;&nbsp;&bull; The **src** address must be 2-byte aligned.<br>&nbsp;&nbsp;&bull; The **src** length must be an integer multiple of 512 bytes, with a maximum of 32 KB. |
| numOfIndexTabEntry | Input | Input data, indicating the number of index tables to load.<br>&nbsp;&nbsp;&bull;The number of index tables must be greater than 0. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- **LoadUnzipIndex** must be executed before any **LoadDataUnzip** instruction.
- The number of index tables loaded by **LoadUnzipIndex** must be greater than or equal to the number of times the **LoadDataUnzip** instruction is executed.

## Example<a name="section6461234123118"></a>

The running platform supported by this example is the Atlas inference products AI Core. For detailed examples, see [LoadDataUnzip](LoadDataUnzip.md).

```cpp
indexGlobal.SetGlobalBuffer((__gm__ int8_t*)indexGm);
AscendC::LoadUnzipIndex(indexGlobal, numOfIndexTabEntry);
```
