# DataCopyPad (GMToL1 Unaligned Data Copy)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T03:13:22.629Z pushedAt=2026-11-18T01:25:06.699Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<a name="table38301303189"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopyPad_GMToL1_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

This API provides unaligned data movement from Global Memory to L1 Buffer, and allows developers to fill data as needed.

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
| T | Data type of the operands and **paddingValue** (the data value to be filled). |
| mode | Configures the data copy mode. It is of the PaddingMode type, defined as follows:<br>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // Default mode, consistent with the original data copy format. Each data copy is padded to 32-byte alignment.<br>    Compact,     // Compact mode, allowing a single copy to be unaligned, with padding uniformly applied to 32-byte alignment at the end of the entire data block.<br>}; |

**Table 2** API parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, of the type **LocalTensor**.<br>The start address of **LocalTensor** must be 32-byte aligned. |
| src | Input | Source operand, of type GlobalTensor.<br>The start address of GlobalTensor must be 1-byte aligned. |
| dataCopyParams | Input | Copy parameters.<br>&nbsp;&nbsp;&bull; Of type **DataCopyExtParams**. For details about the parameters, see [Table 4](#table10572141063919). |
| padParams | Input | When copying data from Global Memory to Local Memory, you can pad data on the left or right side of the copied data as needed. padParams is the parameter used to control the data padding process.<br>&nbsp;&nbsp;&bull; Type: DataCopyPadExtParams. For details about the parameters, see [Table 6](#table844881954715). |

**Table 3** DataCopyExtParams structure parameter definition

<a name="table10572141063919"></a>

| Parameter | Meaning |
| --- | --- |
| blockCount | Specifies the number of consecutive transferred data blocks contained in this instruction. Data type: uint16_t. Value range: blockCount∈[1, 4095]. |
| blockLen | Specifies the length of each consecutively transmitted data block in this instruction. **This instruction supports unaligned copy**, and **the length of each consecutively transmitted data block is in bytes**. Data type: uint32_t. Value range: blockLen∈[1, 2097151]. **blockLen must be an integer multiple of sizeof(T)**. |
| srcStride | Source operand. The interval between adjacent contiguous data blocks (the interval between the end of the preceding data block and the start of the following data block), in bytes. Value range: [-blockLen, 2^40-1]. |
| dstStride | Destination operand. The interval between adjacent contiguous data blocks (the interval between the end of the preceding data block and the start of the following data block), in dataBlock (32 bytes). Value range: [0, 65535]. |
| rsv | Reserved field. |

**Table 4** Parameter Definition of the DataCopyPadExtParams<T\> Structure

<a name="table844881954715"></a>

| Parameter | Meaning |
| --- | --- |
| isPad | Whether to fill user-defined data. Value range: true, false.<br>&nbsp;&nbsp;&bull; true: Fill the padding value.<br>&nbsp;&nbsp;&bull; false: The user does not need to specify a padding value, and a random value is filled by default. |
| leftPadding | Data range to be padded on the left side of the consecutively transmitted data block, in elements.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.**<br>**In Compact mode, leftPadding and rightPadding do not take effect, and the data is padded only at the end of the entire data block to 32-byte alignment.** |
| rightPadding | Data range to be padded on the right side of the consecutively transmitted data block, in elements.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.**<br>**In Compact mode, leftPadding and rightPadding do not take effect, and the data is padded only at the end of the entire data block to 32-byte alignment.** |
| paddingValue | Data value to be padded on the left and right sides, which must be within the byte range occupied by the data.<br>The data type is the same as that of the source operand, of type T.<br>**When the data type length is 64 bits, this parameter can only be set to 0.** |

The following provides configuration examples for the following scenarios:

- <a name="li1975762118172"></a>Configuration example of the movement mode
    - Normal mode

        blockLen is 48, and each consecutively transmitted data block contains 48 bytes. srcStride is 0 because the logical position of the source operand is GM, and the unit of srcStride is bytes, meaning that adjacent data blocks of the source operand are tightly packed. dstStride is 0 because the logical position of the destination operand is L1 Buffer, and the unit of dstStride is DataBlock (32 bytes), meaning that there is no gap between adjacent data blocks of the destination operand. Note that the data block contains leftPadding/rightPadding data.

        blockLen + leftPadding + rightPadding satisfies 32-byte alignment. isPad is false, and the data values padded on the left and right sides default to random values; otherwise, they are paddingValue. In this example, leftPadding is 0 and rightPadding is 16, so each consecutively transmitted data block is padded with 16 bytes on the right side. The total length of the destination operand is 192 bytes.

    - Compact mode

        blockLen is 48, and each consecutively transmitted data block contains 48 bytes. srcStride is 0 because the logical position of the source operand is GM, and the unit of srcStride is bytes, meaning that adjacent data blocks of the source operand are tightly packed. dstStride is 0 because the logical position of the destination operand is L1 Buffer, and the unit of dstStride is DataBlock (32 bytes), meaning that adjacent data blocks of the destination operand are tightly packed and no data is padded.

        **In Compact mode, leftPadding and rightPadding do not take effect.** After the valid data is tightly packed, padding is uniformly added at the end of the entire data block to achieve 32-byte alignment. In this example, blockLen \* blockCount = 48 \* 3 = 144 bytes, and 16 bytes are padded at the end of the entire data block to satisfy 32-byte alignment, so the total length of the destination operand is 160 bytes.

**Figure 1**  Normal mode and Compact mode transfer configuration diagram<a id="fig_paddingmode_demo"></a>

![](../../../../figures/paddingMode.png "Normal mode and Compact mode transfer configuration diagram")

## Data Types

Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The number of bytes of **leftPadding** and **rightPadding** must not exceed 32 bytes.

## Example<a name="section177231425115410"></a>

The example scenario is shown in [Figure 1](#fig_paddingmode_demo). This API is supported only on the Ascend 950PR&950DT products.

The Normal mode call example is as follows:

```cpp
constexpr uint32_t srcElemCount = 144;
constexpr uint32_t dstElemCount = 192;

// Source operand: three data blocks stored contiguously in GM, each 48B, totaling 144 int8_t.
AscendC::GlobalTensor<int8_t> srcGm;
srcGm.SetGlobalBuffer((__gm__ int8_t *)src, srcElemCount);
// Destination operand: L1 Buffer. In Normal mode, 16B is padded to the right of the 48B valid data in each block, totaling 192B for three blocks.
AscendC::LocalTensor<int8_t> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// Copy three consecutive transmit data blocks.
copyParams.blockCount = 3;
// Each consecutive transmit data block contains 48B of valid data.
copyParams.blockLen = 48;
// The source operand is in GM, and the unit is bytes; adjacent data blocks are tightly packed with an interval of 0B.
copyParams.srcStride = 0;
// There is no extra interval between adjacent data blocks at destination A1, and the unit of dstStride is DataBlock (32 bytes).
copyParams.dstStride = 0;
// Reserved field, fixed to 0.
copyParams.rsv = 0;

AscendC::DataCopyPadExtParams<int8_t> padParams;
// The padding area uses the default random value, and paddingValue is not used.
padParams.isPad = false;
// No padding on the left; GM data is written directly from the start position of each destination data block.
padParams.leftPadding = 0;
// Pad 16 int8_t values on the right of each block, that is, 16B; 48B + 16B = 64B, satisfying 32B byte alignment.
padParams.rightPadding = 16;
// This value does not take effect when isPad = false. Fill 0 to keep the field complete.
padParams.paddingValue = static_cast<int8_t>(0);

AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Normal>(dstLocal, srcGm, copyParams, padParams);
```

The following is an example of calling in Compact mode:

```cpp
constexpr uint32_t srcElemCount = 144;
constexpr uint32_t dstElemCount = 160;

// Source operand: three data blocks stored consecutively in GM, each 48B, totaling 144 int8_t values.
AscendC::GlobalTensor<int8_t> srcGm;
srcGm.SetGlobalBuffer((__gm__ int8_t *)src, srcElemCount);

// Destination operand: position A1 of the L1 Buffer. In Compact mode, the three blocks contain 144B of valid data in total, padded with 16B only on the right of the last block.
AscendC::LocalTensor<int8_t> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// Move three consecutive transmit data blocks.
copyParams.blockCount = 3;
// Each consecutive transmit data block contains 48B of valid data.
copyParams.blockLen = 48;
// Source operand is in GM, and the unit is bytes; adjacent data blocks are tightly packed with an interval of 0B.
copyParams.srcStride = 0;
// In Compact mode, valid data on the destination side is tightly packed, with no padding interval inserted between data blocks.
copyParams.dstStride = 0;
// Reserved field. Fill 0.
copyParams.rsv = 0;

AscendC::DataCopyPadExtParams<int8_t> padParams;
// The padding area uses the default random value instead of paddingValue.
padParams.isPad = false;
// When isPad is false, this value does not take effect. Fill 0 to keep the field complete.
padParams.paddingValue = static_cast<int8_t>(0);

AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Compact>(dstLocal, srcGm, copyParams, padParams);
```
