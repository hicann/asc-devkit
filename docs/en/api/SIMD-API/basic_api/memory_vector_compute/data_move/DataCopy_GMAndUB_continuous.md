# Continuous Data Transfer Between GM and UB (DataCopy)<a name="ZH-CN_TOPIC_0000002382908021"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T13:09:17.106Z -->

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
- Atlas inference products AI Core: Supported
<!-- end id21 -->
<!-- npu="310p" id22 -->
- Atlas inference products Vector Core: Supported
<!-- end id22 -->
<!-- npu="910" id23 -->
- Atlas training products: Supported
<!-- end id23 -->

## Description<a name="section474617392321"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports continuous data copy between Global Memory and Unified Buffer. The data remains in its original format and content during transmission.

The supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id1 -->

## Prototype<a name="section1954364615315"></a>

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Type](#section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand.<br>&bull; When the type is [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md), the storage location is Unified Buffer, and the destination address must be 32-byte aligned.<br>&bull; When the type is [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md), the storage location is Global Memory, and the destination address must be aligned according to the number of bytes occupied by the corresponding data type. |
| src | Input | Source operand.<br>&bull; When the type is GlobalTensor, the storage location is Global Memory, and the source address must be aligned according to the number of bytes occupied by the corresponding data type.<br>&bull; When the type is LocalTensor, the storage location is Unified Buffer, and the source address must be 32-byte aligned. |
| count | Input | Number of elements involved in the copy.<br>Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32 bytes. |

Taking the half data type as an example, the shape of the source operand is 1 \* 128. When count = 128, [Figure 1](#zh-cn_topic_0000002534928976_fig54801848104717) continuously copies 128 elements from the source operand to the destination operand.

**Figure 1** Continuous data copy diagram<a name="zh-cn_topic_0000002534928976_fig54801848104717"></a>  
![](../../../../figures/continuous_data_copy_diagram.png "Continuous data copy diagram")

## Data Type<a name="section4219135304818"></a>

The data types supported by the source operand and the destination operand must be consistent. The Global Memory -> Unified Buffer and Unified Buffer -> Global Memory data paths support the same data types on the same product, as follows:

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: the supported data types are b8, b16, b32, and b64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id4 -->

<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id5 -->

<!-- npu="310p" id6 -->
- Atlas inference products (AI Core): the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id6 -->

<!-- npu="310p" id7 -->
- Atlas inference products (Vector Core): the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id7 -->

<!-- npu="910" id8 -->
- Atlas training products: the supported data types are int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id8 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address in Global Memory must be aligned according to the number of bytes occupied by the corresponding data type, and the address in Unified Buffer must be 32-byte aligned.
- When calling the continuous copy API, count \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
- If multiple DataCopy instructions need to be executed and their destination addresses overlap, you need to call [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that the multiple DataCopy instructions are serialized to prevent abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and their destination Global Memory addresses overlap, you need to call PipeBarrier<PIPE_MTE3>() between the two copy instructions to add synchronization for the MTE3 copy-out pipeline. As shown in the right diagram below, when the destination Unified Buffer addresses overlap, you need to call PipeBarrier<PIPE_MTE2>() between the two copy instructions to add synchronization for the MTE2 copy-in pipeline.

    ![](../../../../figures/datacopy_address_overlap_sync_diagram.png)

<!-- npu="910b,A3" id9 -->
- For the following product models:
    - Atlas A2 training products/Atlas A2 inference products
    - Atlas A3 training products/Atlas A3 inference products

    In the cross-card communication operator development scenario, DataCopy APIs support cross-card data copy. Only the HCCS physical link is supported, and other paths are not supported. During development, you need to pay attention to the physical path involved in inter-card communication. You can run the `npu-smi info -t topo` command to query the HCCS physical link.
<!-- end id9 -->

## Example<a name="section122101199486"></a>

- Global Memory -> Unified Buffer

    ```cpp
    // srcLocal is a LocalTensor of the half type, and srcGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes the count parameter to complete the continuous copy.
    AscendC::DataCopy(srcLocal, srcGlobal, 512);
    ```

    Result example:

    ```text
    Input data srcGlobal: [1 2 3 ... 512]
    Output data srcLocal: [1 2 3 ... 512]
    ```

- Unified Buffer -> Global Memory

    ```cpp
    // dstLocal is a LocalTensor of the half type, and dstGlobal is a GlobalTensor of the half type.
    // Use the copy API that takes the count parameter to complete the continuous copy.
    AscendC::DataCopy(dstGlobal, dstLocal, 512);
    ```

    Result example:

    ```text
    Input data dstLocal: [1 2 3 ... 512]
    Output data dstGlobal: [1 2 3 ... 512]
    ```
