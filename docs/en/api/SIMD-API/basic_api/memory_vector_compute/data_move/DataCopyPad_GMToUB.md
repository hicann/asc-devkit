# DataCopyPad (GMToUB Unaligned Data Movement)<a name="ZH-CN_TOPIC_0000001894460401"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:46:29.697Z pushedAt=2026-10-28T03:14:39.502Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype Without the mode Template Parameter

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_GMToUB_res.md#id1 -->

### Function Prototype with the mode Template Parameter

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_GMToUB_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API copies data from Global Memory to Unified Buffer (UB) in a non-aligned manner and pads data as needed.

When the length of each copied data block (blockLen) is not 32-byte aligned, each data block must be padded to 32-byte alignment.

<!-- npu="950" id15 -->
In particular, for Ascend 950PR&950DT products, the Compact mode is supported. In this mode, a single data block is allowed to be non-32-byte aligned, and all data blocks are merged into one contiguous data block, which is then padded on the right side to 32-byte alignment.
<!-- end id15 -->

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
    - GM -> VECOUT
    <!-- npu="950" id16 -->
    - GM -> VECCALC (supported only on Ascend 950PR&950DT products)
    <!-- end id16 -->

The padding data can be configured in two ways:

- Configure the paddingValue parameter of the [DataCopyPadExtParams](#table_gm2ub_pad_5) structure. In this case, the isPad parameter of DataCopyPadExtParams must be set to true.
- Use the register configuration API [SetPadValue](../data_move_aux_config/SetPadValue_ISASI.md) to set the padding data outside the API. In this case, the isPad parameter of DataCopyPadExtParams must be set to false.

## Prototype<a name="section620mcpsimp"></a>

- Configuring the data movement mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```

<!-- npu="950" id17 -->

- Configuring the data movement mode is supported (only supported by Ascend 950PR&950DT products).

    ```cpp
    // This function prototype is supported only on Ascend 950PR&950DT products.
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```

<!-- end id17 -->

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand and paddingValue (padding data value). For the data types supported by each product, see [Data Type](#section4219135304818). |
| mode | Configures the data copy mode. It is of the PaddingMode type, defined as follows:<br>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // Default mode, consistent with the original data copy format. Each data copy is padded to 32-byte alignment.<br>    Compact,     // Compact mode, allowing a single copy to be unaligned, with padding uniformly applied to 32-byte alignment at the end of the entire data block.<br>}; |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md), stored in UB. The destination address must be 32-byte aligned. |
| src | Input | Source operand, of type [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md), stored in Global Memory. The source address must be 1-byte aligned. |
| dataCopyParams | Input | Copy parameters, of type DataCopyExtParams. For details about the parameters, see the following. |
| padParams | Input | When copying data from Global Memory to Local Memory, you can pad data on the left or right side of the copied data as needed. padParams is a parameter that controls the data padding process, of type DataCopyPadExtParams. For details about the parameters, see [Table 5](#table_gm2ub_pad_5). |

For the structure parameter definitions listed in the following table, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.

<!-- npu="950" id30 -->

For Ascend 950PR&950DT products, see [Table 3](#table_gm2ub_pad_3) for the DataCopyExtParams structure parameter definitions.

**Table 3**  DataCopyExtParams structure parameter definitions<a name="table_gm2ub_pad_3"></a>

| Parameter | Description |
| :--- | :--- |
| blockCount | Number of data blocks to copy. Data type: uint16_t. Value range: blockCount∈[0, 65535]. Default value is 1. |
| blockLen | Length of each data block to copy. Data type: uint32_t. Value range: blockLen∈[0, $2^{21}−1$]. Unit: 1B.<br>**Note: blockLen must be an integer multiple of sizeof(T) and must not exceed the UB space size.** |
| srcStride | Interval between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: int64_t. Value range: srcStride∈[-blockLen, $2^{40}−1$]. Unit: 1B. When srcStride = -blockLen, the consecutive data blocks copied each time are the same block, that is, the first data block is copied repeatedly. |
| dstStride | Interval between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: int64_t. Value range: dstStride∈[0, 65535]. Unit: dataBlock (32B). If PaddingMode is set to Compact mode, this parameter is invalid and the default value 0 is used.<br>**Note: It must not exceed the UB space size.** |
| rsv | Reserved parameter, which must be set to 0. |

<!-- end id30 -->

<!-- npu="A3,910b,310b" id31 -->

For the following models, see [Table 4](#table_gm2ub_pad_4) for the DataCopyExtParams structure parameter definitions:

<!-- npu="A3" id33 -->
- Atlas A3 products
<!-- end id33 -->
<!-- npu="910b" id34 -->
- Atlas A2 products
<!-- end id34 -->
<!-- npu="310b" id35 -->
- Atlas 200I/500 A2 inference products
<!-- end id35 -->

**Table 4**  DataCopyExtParams structure parameter definitions<a name="table_gm2ub_pad_4"></a>

| Parameter | Description |
| :--- | :--- |
| blockCount | Number of data blocks to copy. Data type: uint16_t. Value range: blockCount∈[0, 4095]. Default value is 1. |
| blockLen | Length of each data block to copy. Data type: uint32_t. Value range: blockLen∈[0, $2^{21}−1$]. Unit: 1B.<br>**Note: blockLen must be an integer multiple of sizeof(T) and must not exceed the UB space size.** |
| srcStride | Interval between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint32_t. Value range: srcStride∈[0, $2^{32}−1$]. Unit: 1B. |
| dstStride | Interval between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint32_t. Value range: dstStride∈[0, $2^{32}−1$]. Unit: dataBlock (32B). If PaddingMode is set to Compact mode, this parameter is invalid and the default value 0 is used.<br>**Note: It must not exceed the UB space size.** |
| rsv | Reserved parameter, which must be set to 0. |

<!-- end id31 -->

**Table 5**  DataCopyPadExtParams&lt;T&gt; structure parameter definition<a name="table_gm2ub_pad_5"></a>

| Parameter | Description |
| :--- | :--- |
| isPad | Whether to set the padding value through the paddingValue field in the DataCopyPadExtParams structure.<br>true: Use the paddingValue field as the padding value.<br>false: Do not use the paddingValue field. There are two cases depending on whether the register configuration API [SetPadValue](../data_move_aux_config/SetPadValue_ISASI.md) is called.<br>&bull;When SetPadValue is called, the data to be padded is configured outside the API.<br>&bull;When SetPadValue is not called, the hardware automatically pads dummy data on the **right side of each data block** to 32-byte alignment. If PaddingMode is Compact, dummy data is padded only on the **right side of the last data block** to 32-byte alignment. |
| leftPadding | Data range to be padded on the left side of the consecutively copied data block, in elements.<br>If PaddingMode is Compact, this parameter is invalid, and data is padded only on the **right side of the last data block**.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.** |
| rightPadding | Data range to be padded on the right side of the consecutively copied data block, in elements.<br>If PaddingMode is Compact, this parameter is invalid, and data is padded only on the **right side of the last data block**.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.** |
| paddingValue | Data value to be padded on the left and right sides, which must be within the byte range occupied by the data.<br>The data type is the same as that of the source operand, and the data type is the template parameter T.<br>**When the data type length is 64 bits, this parameter can only be set to 0.** |

- GM -> VECIN/VECOUT configuration examples<a name="li73127579197"></a>

    The following three examples show different configuration scenarios of DataCopyPad on the GM -> VECIN/VECOUT path. Example 1 shows data copy in a 32-byte aligned scenario. Example 2 shows the dummy padding mechanism when the data is not 32-byte aligned. Example 3 (supported only by Ascend 950PR&950DT products) shows the scenario of repeatedly copying the same data block using a negative stride.

    - Parameter explanation
        - When blockLen+leftPadding+rightPadding meets 32-byte alignment, if isPad is false, the data values padded on the left and right sides default to random values; otherwise, they are paddingValue.
        - When blockLen+leftPadding+rightPadding does not meet 32-byte alignment, the framework pads some dummy data to ensure that the data padded on the left and right sides, blockLen, and the dummy data are 32-byte aligned. If both leftPadding and rightPadding are 0, the dummy data defaults to the first element value of the data block to be copied. If leftPadding/rightPadding is not 0, when isPad is false, the data values padded on the left and right sides and the dummy value are all random values; otherwise, they are paddingValue.

    - Example 1: 32-byte aligned scenario

        blockLen is 64, and each consecutively transmitted data block contains 64 bytes. srcStride is 1 because the logical position of the source operand is GM, and the unit of srcStride is bytes, meaning that the difference between the **end address** of the previous data block and the **start address** of the next data block in the source operand is 1 byte. dstStride is 1 because the logical position of the destination operand is VECIN/VECOUT, and the unit of dstStride is the number of data blocks (each data block is 32 bytes), meaning that adjacent data blocks in the destination operand are separated by 1 data block.

        blockLen+leftPadding+rightPadding meets 32-byte alignment, and isPad is false, so the data values padded on the left and right sides default to random values; otherwise, they are paddingValue. In this example, both leftPadding and rightPadding are 0, so no padding is performed.

        When blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment, the framework pads some dummy data to ensure that the data padded on the left and right sides, together with blockLen and the dummy data, is 32-byte aligned. When leftPadding/rightPadding is not 0: if isPad is false, the data values padded on the left and right sides and the dummy value are all random values; otherwise, they are paddingValue.

        ![](../../../../figures/datacopypad1.png)

    - Example 2: Non-32-byte-aligned scenario

        blockLen is 47, and each consecutive data block contains 47 bytes. srcStride is 1, indicating that the difference between the **end address** of the previous data block and the **start address** of the next data block in the source operand is 1 byte. dstStride is 1, indicating that adjacent data blocks in the destination operand are separated by 1 dataBlock.

        When blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment and leftPadding and rightPadding are both 0, dummy is padded with the first element value of the data block to be copied by default.

        When blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment and leftPadding/rightPadding is not 0: if isPad is false, the data values padded on the left and right sides and the dummy value are all random values; otherwise, they are paddingValue.

        ![](../../../../figures/datacopypad2.png)

    <!-- npu="950" id18 -->

    - Example 3: Negative-stride repeated copy scenario (supported only by Ascend 950PR&950DT products)

        blockLen is 48, and each consecutive data block contains 48 bytes. srcStride is -48, indicating that the difference between the **end address** of the previous data block and the **start address** of the next data block in the source operand is -48 bytes (the length of one consecutive data block is 48 bytes), which means that the consecutive data block copied each time is the same block. dstStride is 1, indicating that adjacent data blocks in the destination operand are separated by 1 dataBlock.

        When blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment and leftPadding and rightPadding are both 0, dummy is padded with the first element value of the data block to be copied by default.

        blockLen + leftPadding + rightPadding does not satisfy 32-byte alignment, and leftPadding/rightPadding is not 0: if isPad is false, the data values padded on the left and right sides and the dummy value are all random values; otherwise, they are paddingValue.

        ![](../../../../figures/datacopypad.png)

    <!-- end id18 -->

- Copy mode configuration examples (supported only by Ascend 950PR&950DT products)

    The following two examples show the difference between the Normal and Compact copy modes on Ascend 950PR&950DT products.

    <!-- npu="950" id19 -->

    - Example 1: Normal mode

        blockLen is 48, and each consecutively transmitted data block contains 48 bytes. srcStride is 0 because the logical position of the source operand is GM, and the unit of srcStride is bytes, meaning that adjacent data blocks of the source operand are tightly packed. dstStride is 0 because the logical position of the destination operand is VECIN/VECOUT, and the unit of dstStride is DataBlock (32 bytes), meaning that there is no gap between adjacent data blocks of the destination operand. Note that the data block contains leftPadding/rightPadding data.

        blockLen + leftPadding + rightPadding satisfies 32-byte alignment. isPad is false, and the data values padded on the left and right sides default to random values; otherwise, they are paddingValue. In this example, leftPadding is 0 and rightPadding is 16, so each consecutively transmitted data block is padded with 16 bytes on the right side. The total length of the destination operand is 192 bytes.

    - Example 2: Compact mode

        blockLen is 48, and each consecutively transmitted data block contains 48 bytes. srcStride is 0 because the logical position of the source operand is GM, and the unit of srcStride is bytes, meaning that adjacent data blocks of the source operand are tightly packed. dstStride is 0 because the logical position of the destination operand is VECIN/VECOUT, and the unit of dstStride is DataBlock (32 bytes), meaning that adjacent data blocks of the destination operand are tightly packed and no data is padded.

        blockLen * blockCount + leftPadding + rightPadding satisfies 32-byte alignment. isPad is false, and the data values padded on the left and right sides default to random values; otherwise, they are paddingValue. In this example, leftPadding is 0 and rightPadding is 16, so 16 bytes are padded on the right side of the last data block. The total length of the destination operand is 160 bytes.

        ![](../../../../figures/paddingMode.png)

    <!-- end id19 -->

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id20 -->

- For the Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.

<!-- end id20 -->

<!-- npu="A3" id21 -->

- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id21 -->

<!-- npu="910b" id22 -->

- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id22 -->

<!-- npu="310b" id23 -->

- Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.

<!-- end id23 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The value range of the parameters in the DataCopyPadExtParams structure must not exceed the UB space size.
<!-- npu="A3,910b,950" id27 -->
- When either blockCount or blockLen in the DataCopyExtParams structure is 0, this API is treated as a NOP (no operation). This applies to the following models:
  <!-- npu="A3" id24 -->
  - Atlas A3 products
  <!-- end id24 -->
  <!-- npu="910b" id25 -->
  - Atlas A2 products
  <!-- end id25 -->
  <!-- npu="950" id26 -->
  - Ascend 950PR&950DT products
  <!-- end id26 -->
<!-- end id27 -->
<!-- npu="950" id28 -->

- For Ascend 950PR&950DT products, if PaddingMode is set to Compact, the dstStride, leftPadding, and rightPadding parameters are invalid.

<!-- end id28 -->

- The source address in Global Memory must be 1-byte aligned, and the destination address in UB must be 32-byte aligned.
- The number of bytes of leftPadding and rightPadding must not exceed 32B.
- blockLen must be an integer multiple of sizeof\(T\).
- The data type of the paddingValue parameter in the DataCopyPadExtParams structure must be consistent with that of the source operand. When the data type is b64, paddingValue can only be set to 0.
<!-- npu="950" id29 -->
- For Ascend 950PR&950DT products, the data types and value ranges of the parameters in the DataCopyExtParams structure are as follows:

    **Table 6** Data types and value ranges of DataCopyExtParams structure parameters

    | Parameter | Data Type | Value Range |
    | --- | --- | --- |
    | blockCount | uint16_t | [0, 65535] |
    | blockLen | uint32_t | [0, $2^{21}−1$] |
    | srcStride | int64_t | [-blockLen, $2^{40}−1$] |
    | dstStride | int64_t | [0, 65535] |

<!-- end id29 -->
<!-- npu="A3,910b,310b" id32 -->
- For the following models, the data types and value ranges of DataCopyExtParams structure parameters are as follows:
  <!-- npu="A3" id36 -->
  - Atlas A3 products
  <!-- end id36 -->
  <!-- npu="910b" id37 -->
  - Atlas A2 products
  <!-- end id37 -->
  <!-- npu="310b" id38 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id38 -->

    **Table 7** Data types and value ranges of DataCopyExtParams structure parameters

    | Parameter | Data Type | Value Range |
    | --- | --- | --- |
    | blockCount | uint16_t | [0, 4095] |
    | blockLen | uint32_t | [0, $2^{21}−1$] |
    | srcStride | uint32_t | [0, $2^{32}−1$] |
    | dstStride | uint32_t | [0, $2^{32}−1$] |

<!-- end id32 -->

## Example<a name="section177231425115410"></a>

- Scenario 1: Use SetPadValue for custom padding

    ```cpp
    // DataCopyExtParams parameters: blockCount=1, blockLen=20*sizeof(half), srcStride=0, dstStride=0, rsv=0
    // DataCopyPadExtParams parameters: isPad=false, leftPadding=0, rightPadding=12
    // Set the padding value to 1 via SetPadValue, used together with isPad=false.
    AscendC::DataCopyExtParams copyParams{1, srcCols * sizeof(T), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams;
    padParams.isPad = false;
    padParams.leftPadding = 0;
    padParams.rightPadding = dstCols - srcCols;

    AscendC::SetPadValue((T)1);
    AscendC::DataCopyPad(srcLocal, srcGlobal, copyParams, padParams);
    ```

- Scenario 2: Use rightPadding for default padding

    ```cpp
    // DataCopyExtParams parameters: blockCount=32, blockLen=59*sizeof(float), srcStride=0, dstStride=0, rsv=0
    // DataCopyPadExtParams parameters: isPad=true, leftPadding=0, rightPadding=5
    // When isPad=true, the padding value is 0 and SetPadValue is not used.
    AscendC::DataCopyExtParams copyParams{srcRows, srcCols * sizeof(float), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams;
    padParams.isPad = true;
    padParams.leftPadding = 0;
    padParams.rightPadding = dstCols - srcCols;

    AscendC::DataCopyPad(srcLocal, srcGlobal, copyParams, padParams);
    ```

- Scenario 3: Use Compact mode for compact padding (supported only on Ascend 950PR&950DT products)

    ```cpp
    // DataCopyExtParams parameters: blockCount=3, blockLen=24*sizeof(half), srcStride=0, dstStride=0, rsv=0
    // DataCopyPadExtParams parameters: isPad=true, leftPadding=0, rightPadding=16
    // PaddingMode::Compact: Arrange multiple rows of data compactly into one row, with the padding area placed compactly after the data.
    // The input [3, 24] is compactly arranged as [1, 80], where the first 72 elements are the 3 rows of data and the last 8 elements are the padding area.
    AscendC::DataCopyExtParams copyParams{srcRows, srcCols * sizeof(T), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams;
    padParams.isPad = true;
    padParams.leftPadding = 0;
    padParams.rightPadding = dstCols - srcCols * srcRows;

    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(srcLocal, srcGlobal, copyParams, padParams);
    ```

- Scenario 4: Use SetLoopModePara to enable loop mode for data copy (supported only on Ascend 950PR&950DT products)

    ```cpp
    // Compact mode: In each inner loop, copy 80B and then pad 16B to achieve 96-byte alignment.
    // Set the padding value to -1.
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Compact>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- Scenario 5: Use SetLoopModePara to enable loop mode for data copy (supported only by Ascend 950PR&950DT products).

    ```cpp
    // Normal mode: copy 40B per block and then pad 24B to achieve 64-byte alignment.
    // Set the padding value to -1.
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- Scenario 6: Five-dimensional data copy (supported only by Ascend 950PR&950DT products).

    ```cpp
    // Global Memory[2, 4, 3, 128, 126]int8 -> UB[512, 128]int8
    // Normal mode, using loop mode.
    // Copy specification: [2, 2, 2, 64, 126], with 2 bytes of padding added to each 126 bytes to reach 128 bytes.
    // The final UB is stored contiguously as [512, 128].

    // Stride parameter description:
    // - loop1SrcStride=128*126: stride of the second dimension, where the source address skips 128*126 bytes per loop1.
    // - loop1DstStride=64*128: Each loop1 destination address skips 64*128 bytes (64 rows x 128 bytes/row).
    // - loop2SrcStride=3*128*126: Dimension 1 stride. Each loop2 source address skips 3*128*126 bytes.
    // - loop2DstStride=2*64*128: Each loop2 destination address skips 2*64*128 bytes.

    AscendC::LoopModeParams loopParam2Ub {
        2,  // loop1Size=2 (dimension 2 is copied twice)
        2,  // loop2Size=2 (dimension 1 is copied twice)
        128 * 126,  // loop1SrcStride (dimension 2 stride)
        64 * 128,   // loop1DstStride
        3 * 128 * 126,  // loop2SrcStride (dimension 1 stride)
        2 * 64 * 128    // loop2DstStride
    };
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);

    AscendC::DataCopyExtParams copyParams{
        64,  // blockCount=64 (dimension 3 is copied 64 times)
        126,  // blockLen=126 (dimension 4 copies 126 bytes, padded to 128)
        0, 0, 0  // srcStride=0, dstStride=0, rsv=0
    };
    AscendC::DataCopyPadExtParams<int8_t> padParams{true, 0, 0, 0};

    // The dimension 0 for loop copies twice.
    for (uint32_t dim0 = 0; dim0 < 2; dim0++) {
        constexpr uint32_t dim0SrcStride = 4 * 3 * 128 * 126;  // Source stride of dimension 0.
        constexpr uint32_t dim0DstStride = 2 * 2 * 64 * 128;   // Destination stride of dimension 0.
        uint32_t srcOffset = dim0 * dim0SrcStride;
        uint32_t dstOffset = dim0 * dim0DstStride;

        AscendC::DataCopyPad<int8_t>(srcLocal[dstOffset], srcGlobal[srcOffset], copyParams, padParams);
    }

    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

For the complete sample, see the [DataCopyPad sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).
