# GM-to-UB Unaligned Data Transfer (DataCopyPad)<a name="ZH-CN_TOPIC_0000001894460401"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T13:44:26.590Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id22 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id22 -->
<!-- npu="A3" id23 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id23 -->
<!-- npu="910b" id24 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id24 -->
<!-- npu="310b" id25 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id25 -->
<!-- npu="310p" id26 -->
- Atlas inference products AI Core: Not supported
<!-- end id26 -->
<!-- npu="310p" id27 -->
- Atlas inference products Vector Core: Not supported
<!-- end id27 -->
<!-- npu="910" id28 -->
- Atlas training products: Not supported
<!-- end id28 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`

This API moves data from Global Memory to Unified Buffer in an unaligned manner, and allows developers to pad data as needed.

When the length of each transferred data block (blockLen) is not 32-byte aligned, each data block must be padded to 32-byte alignment.

<!-- npu="950" id1 -->
In particular, for Ascend 950PR/Ascend 950DT, the Compact mode is supported. This mode allows a single data block to be non-32-byte aligned, merges all data blocks into one continuous data block, and pads data on the right side of the data block to 32-byte alignment.
<!-- end id1 -->

The supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
    - GM -> VECOUT
    <!-- npu="950" id2 -->
    - GM -> VECCALC (supported only on Ascend 950PR/Ascend 950DT)
    <!-- end id2 -->

The padding data can be configured in two ways:

- Configure the paddingValue parameter of the structure [DataCopyPadExtParams](#table_gm2ub_pad_4). In this case, the isPad parameter of DataCopyPadExtParams must be set to true.
- Use the register configuration API [SetPadValue](../data_move_aux_config/SetPadValue_ISASI.md) to set the padding data outside the API. In this case, the isPad parameter of DataCopyPadExtParams must be set to false.

## Prototype<a name="section620mcpsimp"></a>

- Configuring the data transfer mode (mode) is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```

<!-- npu="950" id3 -->
- Configuring the data transfer mode (mode) is supported (supported only on Ascend 950PR/Ascend 950DT).

    ```cpp
    // This function prototype is supported only on Ascend 950PR/Ascend 950DT.
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```
<!-- end id3 -->

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand and paddingValue (the padding data value). For the data types supported by each product, see [Data Type](#section4219135304818). |
| mode | Configures the data transfer mode. It is of the PaddingMode type, defined as follows:<br>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // Default mode, consistent with the original data transfer format. Each data transfer is padded to 32-byte alignment.<br>    Compact,     // Compact mode, which allows a single transfer to be unaligned and pads the entire data block to 32-byte alignment at the end.<br>};<br>|

**Table 2** API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md) type, stored in Unified Buffer. The destination address must be 32-byte aligned. |
| src | Input | Source operand, of the [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md) type, stored in Global Memory. The source address must be 1-byte aligned. |
| dataCopyParams | Input | Transfer parameters, of the DataCopyExtParams type. For details about the parameters, see [Table 3](#table_gm2ub_pad_3). |
| padParams | Input | When data is transferred from Global Memory to Local Memory, padding data can be added to the left or right side of the transferred data as required by the developer. padParams is the parameter used to control the data padding process. It is of the DataCopyPadExtParams type. For details about the parameters, see [Table 4](#table_gm2ub_pad_4). |

For the definitions of the structure parameters listed in the following tables, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after CANN is installed.

**Table 3**  DataCopyExtParams structure parameter definition<a name="table_gm2ub_pad_3"></a>

| Parameter | Description |
| :--- | :--- |
| blockCount | Number of data blocks to be transferred. The data type is uint16_t. Value range: blockCount∈[0, 4095]. The default value is 1. |
| blockLen | Length of each data block to be transferred. The data type is uint32_t. Value range: blockLen∈[0, 2097151]. Unit: 1B.<br>**blockLen must be an integer multiple of sizeof(T). Ensure that it does not exceed the UB space size.** |
| srcStride | Interval between adjacent data blocks of the source operand (i.e., the difference between the **end address** of the previous data block and the **start address** of the next data block). The data type is uint32_t. Value range: srcStride∈[0, 2^32 - 1]. Unit: 1B. The data type and supported value range of srcStride may differ across products. For details, see [Constraint](#section633mcpsimp). |
| dstStride | Interval between adjacent data blocks of the destination operand (i.e., the difference between the **end address** of the previous data block and the **start address** of the next data block). The data type is uint32_t. Value range: dstStride∈[0, 2^32 - 1]. Unit: dataBlock (32B). The data type and supported value range of dstStride may differ across products. For details, see [Constraints](#section633mcpsimp). If PaddingMode is Compact mode, this parameter is invalid, and the default value is 0. Use the default value.<br>**Note: Ensure that it does not exceed the UB space size**. |
| rsv | Reserved field. The data type is uint16_t. The default value is 0. |

**Table 4**  DataCopyPadExtParams&lt;T&gt; structure parameter definition<a name="table_gm2ub_pad_4"></a>

| Parameter | Description |
| :--- | :--- |
| isPad | Whether to set the padding value through the paddingValue field in the DataCopyPadExtParams structure.<br>true: Use the paddingValue field as the padding value.<br>false: Do not use the paddingValue field. Depending on whether the register configuration API [SetPadValue](../data_move_aux_config/SetPadValue_ISASI.md) is called, there are two cases.<br>&bull; When SetPadValue is called, the data to be padded is configured outside the API.<br>&bull; When SetPadValue is not called, the hardware automatically pads dummy data on the **right side of each data block** to 32-byte alignment. If PaddingMode is Compact mode, dummy data is padded only on the **right side of the last data block** to 32-byte alignment. |
| leftPadding | Data range to be padded on the left side of the consecutively transferred data blocks. The unit is the number of elements.<br>If PaddingMode is Compact mode, this parameter is invalid, and data is padded only on the **right side of the last data block**.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.** |
| rightPadding | Data range to be padded on the right side of the consecutively transferred data blocks. The unit is the number of elements.<br>If PaddingMode is Compact mode, this parameter is invalid, and data is padded only on the **right side of the last data block**.<br>**The number of bytes occupied by leftPadding and rightPadding must not exceed 32 bytes.** |
| paddingValue | Data value to be padded on the left and right sides. Ensure that it is within the byte range occupied by the data.<br>The data type is consistent with that of the source operand, and the data type is the template parameter T.<br>**When the data type length is 64 bits, this parameter can only be set to 0.** |

- GM -> VECIN/VECOUT configuration example<a name="li73127579197"></a>

    The following three examples demonstrate different configuration scenarios of DataCopyPad on the GM -> VECIN/VECOUT path. Example 1 demonstrates data transfer in a 32-byte aligned scenario. Example 2 demonstrates the dummy padding mechanism when data is not 32-byte aligned. Example 3 (supported only by Ascend 950PR/Ascend 950DT) demonstrates the scenario of repeatedly transferring the same data block using a negative stride.

    - Parameter explanation
        - When blockLen+leftPadding+rightPadding satisfies 32-byte alignment, if isPad is false, the data values padded on the left and right sides are random values by default; otherwise, they are paddingValue.
        - When blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment, the framework pads some dummy data to ensure that the data padded on the left and right sides, blockLen, and the dummy data are 32-byte aligned. If both leftPadding and rightPadding are 0: dummy is padded by default with the value of the first element of the data block to be transferred. If leftPadding/rightPadding is not 0: when isPad is false, the data values padded on the left and right sides and the dummy value are all random values; otherwise, they are paddingValue.

    - Example 1: 32-byte aligned scenario

        blockLen is 64, and each consecutive data block to be transferred contains 64 bytes. srcStride is 1 because the logical position of the source operand is GM and the unit of srcStride is bytes, that is, the difference between the **end address** of the previous data block and the **start address** of the next data block in the source operand is 1 byte. dstStride is 1 because the logical position of the destination operand is VECIN/VECOUT and the unit of dstStride is the number of DataBlocks (each DataBlock is 32 bytes), that is, adjacent data blocks in the destination operand are separated by 1 dataBlock.

        blockLen+leftPadding+rightPadding satisfies 32-byte alignment. If isPad is false, the padding data values on the left and right sides default to random values; otherwise, they are paddingValue. In the example here, leftPadding and rightPadding are both 0, so no padding is performed.

        When blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment, the framework pads some dummy data to ensure that the padding data on the left and right sides, blockLen, and the dummy data are 32-byte aligned. When leftPadding/rightPadding is not 0: if isPad is false, the padding data values on the left and right sides and the dummy values are all random; otherwise, they are paddingValue.

        ![](../../../../figures/datacopypad1.png)

    - Example 2: Non-32-byte-aligned scenario

        blockLen is 47, and each consecutive data block to be transferred contains 47 bytes. srcStride is 1, indicating that the difference between the **end address** of the previous data block and the **start address** of the next data block in the source operand is 1 byte. dstStride is 1, indicating that adjacent data blocks in the destination operand are separated by 1 dataBlock.

        blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment, and leftPadding and rightPadding are both 0: dummy is padded by default with the value of the first element of the data block to be transferred.

        blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment, and leftPadding/rightPadding is not 0: if isPad is false, the padding data values on the left and right sides and the dummy values are all random; otherwise, they are paddingValue.

        ![](../../../../figures/datacopypad2.png)

    <!-- npu="950" id4 -->
    - Example 3: Negative-stride repeated transfer scenario (supported only on Ascend 950PR/Ascend 950DT)

        blockLen is 48, and each consecutive transfer data block contains 48 bytes. srcStride is -48, indicating that the difference between the **end address** of the previous data block and the **start address** of the next data block in the source operand is -48 bytes (the length of a consecutive data block is 48 bytes), which means that the consecutive data block transferred each time is the same block. dstStride is 1, indicating that adjacent data blocks in the destination operand are separated by one dataBlock.

        If blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment and both leftPadding and rightPadding are 0, dummy will be padded by default with the value of the first element of the data block to be transferred.

        If blockLen+leftPadding+rightPadding does not satisfy 32-byte alignment and leftPadding/rightPadding are not 0: if isPad is false, the padding data values on the left and right sides and the dummy value are all random values; otherwise, they are paddingValue.

        ![](../../../../figures/datacopypad.png)
    <!-- end id4 -->

- Configuration examples of transfer modes (supported only on Ascend 950PR/Ascend 950DT)

    The following two examples show the difference between the Normal and Compact transfer modes on Ascend 950PR/Ascend 950DT.

    <!-- npu="950" id5 -->
    - Example 1: Normal mode

        blockLen is 48, and each consecutive transfer data block contains 48 bytes. srcStride is 0, the logical position of the source operand is GM, and the unit of srcStride is byte, meaning that adjacent data blocks in the source operand are tightly packed. dstStride is 0, the logical position of the destination operand is VECIN/VECOUT, and the unit of dstStride is DataBlock (32 bytes), meaning that there is no gap between adjacent data blocks in the destination operand. Note that a data block contains leftPadding/rightPadding data.

        blockLen + leftPadding + rightPadding satisfies 32-byte alignment. If isPad is false, the padding data values on the left and right sides default to random values; otherwise, they are paddingValue. In this example, leftPadding is 0 and rightPadding is 16, so each consecutive transfer data block is padded with 16 bytes on the right side. The total length of the destination operand is 192 bytes.

    - Example 2: Compact mode

        blockLen is 48, and each consecutive data block transferred contains 48 bytes. srcStride is 0, the logical position of the source operand is GM, and the unit of srcStride is byte, i.e., the source operand's adjacent data blocks are tightly packed. dstStride is 0, the logical position of the destination operand is VECIN/VECOUT, and the unit of dstStride is DataBlock (32 bytes). The destination operand's adjacent data blocks are tightly packed, and no padding data is added.

        blockLen * blockCount + leftPadding + rightPadding satisfies 32-byte alignment. isPad is false, and the padding values on the left and right sides default to random values; otherwise, they are paddingValue. In this example, leftPadding is 0 and rightPadding is 16, so 16 bytes are padded on the right side of the last data block. The total length of the destination operand is 160 bytes.

        ![](../../../../figures/paddingMode.png)
    <!-- end id5 -->

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id6 -->
- Ascend 950PR/Ascend 950DT, the supported data type is: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id6 -->

<!-- npu="A3" id7 -->
- Atlas A3 training series products/Atlas A3 inference series products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id7 -->

<!-- npu="910b" id8 -->
- Atlas A2 training series products/Atlas A2 inference series products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id8 -->

<!-- npu="310b" id9 -->
- Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id9 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The value range of the parameters in the DataCopyPadExtParams structure must not exceed the UB space size.
<!-- npu="A3,910b" id14 -->
- When either blockCount or blockLen in the DataCopyExtParams structure is 0, this API is treated as a NOP (no operation). This description applies to the following models:
  <!-- npu="A3" id10 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id10 -->
  <!-- npu="910b" id11 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id11 -->
<!-- end id14 -->
<!-- npu="950" id12 -->
- For Ascend 950PR/Ascend 950DT, if PaddingMode is Compact mode, the parameters dstStride, leftPadding, and rightPadding are invalid.
<!-- end id12 -->

- The source address in Global Memory must be 1-byte aligned, and the destination address in Unified Buffer must be 32-byte aligned.
- The number of bytes occupied by leftPadding and rightPadding must not exceed 32B.
- blockLen must be an integer multiple of sizeof\(T\).
- The data type of the paddingValue parameter in the DataCopyPadExtParams structure must be consistent with that of the source operand. When the data type is b64, paddingValue can only be set to 0.
- The values of the parameters in the DataCopyExtParams structure must be within the value range:

    **Table 5**  Value range of the DataCopyExtParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 2097151] |
    | srcStride | [0, 2^32 - 1] |
    | dstStride | [0, 2^32 - 1] |

    <!-- npu="950" id13 -->
    > [!NOTE]
     > In particular, for Ascend 950PR/Ascend 950DT, the data types and value ranges of srcStride and dstStride are as follows:
     > - srcStride: The data type is int64_t, and the value range is [-blockLen, 2^40-1]. When srcStride = -blockLen, it indicates that the consecutive data blocks transferred each time are the same block, that is, the first data block is transferred repeatedly.
     > - dstStride: The data type is int64_t, and the value range is [0, 65535].
    <!-- end id13 -->

## Example<a name="section177231425115410"></a>

- Scenario 1: Use SetPadValue for custom padding

    ```cpp
    // DataCopyExtParams parameters: blockCount=1, blockLen=20*sizeof(half), srcBlkStride=0, srcRepStride=0, reserved=0
    // DataCopyPadExtParams parameters: isPad=false, leftPadding=0, rightPadding=12
    // SetPadValue sets the padding value to 1, used together with isPad=false.
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
    // DataCopyExtParams parameters: blockCount=32, blockLen=59*sizeof(float), srcBlkStride=0, srcRepStride=0, reserved=0
    // DataCopyPadExtParams parameters: isPad=true, leftPadding=0, rightPadding=5
    // When isPad=true, the padding value is 0, and SetPadValue is not used.
    AscendC::DataCopyExtParams copyParams{srcRows, srcCols * sizeof(float), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams;
    padParams.isPad = true;
    padParams.leftPadding = 0;
    padParams.rightPadding = dstCols - srcCols;

    AscendC::DataCopyPad(srcLocal, srcGlobal, copyParams, padParams);
    ```

- Scenario 3: Use Compact mode for compact padding (supported only on Ascend 950PR/Ascend 950DT)

    ```cpp
    // DataCopyExtParams parameters: blockCount=3, blockLen=24*sizeof(half), srcBlkStride=0, srcRepStride=0, reserved=0
    // DataCopyPadExtParams parameters: isPad=true, leftPadding=0, rightPadding=16
    // PaddingMode::Compact: tightly pack multiple rows into one row, with the padding area placed compactly after the data.
    // The input [3, 24] is tightly packed into [1, 80], where the first 72 elements are the 3 rows of data and the last 8 elements are the padding area.
    AscendC::DataCopyExtParams copyParams{srcRows, srcCols * sizeof(T), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams;
    padParams.isPad = true;
    padParams.leftPadding = 0;
    padParams.rightPadding = dstCols - srcCols * srcRows;

    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(srcLocal, srcGlobal, copyParams, padParams);
    ```

- Scenario 4: Use SetLoopModePara to enable loop mode for data transfer (supported only on Ascend 950PR/Ascend 950DT)

    ```cpp
    // Compact mode: each inner loop transfers 80B and then pads 16B to achieve 96-byte alignment.
    // Set the padding value to -1
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Compact>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- Scenario 5: Use SetLoopModePara to enable loop mode for data transfer (supported only on Ascend 950PR/Ascend 950DT)

    ```cpp
    // Normal mode: each block transfers 40B and then pads 24B to achieve 64-byte alignment.
    // Set the padding value to -1
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- Scenario 6: Five-dimensional data transfer (supported only on Ascend 950PR/Ascend 950DT)

    ```cpp
    // Global Memory[2, 4, 3, 128, 126]int8 -> Unified Buffer[512, 128]int8
    // Normal mode, using loop mode.
    // Transfer specification: [2, 2, 2, 64, 126], where each 126-byte block is padded with 2 bytes to 128 bytes.
    // The final UB is stored contiguously as [512, 128].

    // Stride parameter description:
    // - loop1SrcStride=128*126: stride of the second dimension, where the source address skips 128*126 bytes for each loop1.
    // - loop1DstStride=64*128: the destination address skips 64*128 bytes for each loop1 (64 rows x 128 bytes/row).
    // - loop2SrcStride=3*128*126: stride of the first dimension, where the source address skips 3*128*126 bytes for each loop2.
    // - loop2DstStride=2*64*128: the destination address skips 2*64*128 bytes for each loop2.

    AscendC::LoopModeParams loopParam2Ub {
        2,  // loop1Size=2 (the second dimension is transferred twice)
        2,  // loop2Size=2 (transfer twice in dimension 1)
        128 * 126,  // loop1SrcStride (stride of dimension 2)
        64 * 128,   // loop1DstStride
        3 * 128 * 126,  // loop2SrcStride (stride of dimension 1)
        2 * 64 * 128    // loop2DstStride
    };
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);

    AscendC::DataCopyExtParams copyParams{
        64,  // blockCount=64 (transfer 64 times in dimension 3)
        126,  // blockLen=126 (transfer 126 bytes in dimension 4, padded to 128)
        0, 0, 0  // srcStride/dstStride=0
    };
    AscendC::DataCopyPadExtParams<int8_t> padParams{true, 0, 0, 0};

    // The dimension-0 for loop transfers 2 times.
    for (uint32_t dim0 = 0; dim0 < 2; dim0++) {
        constexpr uint32_t dim0SrcStride = 4 * 3 * 128 * 126;  // source stride of dimension 0
        constexpr uint32_t dim0DstStride = 2 * 2 * 64 * 128;   // destination stride of dimension 0
        uint32_t srcOffset = dim0 * dim0SrcStride;
        uint32_t dstOffset = dim0 * dim0DstStride;

        AscendC::DataCopyPad<int8_t>(srcLocal[dstOffset], srcGlobal[srcOffset], copyParams, padParams);
    }

    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

For the complete sample, see [DataCopyPad sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).
