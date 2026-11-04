# DataCopy (Contiguous Data Movement Between GM and UB)<a name="ZH-CN_TOPIC_0000002382908021"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:33:42.218Z pushedAt=2026-10-28T03:14:39.487Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id21 -->
- Ascend 950PR&950DT products: supported
<!-- end id21 -->
<!-- npu="A3" id22 -->
- Atlas A3 products: supported
<!-- end id22 -->
<!-- npu="910b" id23 -->
- Atlas A2 products: supported
<!-- end id23 -->
<!-- npu="310b" id24 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id24 -->
<!-- npu="310p" id25 -->
- Atlas inference products AI Core: supported
<!-- end id25 -->
<!-- npu="310p" id26 -->
- Atlas inference products Vector Core: supported
<!-- end id26 -->
<!-- npu="910" id27 -->
- Atlas training products: supported
<!-- end id27 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMAndUB_continuous_res.md#id1 -->

## Description<a name="section474617392321"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports continuous data copy between Global Memory and Unified Buffer (UB). The data keeps its original format and content unchanged during transmission.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id1 -->

## Prototype<a name="section1954364615315"></a>

- Global Memory -> UB

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
    ```

- UB -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Types](#section4219135304818). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand.<br>&bull;When the type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md), the storage location is UB, and the destination address requires 32-byte alignment.<br>&bull;When the type is [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md), the storage location is Global Memory, and the destination address requires alignment by the number of bytes occupied by the corresponding data type. |
| src | Input | Source operand.<br>&bull;When the type is GlobalTensor, the storage location is Global Memory, and the source address requires alignment by the number of bytes occupied by the corresponding data type.<br>&bull;When the type is LocalTensor, the storage location is UB, and the source address requires 32-byte alignment. |
| count | Input | Number of elements to be copied.<br>**Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

Taking the half data type as an example, the shape of the source operand is 1 \* 128. When count = 128, [Figure 1](#zh-cn_topic_0000002534928976_fig54801848104717) continuously copies 128 elements from the source operand to the destination operand.

**Figure 1**  Continuous data copy diagram<a name="zh-cn_topic_0000002534928976_fig54801848104717"></a>  
![](../../../../figures/continuous_data_copy_diagram.png "Continuous data copy diagram")

## Data Type<a name="section4219135304818"></a>

The source operand and destination operand support the same data types. The Global Memory -> UB and UB -> Global Memory data paths support the same data types for the same product, as follows:

<!-- npu="950" id2 -->
- For Ascend 950PR&950DT products, the supported data types are: b8, b16, b32, b64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id4 -->

<!-- npu="310b" id5 -->
- For Atlas 200I/500 A2 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id5 -->

<!-- npu="310p" id6 -->
- For Atlas inference products AI Core, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id6 -->

<!-- npu="310p" id7 -->
- For Atlas inference products Vector Core, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id7 -->

<!-- npu="910" id8 -->
- For Atlas training products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id8 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- Addresses in Global Memory must be aligned to the number of bytes occupied by the corresponding data type, and addresses in UB must be 32-byte aligned.
- When calling the continuous copy API, **count** \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
- If multiple DataCopy instructions need to be executed and their destination addresses overlap, call [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that the multiple DataCopy instructions are serialized to prevent abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and their destination Global Memory addresses overlap, call `PipeBarrier<PIPE_MTE3>()` between the two copy instructions to synchronize the MTE3 copy-out pipeline. As shown in the right diagram below, when the destination UB addresses overlap, call `PipeBarrier<PIPE_MTE2>()` between the two copy instructions to synchronize the MTE2 copy-in pipeline.

    ![](../../../../figures/datacopy_address_overlap_sync_diagram.png)

<!-- npu="910b,A3" id11 -->
- For the following product models:
    - Atlas A2 products
    - Atlas A3 products

    In the cross-card communication operator development scenario, the DataCopy class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the `npu-smi info -t topo` command to query the HCCS physical link.
<!-- end id11 -->

## Examples<a name="section122101199486"></a>

- Global Memory -> UB

    ```cpp
    // srcLocal is a LocalTensor of the half type, and srcGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes the count parameter to complete continuous copy.
    AscendC::DataCopy(srcLocal, srcGlobal, 512);
    ```

    Result example:

    ```text
    Input data srcGlobal: [1 2 3 ... 512]
    Output data srcLocal: [1 2 3 ... 512]
    ```

- UB -> Global Memory

    ```cpp
    // dstLocal is a LocalTensor of the half type, and dstGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes the count parameter to complete continuous copy.
    AscendC::DataCopy(dstGlobal, dstLocal, 512);
    ```

    Result example:

    ```text
    Input data dstLocal: [1 2 3 ... 512]
    Output data dstGlobal: [1 2 3 ... 512]
    ```
