# UB-to-GM Unaligned Data Transfer (DataCopyPad)<a name="ZH-CN_TOPIC_0000001894460401"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T13:50:35.157Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id17 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id17 -->
<!-- npu="A3" id18 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id18 -->
<!-- npu="910b" id19 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id19 -->
<!-- npu="310b" id20 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas inference products AI Core: Not supported
<!-- end id21 -->
<!-- npu="310p" id22 -->
- Atlas inference products Vector Core: Not supported
<!-- end id22 -->
<!-- npu="910" id23 -->
- Atlas training products: Not supported
<!-- end id23 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

This API provides the function of copying data from the Unified Buffer to Global Memory in an unaligned manner.

The supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Unified Buffer -> Global Memory
    - VECIN -> GM
    - VECOUT -> GM

- As shown in [Figure 1](#fig_datacopypad3), in the 32-byte aligned copy scenario, all data read from the Unified Buffer is copied to Global Memory.
- As shown in [Figure 2](#fig_datacopypad4), in the non-32-byte aligned scenario, dummy data is filled in when reading Unified Buffer data to align it to 32B, and the dummy data is discarded when copying to Global Memory, thereby implementing unaligned copy from the Unified Buffer to Global Memory.

## Prototype<a name="section620mcpsimp"></a>

- Configuring the data copy mode (mode) is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- npu="950" id1 -->
- Configuring the data copy mode (mode) is supported (only Ascend 950PR/Ascend 950DT support this).

    ```cpp
    // This function prototype is supported only on Ascend 950PR/Ascend 950DT.
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```
<!-- end id1 -->

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For the data types supported by each product, see [Data Type](#section4219135304818). |
| mode | Configures the data copy mode. The type is PaddingMode, defined as follows:<br>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // Default mode, consistent with the original data copy format. Each data copy is padded to 32-byte alignment.<br>    Compact,     // Compact mode, allowing a single copy to be unaligned. Padding to 32-byte alignment is performed uniformly at the end of the entire data block.<br>};<br>|

**Table 2** API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md) type.<br>The start address has no address alignment constraint. |
| src | Input | Source operand, of the [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md) type.<br>The start address must be 32-byte aligned. |
| dataCopyParams | Input | Copy parameters, of the DataCopyExtParams type. For details about the parameters, see [Table 3](#table_ub2gm_pad_3). |

For the definitions of the structure parameters listed in the following table, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after CANN software installation.

**Table 3**  DataCopyExtParams structure parameter definition<a name="table_ub2gm_pad_3"></a>

| Parameter | Description |
| :--- | :--- |
| blockCount | Specifies the number of consecutive transferred data blocks contained in this instruction. The data type is uint16_t, and the value range is blockCount∈[0, 4095]. |
| blockLen | Specifies the length of each consecutive transferred data block in this instruction. **This instruction supports unaligned copy**, and **the length of each consecutive transferred data block is in bytes**. The data type is uint32_t, and the value range is blockLen∈[0, 2097151]**. blockLen must be an integer multiple of sizeof(T), and must not exceed the UB space size**. |
| srcStride | Source operand, the interval between adjacent consecutive data blocks (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block).<br>The logical position of the source operand is VECIN/VECOUT, and the unit is dataBlock (32 bytes).<br>The data type is uint32_t, and the value range is [0, 2^32-1]. The data type and supported value range of srcStride may differ across products. For details, see [Constraints](#section633mcpsimp). |
| dstStride | Destination operand, the interval between adjacent consecutive data blocks (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block).<br>The logical position of the destination operand is GM, and the unit is byte.<br>The data type is uint32_t, and the value range is [0, 2^32-1]. The data type and supported value range of dstStride may differ across products. For details, see [Constraints](#section633mcpsimp). |
| rsv | Reserved field. |

The following two scenarios describe the unaligned copy from Unified Buffer to Global Memory, corresponding to 32-byte alignment and non-32-byte alignment respectively:

- 32-byte alignment scenario<a name="32-byte-alignment-scenario"></a>

    As shown in [Figure 1](#fig_datacopypad3), blockLen is 64, and each contiguous transferred data block contains 64 bytes. srcStride is 1, the logical position of the source operand is VECIN/VECOUT, and the unit of srcStride is dataBlock (32 bytes), meaning that adjacent data blocks of the source operand are separated by 1 dataBlock. dstStride is 1, the logical position of the destination operand is GM, and the unit of dstStride is byte, meaning that adjacent data blocks of the destination operand are separated by 1 byte.

    For the 32-byte-aligned copy scenario, all data read from the Unified Buffer is copied to Global Memory.

    **Figure 1**  Schematic diagram of unaligned copy from Unified Buffer to Global Memory when blockLen is 32-byte aligned<a name="fig_datacopypad3"></a>  
    ![](../../../../figures/datacopypad3.png)

- Non-32-byte-aligned scenario<a name="non-32-byte-aligned-scenario"></a>

    As shown in [Figure 2](#fig_datacopypad4), blockLen is 47, and each contiguous transferred data block contains 47 bytes, which does not satisfy 32-byte alignment. srcStride is 1, meaning that adjacent data blocks of the source operand are separated by 1 dataBlock. dstStride is 1, meaning that adjacent data blocks of the destination operand are separated by 1 byte.

    For the non-32-byte-aligned scenario, because the Unified Buffer requires 32-byte alignment, the framework automatically appends 17 bytes of dummy data during copy-out to ensure alignment, and automatically discards the padded dummy data when copying to Global Memory, thereby implementing unaligned copy from Unified Buffer to Global Memory.

    **Figure 2**  Schematic diagram of unaligned copy from Unified Buffer to Global Memory when blockLen does not satisfy 32-byte alignment<a name="fig_datacopypad4"></a>  
    ![](../../../../figures/datacopypad4.png)

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: supported data types are bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, and complex64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id4 -->

<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference product: supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id5 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The source address in the Unified Buffer must be 32-byte aligned, and the destination address in the Global Memory must be 1-byte aligned.
- The values of the DataCopyExtParams structure parameters must be within the value range:

    **Table 4**  Value range of DataCopyExtParams structure parameters

    | Parameter | Range |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 2097151] |
    | srcStride | [0, 2^32 - 1] |
    | dstStride | [0, 2^32 - 1] |

    <!-- npu="950" id6 -->
    > [!NOTE]
    > In particular, for Ascend 950PR/Ascend 950DT, the data types and value ranges of srcStride and dstStride are as follows:
    > - srcStride: the data type is int64_t, and the value range is [0, 65535].
    > - dstStride: the data type is int64_t, and the value range is [0, 2^40-1].
    <!-- end id6 -->

<!-- npu="A3,910b" id9 -->
- When either blockCount or blockLen of the DataCopyExtParams structure parameter is 0, this API is treated as a NOP (no operation). This description applies to the following models:
  <!-- npu="A3" id7 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id7 -->
  <!-- npu="910b" id8 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id8 -->
<!-- end id9 -->

## Example<a name="section177231425115410"></a>

```cpp
AscendC::DataCopyExtParams copyParams{1, dstTotalLength * sizeof(T), 0, 0, 0};
AscendC::DataCopyPad(dstGlobal, srcLocal, copyParams);
```

For the complete sample, see [DataCopyPad sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).
