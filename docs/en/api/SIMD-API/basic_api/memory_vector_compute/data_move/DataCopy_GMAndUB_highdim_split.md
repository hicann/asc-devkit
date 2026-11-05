# DataCopy (High-Dimensional Slicing Data Movement Between GM and UB)<a name="ZH-CN_TOPIC_00000023829080211"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:34:08.033Z pushedAt=2026-10-28T03:14:39.488Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMAndUB_highdim_split_res.md#id1 -->

## Description<a name="section474617392321"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports high-dimensional segmented data copy between Global Memory and Unified Buffer (UB). The data retains its original format and content during transfer.

High-dimensional splitting data movement can be configured with movement parameters such as the number of data blocks, the length of a single data block, and the address offset, and supports both non-contiguous and contiguous data movement.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id10 -->
    - CO2 -> GM (supported only on the Atlas inference products AI Core)
    <!-- end id10 -->

## Prototype<a name="section1954364615315"></a>

The API supports both non-continuous copy and continuous copy. For continuous copy scenarios, [DataCopy\(GM and UB continuous data copy \)](DataCopy_GMAndUB_continuous.md) are recommended.

- Global Memory -> UB

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- UB -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Types](#section4219135304818). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand.<br>&bull;When the type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md), the storage location is UB, and the destination address must be 32-byte aligned.<br>&bull;When the type is [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md), the storage location is Global Memory, and the destination address must be aligned to the number of bytes occupied by the corresponding data type. |
| src | Input | Source operand.<br>&bull;When the type is GlobalTensor, the storage location is Global Memory, and the source address must be aligned to the number of bytes occupied by the corresponding data type.<br>&bull;When the type is LocalTensor, the storage location is UB, and the source address must be 32-byte aligned. |
| repeatParams | Input | Copy parameters, of type DataCopyParams. For details about the parameters, see [Table 3](#table_highdim_3). This parameter configures the size, number, and gap of the data blocks to be copied, and supports both non-contiguous and contiguous copying.<br>For the definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3**  DataCopyParams structure parameter definition<a name="table_highdim_3"></a>

| Parameter | Description |
| --- | --- |
| blockCount | Number of data blocks to be copied. Data type: uint16_t. Value range: blockCount ∈ [0, 4095]. Default value: 1. |
| blockLen | Length of each data block to be copied. Data type: uint16_t. Value range: blockLen ∈ [0, 65535]. Unit: DataBlock (32B). |
| srcGap | Gap between adjacent data blocks of the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: srcGap ∈ [0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| dstGap | Gap between adjacent data blocks of the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: dstGap ∈ [0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

The following sample demonstrates the usage of contiguous copy and non-contiguous copy through two examples.

For contiguous copy scenarios, [DataCopy\(GM and UB contiguous data copy \)](DataCopy_GMAndUB_continuous.md) are recommended. The following example only demonstrates that the high-dimensional split data copy API supports contiguous copy.

- Example 1: Contiguous copy

    Taking the half data type as an example, the shape of the source operand is 1 \* 128.

    Using [Figure 1](#zh-cn_topic_0000002535088926_fig4621152141711) as an example, **contiguously** copy 128 elements from the source operand to the destination operand. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 1, copying 1 data block, indicating continuous copying.
    - blockLen = 8, one data block has a length of 8 DataBlocks.
    - srcGap = 0, with an inter-block gap of 0 between adjacent data blocks in the source operand.
    - dstGap = 0, with an inter-block gap of 0 between adjacent data blocks in the destination operand.

    **Figure 1**  Contiguous copy diagram<a name="zh-cn_topic_0000002535088926_fig4621152141711"></a>  
    ![](../../../../figures/continuous_data_copy_diagram.png "Contiguous copy diagram")

- Example 2: Non-contiguous copy

    Take the half data type as an example. The shape of the source operand is 1 \* 112.

    Using [Figure 2](#zh-cn_topic_0000002535088926_fig422819202304) as an example, **non-contiguously** copy data from the source operand to the destination operand. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 2, copying 2 data blocks.
    - blockLen = 3, the length of one data block is 3 Data Blocks.
    - srcGap = 1, the gap between adjacent data blocks in the source operand is 1.
    - dstGap = 2, the gap between adjacent data blocks in the destination operand is 2.

    **Figure 2**  Non-continuous Copy Diagram<a name="zh-cn_topic_0000002535088926_fig422819202304"></a>  
    ![](../../../../figures/discontinuous_data_copy_diagram.png "Non-continuous Copy Diagram")

## Data Type<a name="section4219135304818"></a>

The source operand and destination operand support the same data types. The Global Memory -> UB and UB -> Global Memory data paths support the same data types for the same product, as follows:

<!-- npu="950" id11 -->

- Ascend 950PR&950DT products support the following data types: b8, b16, b32, b64.

<!-- end id11 -->

<!-- npu="A3" id12 -->

- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id12 -->

<!-- npu="910b" id13 -->

- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id13 -->

<!-- npu="310b" id14 -->

- Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id14 -->

<!-- npu="310p" id15 -->

- Atlas inference products AI Core support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id15 -->

<!-- npu="310p" id16 -->

- Atlas inference products Vector Core support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id16 -->

<!-- npu="910" id17 -->

- Atlas training products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.

<!-- end id17 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address in Global Memory must be aligned to the number of bytes occupied by the corresponding data type, and the address in UB must be 32-byte aligned.
<!-- npu="A3,910b,950" id23 -->
- When either blockCount or blockLen in the DataCopyParams structure is 0, this API is treated as a NOP (no operation). This applies to the following products:
  <!-- npu="A3" id20 -->
  - Atlas A3 products
  <!-- end id20 -->
  <!-- npu="910b" id21 -->
  - Atlas A2 products
  <!-- end id21 -->
  <!-- npu="950" id22 -->
  - Ascend 950PR&950DT products
  <!-- end id22 -->
<!-- end id23 -->
- The values of the **DataCopyParams** structure parameters must be within the value range:

    **Table 4** Value range of the DataCopyParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

- If you need to execute multiple DataCopy instructions and their destination addresses overlap, call [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that the multiple DataCopy instructions are serialized to prevent abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and their destination Global Memory addresses overlap, call `PipeBarrier<PIPE_MTE3>()` between the two copy instructions to synchronize the MTE3 copy-out pipeline. As shown in the right diagram below, when the destination UB addresses overlap, call `PipeBarrier<PIPE_MTE2>()` between the two copy instructions to synchronize the MTE2 copy-in pipeline.

    ![](../../../../figures/datacopy_address_overlap_sync_diagram.png)

<!-- npu="910b,A3" id24 -->

- For the following product models:
    - Atlas A2 products
    - Atlas A3 products

    In the cross-card communication operator development scenario, the **DataCopy** class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the `npu-smi info -t topo` command to query the HCCS physical link.

<!-- end id24 -->

## Examples<a name="section122101199486"></a>

- Global Memory -> UB

    ```cpp
    // srcLocal is a LocalTensor of the half type, and srcGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes DataCopyParams parameters, which supports continuous and non-continuous copy.
    AscendC::DataCopyParams intriParams;
    intriParams.blockCount = 1; // The number of continuous data blocks is 1.
    intriParams.blockLen = 512 * sizeof(half) / 32; // Length of a continuous data block, in DataBlock units. The length here is 512 half elements.
    intriParams.srcGap = 0; // The source operand performs continuous copy.
    intriParams.dstGap = 0; // The destination operand is laid out continuously.
    AscendC::DataCopy(srcLocal, srcGlobal, intriParams);
    ```

    Result example:

    ```text
    Input data srcGlobal: [1 2 3 ... 512]
    Output data srcLocal: [1 2 3 ... 512]
    ```

- UB -> Global Memory

    ```cpp
    // dstLocal is a LocalTensor of the half type, and dstGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes DataCopyParams to support continuous and non-continuous copy.
    AscendC::DataCopyParams intriParams;
    intriParams.blockCount = 1; // Number of continuous data blocks is 1.
    intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the continuous data block, in DataBlock units. Here the length is 512 half elements.
    intriParams.srcGap = 0; // The source operand performs continuous copy.
    intriParams.dstGap = 0; // The destination operand is laid out continuously.
    AscendC::DataCopy(dstGlobal, dstLocal, intriParams);
    ```

    Result example:

    ```text
    Input data dstLocal: [1 2 3 ... 512]
    Output data dstGlobal: [1 2 3 ... 512]
    ```
