# High-Dimensional Split Data Transfer Between GM and UB (DataCopy)<a name="ZH-CN_TOPIC_00000023829080211"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T13:13:30.422Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id20 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id20 -->
<!-- npu="A3" id21 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id21 -->
<!-- npu="910b" id22 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id22 -->
<!-- npu="310b" id23 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id23 -->
<!-- npu="310p" id24 -->
- Atlas inference products AI Core: Supported
<!-- end id24 -->
<!-- npu="310p" id25 -->
- Atlas inference products Vector Core: Supported
<!-- end id25 -->
<!-- npu="910" id26 -->
- Atlas training products: Supported
<!-- end id26 -->

## Description<a name="section474617392321"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

It supports high-dimensional segmented data copy between Global Memory and Unified Buffer, with the data retaining its original format and content during transmission.

High-dimensional segmented data copy can be implemented by configuring copy parameters such as the number of data blocks, the length of a single data block, and the address offset, supporting both non-continuous and continuous data copy.

The supported data paths are as follows (represented by the [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id1 -->

## Prototype<a name="section1954364615315"></a>

The API supports both non-continuous copy and continuous copy. For continuous copy scenarios, it is recommended to use [GM and UB continuous data copy \(DataCopy\)](DataCopy_GMAndUB_continuous.md).

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Type](#section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand.<br>&bull; When the type is [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md), the storage location is Unified Buffer, and the destination address must be 32-byte aligned.<br>&bull; When the type is [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md), the storage location is Global Memory, and the destination address must be aligned to the number of bytes occupied by the corresponding data type. |
| src | Input | Source operand.<br>&bull; When the type is GlobalTensor, the storage location is Global Memory, and the source address must be aligned to the number of bytes occupied by the corresponding data type.<br>&bull; When the type is LocalTensor, the storage location is Unified Buffer, and the source address must be 32-byte aligned. |
| repeatParams | Input | Copy parameters, of the DataCopyParams type. For details about the parameters, see [Table 3](#table_highdim_3). This parameter configures the size, count, and gap of the data blocks to be copied, and supports both non-continuous copy and continuous copy.<br>For the specific definition, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after CANN software installation. |

**Table 3** DataCopyParams structure parameter definition<a name="table_highdim_3"></a>

| Parameter | Description |
| --- | --- |
| blockCount | Number of data blocks to be copied. Data type: uint16_t. Value range: blockCount∈[0, 4095]. Default value: 1. |
| blockLen | Length of each data block to be copied. Data type: uint16_t. Value range: blockLen∈[0, 65535]. Unit: DataBlock (32B). |
| srcGap | Gap between adjacent data blocks in the source operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: srcGap∈[0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, srcGap is meaningless and can be set to 0. |
| dstGap | Gap between adjacent data blocks in the destination operand (that is, the difference between the **end address** of the previous data block and the **start address** of the next data block). Data type: uint16_t. Value range: dstGap∈[0, 65535]. Unit: DataBlock (32B).<br>&bull; When blockCount = 1, dstGap is meaningless and can be set to 0. |

The following sample demonstrates the usage of continuous copy and non-continuous copy through two examples.

For continuous copy scenarios, it is recommended to use [Continuous Data Transfer Between GM and UB \(DataCopy\)](DataCopy_GMAndUB_continuous.md). The following examples only demonstrate that the high-dimensional split data copy API supports continuous copy.

- Example 1: Continuous copy

    Taking the half data type as an example, the shape of the source operand is 1 \* 128.

    Using [Figure 1](#zh-cn_topic_0000002535088926_fig4621152141711) as an example, the 128 elements in the source operand are **continuously** copied to the destination operand. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 1, one data block is copied, indicating a continuous copy.
    - blockLen = 8, the length of one data block is 8 DataBlocks.
    - srcGap = 0, the gap between adjacent data blocks in the source operand is 0.
    - dstGap = 0, the gap between adjacent data blocks in the destination operand is 0.

    **Figure 1**  Continuous copy diagram<a name="zh-cn_topic_0000002535088926_fig4621152141711"></a>  
    ![](../../../../figures/continuous_data_copy_diagram.png "Continuous copy diagram")

- Example 2: Non-continuous copy

    Taking the half data type as an example, the shape of the source operand is 1 \* 112.

    Taking [Figure 2](#zh-cn_topic_0000002535088926_fig422819202304) as an example, the data in the source operand is copied to the destination operand in a **non-continuous** manner. The DataCopyParams structure parameters are configured as follows:

    - blockCount = 2, indicating that two data blocks are copied.
    - blockLen = 3, indicating that the length of one data block is three DataBlocks.
    - srcGap = 1, indicating that the gap between adjacent data blocks in the source operand is 1.
    - dstGap = 2, indicating that the gap between adjacent data blocks in the destination operand is 2.

    **Figure 2** Non-continuous copy diagram<a name="zh-cn_topic_0000002535088926_fig422819202304"></a>  
    ![](../../../../figures/discontinuous_data_copy_diagram.png "Non-continuous copy diagram")

## Data Type<a name="section4219135304818"></a>

The data types supported by the source operand and the destination operand must be consistent. The Global Memory -> Unified Buffer and Unified Buffer -> Global Memory data paths support the same data types for the same product, as follows:

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: b8, b16, b32, and b64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id4 -->

<!-- npu="310b" id5 -->
- Atlas inference products: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id5 -->

<!-- npu="310p" id6 -->
- Atlas inference products AI Core: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id6 -->

<!-- npu="310p" id7 -->
- Atlas inference products Vector Core: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id7 -->

<!-- npu="910" id8 -->
- Atlas training products: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id8 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- Addresses in Global Memory must be aligned according to the number of bytes occupied by the corresponding data type, and addresses in Unified Buffer must be 32-byte aligned.
<!-- npu="A3,910b" id12 -->
- When either blockCount or blockLen in the DataCopyParams structure is 0, the API is treated as a NOP (no operation). This description applies to the following models:
  <!-- npu="A3" id9 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id9 -->
  <!-- npu="910b" id10 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id10 -->
<!-- end id12 -->
- The values of the DataCopyParams structure parameters must be within the value range:

    **Table 4**  Value range of DataCopyParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

- If multiple DataCopy instructions need to be executed and their destination addresses overlap, call [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that the multiple DataCopy instructions are serialized to prevent abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and their destination Global Memory addresses overlap, call PipeBarrier<PIPE_MTE3>() between the two copy instructions to add synchronization for the MTE3 copy-out pipeline. As shown in the right diagram below, when the destination Unified Buffer addresses overlap, call PipeBarrier<PIPE_MTE2>() between the two copy instructions to add synchronization for the MTE2 copy-in pipeline.

    ![](../../../../figures/datacopy_address_overlap_sync_diagram.png)

<!-- npu="910b,A3" id11 -->
- For the following product models:
    - Atlas A2 training products/Atlas A2 inference products
    - Atlas A3 training products/Atlas A3 inference products

    In cross-card communication operator development scenarios, DataCopy APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical paths involved in inter-card communication. You can run the `npu-smi info -t topo` command to query the HCCS physical link.
<!-- end id11 -->

## Example<a name="section122101199486"></a>

- Global Memory -> Unified Buffer

    ```cpp
    // srcLocal is a LocalTensor of the half type, and srcGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes DataCopyParams to support both continuous and non-continuous copy.
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // The number of continuous data blocks is 1.
    intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the continuous data block, in DataBlock. The length here is 512 half elements.
    intriParams.srcGap = 0; // The source operand is copied continuously.
    intriParams.dstGap = 0; // The destination operand is arranged continuously.
    AscendC::DataCopy(srcLocal, srcGlobal, intriParams);
    ```

    Result example:

    ```text
    Input data srcGlobal: [1 2 3 ... 512]
    Output data srcLocal: [1 2 3 ... 512]
    ```

- Unified Buffer -> Global Memory

    ```cpp
    // dstLocal is a LocalTensor of the half type, and dstGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes DataCopyParams to support both continuous and non-continuous copy.
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // The number of continuous data blocks is 1.
    intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the continuous data block, in DataBlock units. The length here is 512 half elements.
    intriParams.srcGap = 0; // Perform continuous copy on the source operand.
    intriParams.dstGap = 0; // The destination operand is arranged continuously.
    AscendC::DataCopy(dstGlobal, dstLocal, intriParams);
    ```

    Result example:

    ```text
    Input data dstLocal: [1 2 3 ... 512]
    Output data dstGlobal: [1 2 3 ... 512]
    ```
