# DataCopyPad (UB to GM unaligned data movement)<a name="ZH-CN_TOPIC_0000001894460401"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:51:24.156Z pushedAt=2026-10-28T03:14:39.505Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype of the Mode That Does Not Support Data Copy

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_UBToGM_res.md#id1 -->

### Prototype Supporting the Data Copy Mode

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: not supported
<!-- end id14 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_UBToGM_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API copies data from the Unified Buffer (UB) to Global Memory in a non-aligned manner.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- UB -> Global Memory
    - VECIN -> GM
    - VECOUT -> GM

- As shown in [Figure 1](#fig_datacopypad3), for the 32-byte aligned copy scenario, all data read from the UB is copied to Global Memory.
- As shown in [Figure 2](#fig_datacopypad4), for the non-32-byte-aligned scenario, dummy data is filled in when reading UB data to align it to 32B, and the dummy data is discarded when copying to Global Memory, thereby implementing non-aligned copy from the UB to Global Memory.

## Prototype<a name="section620mcpsimp"></a>

- Configuring the data movement mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- npu="950" id15 -->

- Configuring the data movement mode is supported (only supported by Ascend 950PR&950DT products).

    ```cpp
    // This function prototype is supported only on Ascend 950PR&950DT products.
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- end id15 -->

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types of each product, see [Data Types](#section4219135304818). |
| mode | Configures the data copy mode. It is of the PaddingMode type, defined as follows:<br>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // Default mode, consistent with the original data copy format. Each data copy is padded to 32-byte alignment.<br>    Compact,     // Compact mode, allowing a single copy to be unaligned, with padding uniformly applied to 32-byte alignment at the end of the entire data block.<br>}; |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand of type [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md).<br>No address alignment constraint on the start address. |
| src | Input | Source operand of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md).<br>The start address must be 32-byte aligned. |
| dataCopyParams | Input | Copy parameters of the DataCopyExtParams type. See the following for details. |

For the structure parameter definitions listed in the following tables, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.

<!-- npu="950" id25 -->

For Ascend 950PR&950DT products, see [Table 3](#table_ub2gm_pad_3) for the DataCopyExtParams structure parameter definitions.

**Table 3**  DataCopyExtParams structure parameter definitions<a name="table_ub2gm_pad_3"></a>

| Parameter | Description |
| :--- | :--- |
| blockCount | Specifies the number of consecutive transferred data blocks contained in this instruction. Data type: uint16_t. Value range: blockCount∈[0, 65535]. |
| blockLen | Specifies the length of each consecutive transferred data block in this instruction. Data type: uint32_t. Value range: blockLen∈[0, $2^{21}−1$]. Unit: 1B.<br>**Note: blockLen must be an integer multiple of sizeof(T) and must not exceed the UB space size**. |
| srcStride | Source operand. Interval between adjacent consecutive data blocks (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block).<br>The logical position of the source operand is VECIN/VECOUT, and the unit is dataBlock (32 bytes).<br>Data type: int64_t. Value range: [0, 65535]. |
| dstStride | Destination operand. Interval between adjacent consecutive data blocks (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block).<br>The logical position of the destination operand is GM, and the unit is byte.<br>Data type: int64_t. Value range: [0, $2^{40}−1$]. |
| rsv | Reserved parameter. Set to 0. |

<!-- end id25 -->

<!-- npu="A3,910b,310b" id26 -->

For the following models, see [Table 4](#table_ub2gm_pad_4) for the DataCopyExtParams structure parameter definitions:

<!-- npu="A3" id28 -->
- Atlas A3 products
<!-- end id28 -->
<!-- npu="910b" id29 -->
- Atlas A2 products
<!-- end id29 -->
<!-- npu="310b" id30 -->
- Atlas 200I/500 A2 inference products
<!-- end id30 -->

**Table 4**  DataCopyExtParams structure parameter definitions<a name="table_ub2gm_pad_4"></a>

| Parameter | Description |
| :--- | :--- |
| blockCount | Specifies the number of consecutive transferred data blocks contained in this instruction. Data type: uint16_t. Value range: blockCount∈[0, 4095]. |
| blockLen | Specifies the length of each consecutive transferred data block in this instruction. Data type: uint32_t. Value range: blockLen∈[0, $2^{21}−1$]. Unit: 1B.<br>**Note: blockLen must be an integer multiple of sizeof(T) and must not exceed the UB space size**. |
| srcStride | Source operand. Interval between adjacent consecutive data blocks (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block).<br>The logical position of the source operand is VECIN/VECOUT, and the unit is dataBlock (32 bytes).<br>Data type: uint32_t. Value range: [0, $2^{32}−1$]. |
| dstStride | Destination operand. Interval between adjacent consecutive data blocks (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block).<br>The logical position of the destination operand is GM, and the unit is byte.<br>Data type: uint32_t. Value range: [0, $2^{32}−1$]. |
| rsv | Reserved parameter. Set to 0. |

<!-- end id26 -->

The following describes the non-alignment copy from UB to Global Memory through two scenarios, corresponding to 32-byte alignment and non-32-byte alignment respectively:

- 32-byte alignment scenario<a name="32-byte-alignment-scenario"></a>

    As shown in [Figure 1](#fig_datacopypad3), blockLen is 64, and each consecutively transferred data block contains 64 bytes. srcStride is 1, the logical position of the source operand is VECIN/VECOUT, and the unit of srcStride is dataBlock (32 bytes), meaning that adjacent data blocks of the source operand are separated by an interval of 1 dataBlock. dstStride is 1, the logical position of the destination operand is GM, and the unit of dstStride is byte, meaning that adjacent data blocks of the destination operand are separated by an interval of 1 byte.

    For the 32-byte-aligned copy scenario, all data read from the UB is copied to Global Memory.

    **Figure 1**  Non-aligned copy from UB to Global Memory when blockLen is 32-byte aligned<a name="fig_datacopypad3"></a><br>
    ![](../../../../figures/datacopypad3.png)

- Non-32-byte-aligned scenario<a name="non-32-byte-aligned-scenario"></a>

    As shown in [Figure 2](#fig_datacopypad4), blockLen is 47, and each consecutively transferred data block contains 47 bytes, which does not satisfy 32-byte alignment. srcStride is 1, meaning that adjacent data blocks of the source operand are separated by an interval of 1 dataBlock. dstStride is 1, meaning that adjacent data blocks of the destination operand are separated by an interval of 1 byte.

    For the non-32-byte-aligned scenario, because the UB requires 32-byte alignment, the framework automatically appends 17 bytes of dummy data during copy-out to ensure alignment, and automatically discards the padded dummy data when copying to Global Memory, thereby implementing non-aligned copy from UB to Global Memory.

    **Figure 2**  Non-aligned copy from UB to Global Memory when blockLen does not satisfy 32-byte alignment<a name="fig_datacopypad4"></a><br>
    ![](../../../../figures/datacopypad4.png)

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id16 -->

- For the Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.

<!-- end id16 -->

<!-- npu="A3" id17 -->

- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id17 -->

<!-- npu="910b" id18 -->

- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id18 -->

<!-- npu="310b" id19 -->

- Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.

<!-- end id19 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The source address in the UB must be 32-byte aligned, and the destination address in Global Memory must be 1-byte aligned.
<!-- npu="950" id20 -->
- For Ascend 950PR&950DT products, the data types and value ranges of the DataCopyExtParams structure parameters are as follows:

    **Table 5**  Data types and value ranges of the DataCopyExtParams structure parameters

    | Parameter | Data Type | Value Range |
    | --- | --- | --- |
    | blockCount | uint16_t | [0, 65535] |
    | blockLen | uint32_t | [0, $2^{21}−1$] |
    | srcStride | int64_t | [0, 65535] |
    | dstStride | int64_t | [0, $2^{40}−1$] |

<!-- end id20 -->
<!-- npu="A3,910b,310b" id27 -->
- For the following models, the data types and value ranges of the DataCopyExtParams structure parameters are as follows:
  <!-- npu="A3" id31 -->
  - Atlas A3 products
  <!-- end id31 -->
  <!-- npu="910b" id32 -->
  - Atlas A2 products
  <!-- end id32 -->
  <!-- npu="310b" id33 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id33 -->

    **Table 6**  Data types and value ranges of the DataCopyExtParams structure parameters

    | Parameter | Data Type | Value Range |
    | --- | --- | --- |
    | blockCount | uint16_t | [0, 4095] |
    | blockLen | uint32_t | [0, $2^{21}−1$] |
    | srcStride | uint32_t | [0, $2^{32}−1$] |
    | dstStride | uint32_t | [0, $2^{32}−1$] |

<!-- end id27 -->

<!-- npu="A3,910b,950" id24 -->
- When either blockCount or blockLen of the DataCopyExtParams structure parameters is 0, this API is treated as a NOP (no operation). This description applies to the following models:
  <!-- npu="A3" id21 -->
  - Atlas A3 products
  <!-- end id21 -->
  <!-- npu="910b" id22 -->
  - Atlas A2 products
  <!-- end id22 -->
  <!-- npu="950" id23 -->
  - Ascend 950PR&950DT products
  <!-- end id23 -->
<!-- end id24 -->

## Example<a name="section177231425115410"></a>

```cpp
AscendC::DataCopyExtParams copyParams{1, dstTotalLength * sizeof(T), 0, 0, 0};
AscendC::DataCopyPad(dstGlobal, srcLocal, copyParams);
```

For the complete example, see the [DataCopyPad sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).
