# DataCopyPad (Unaligned Data Transfer from GM to L1)

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T16:17:56.599Z -->

## Applicable Products<a name="section1550532418810"></a>

<a name="table38301303189"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

This API provides non-aligned data transfer from Global Memory to L1 Buffer, allowing developers to fill data as needed.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>

| Parameter | Description |
| --- | --- |
| T | Data type of the operands and **paddingValue** (the data value to be padded). |
| mode | Configures the data transfer mode. It is of the **PaddingMode** type, defined as follows:<br>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // Default mode, consistent with the original data transfer format. Each data transfer is padded to 32-byte alignment.<br>    Compact,     // Compact mode, allowing a single transfer to be unaligned, with padding to 32-byte alignment uniformly at the end of the entire data block.<br>};<br>|

**Table 2** API parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, of the **LocalTensor** type.<br>The start address of **LocalTensor** must be 32-byte aligned. |
| src | Input | Source operand, of the **GlobalTensor** type.<br>The start address of **GlobalTensor** must be 1-byte aligned. |
| dataCopyParams | Input | Transfer parameters.<br>&nbsp;&nbsp;&bull; Of the **DataCopyExtParams** type. For details about its parameters, see [Table 4](#table10572141063919). |
| padParams | Input | When transferring data from Global Memory to Local Memory, data can be padded to the left or right of the transferred data as required by the developer. **padParams** is the parameter used to control the data padding process.<br>&nbsp;&nbsp;&bull; Of the **DataCopyPadExtParams** type. For details about its parameters, see [Table 6](#table844881954715). |

**Table 3** DataCopyExtParams structure parameter definition

<a name="table10572141063919"></a>

| Parameter | Description |
| --- | --- |
| blockCount | Specifies the number of consecutive data blocks to be transmitted by this instruction. The data type is **uint16_t**. Value range: blockCount∈[1, 4095]. |
| blockLen | Specifies the length of each consecutive data block to be transmitted by this instruction. **This instruction supports unaligned transfer**, and **the unit of each consecutive data block length is byte**. The data type is **uint32_t**. Value range: blockLen∈[1, 2097151]. **blockLen must be an integer multiple of sizeof(T)**. |
| srcStride | Source operand, the interval between adjacent consecutive data blocks (the interval between the end of the preceding data block and the start of the following data block), in bytes. Value range: [-blockLen, 2^40-1]. |
| dstStride | Destination operand, the interval between adjacent consecutive data blocks (the interval between the end of the preceding data block and the start of the following data block), in dataBlock (32 bytes). Value range: [0, 65535]. |
| rsv | Reserved field. |

**Table 4** DataCopyPadExtParams<T\> structure parameter definition

<a name="table844881954715"></a>

| Parameter | Description |
| --- | --- |
| isPad | Whether to pad user-defined data. Value range: true, false.<br>&nbsp;&nbsp;&bull; true: Pads the padding value.<br>&nbsp;&nbsp;&bull; false: Indicates that the user does not need to specify a padding value, and random values are padded by default. |
| leftPadding | Data range to be padded on the left side of the consecutive transmit data block, with the unit being the number of elements.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.**<br>**In Compact mode, leftPadding and rightPadding are all ineffective; padding is uniformly applied only at the end of the entire data block to achieve 32-byte alignment.** |
| rightPadding | Data range to be padded on the right side of the consecutive transmit data block, with the unit being the number of elements.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.**<br>**In Compact mode, leftPadding and rightPadding are all ineffective; padding is uniformly applied only at the end of the entire data block to achieve 32-byte alignment.** |
| paddingValue | Data value to be padded on both the left and right sides, which must be within the byte range occupied by the data.<br>The data type is consistent with the source operand, which is the T data type.<br>**When the data type length is 64 bits, this parameter can only be set to 0.** |

The following provides configuration examples for the following scenarios:

- <a name="li1975762118172"></a>Configuration example of the transfer mode
    - Normal mode

        blockLen is 48, and each consecutive transmit data block contains 48 bytes. srcStride is 0 because the logical position of the source operand is GM, and the unit of srcStride is bytes, meaning that adjacent data blocks of the source operand are tightly packed. dstStride is 0 because the logical position of the destination operand is L1 Buffer, and the unit of dstStride is DataBlock (32 bytes), meaning that there is no gap between adjacent data blocks of the destination operand. Note that the data block contains leftPadding/rightPadding data.

        blockLen + leftPadding + rightPadding satisfies 32-byte alignment. isPad is false, so the data values padded on both sides default to random values; otherwise, paddingValue is used. In this example, leftPadding is 0 and rightPadding is 16, so each consecutive transmit data block is padded with 16 bytes on the right side. The total length of the destination operand is 192 bytes.

    - Compact mode

        blockLen is 48, and each consecutive transmit data block contains 48 bytes. srcStride is 0 because the logical position of the source operand is GM, and the unit of srcStride is bytes, meaning that adjacent data blocks of the source operand are tightly packed. dstStride is 0 because the logical position of the destination operand is L1 Buffer, and the unit of dstStride is DataBlock (32 bytes), meaning that adjacent data blocks of the destination operand are tightly packed and no data is padded.

        **In Compact mode, leftPadding and rightPadding are all ineffective**. After the valid data is tightly packed, padding is uniformly added at the end of the entire data block to achieve 32-byte alignment. In this example, blockLen \* blockCount = 48 \* 3 = 144 bytes, and 16 bytes are padded at the end of the entire data block to satisfy 32-byte alignment, so the total length of the destination operation is 160 bytes.

**Figure 1**  Data transfer configuration in Normal mode and Compact mode<a id="fig_paddingmode_demo"></a>

![](../../../../figures/paddingMode.png "Data transfer configuration in Normal mode and Compact mode")

## Data Types

Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The number of bytes of **leftPadding** and **rightPadding** must not exceed 32 bytes.

## Example<a name="section177231425115410"></a>

The example scenario is shown in [Figure 1](#fig_paddingmode_demo). This API is supported only on Ascend 950PR/Ascend 950DT.

The following is an example of calling in Normal mode:

```cpp
constexpr uint32_t srcElemCount = 144;
constexpr uint32_t dstElemCount = 192;

// Source operand: three data blocks stored consecutively in GM, each 48B, 144 int8_t in total.
AscendC::GlobalTensor<int8_t> srcGm;
srcGm.SetGlobalBuffer((__gm__ int8_t *)src, srcElemCount);
// Destination operand: L1 Buffer. In Normal mode, 16B is padded to the right of the 48B valid data in each block, totaling 192B for three blocks.
AscendC::LocalTensor<int8_t> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// Copy three consecutive transmit data blocks.
copyParams.blockCount = 3;
// Each consecutive transmit data block contains 48B valid data.
copyParams.blockLen = 48;
// The source operand is in GM, and its unit is byte; adjacent data blocks are tightly packed with an interval of 0B.
copyParams.srcStride = 0;
// There is no extra interval between adjacent data blocks at destination A1, and the unit of dstStride is DataBlock (32 bytes).
copyParams.dstStride = 0;
// Reserved field. Set it to 0.
copyParams.rsv = 0;

AscendC::DataCopyPadExtParams<int8_t> padParams;
// The padding area uses the default random value, and paddingValue is not used.
padParams.isPad = false;
// No padding on the left. GM data is written directly from the start position of each destination data block.
padParams.leftPadding = 0;
// Pad the right side of each block with 16 int8_t values, that is, 16B. 48B + 16B = 64B, which satisfies 32B alignment.
padParams.rightPadding = 16;
// When isPad is false, this value is ineffective. Fill 0 to keep the field complete.
padParams.paddingValue = static_cast<int8_t>(0);

AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Normal>(dstLocal, srcGm, copyParams, padParams);
```

The following shows an example of calling in Compact mode:

```cpp
constexpr uint32_t srcElemCount = 144;
constexpr uint32_t dstElemCount = 160;

// Source operand: three data blocks are stored consecutively in GM, each 48B, totaling 144 int8_t values.
AscendC::GlobalTensor<int8_t> srcGm;
srcGm.SetGlobalBuffer((__gm__ int8_t *)src, srcElemCount);

// Destination operand: the A1 position of L1 Buffer. In Compact mode, the three blocks of valid data total 144B, and only the last block is padded with 16B on the right.
AscendC::LocalTensor<int8_t> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// Copy three consecutive transmit data blocks.
copyParams.blockCount = 3;
// Each consecutive transmit data block contains 48B of valid data.
copyParams.blockLen = 48;
// The source operand is in GM, and its unit is bytes. Adjacent data blocks are tightly packed with an interval of 0B.
copyParams.srcStride = 0;
// In Compact mode, valid data at the destination is tightly packed, with no padding interval inserted between blocks.
copyParams.dstStride = 0;
// Reserved field. Set it to 0.
copyParams.rsv = 0;

AscendC::DataCopyPadExtParams<int8_t> padParams;
// The padding area uses the default random value, and paddingValue is not used.
padParams.isPad = false;
// When isPad is false, this value is ineffective. Set it to 0 to keep the field complete.
padParams.paddingValue = static_cast<int8_t>(0);

AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Compact>(dstLocal, srcGm, copyParams, padParams);
```
