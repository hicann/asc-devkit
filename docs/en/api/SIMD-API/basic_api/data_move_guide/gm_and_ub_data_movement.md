# GM and UB Data Movement<a name="ZH-CN_TOPIC_0000002573902833"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T12:36:50.134Z -->

## Overview<a name="ZH-CN_TOPIC_0000002543422566"></a>

Data movement between Global Memory (GM) and Unified Buffer (UB) provides a variety of APIs that support rich data movement scenarios (see [Table 1](#zh-cn_topic_0000002565937657_table12546123753110)). To achieve efficient vector computation, the system supports moving data from GM to UB for the Vector computation unit to perform vector computation; after computation is complete, the results are moved back from UB to GM, completing the data loop. By flexibly configuring structure parameters, developers can precisely control key attributes such as the number of data blocks, the length of each address, and the address interval, thereby implementing contiguous and non-contiguous data movement (high-dimensional slicing data movement). This fully unleashes the hardware's data movement and computation collaboration capabilities, significantly improving the execution efficiency and overall performance of vector operators.

**Table 1**  Overview of GM-to-UB data movement APIs

<a name="zh-cn_topic_0000002565937657_table12546123753110"></a>

| Data Path | Function | Description |
| :---: | :--- | :--- |
| GM->UB | DataCopy (GM -> UB - contiguous data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting contiguous data movement. |
| UB->GM | DataCopy (UB -> GM - contiguous data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting contiguous data movement. |
| GM->UB | DataCopy (GM -> UB - high-dimensional slicing data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting contiguous and non-contiguous data movement. |
| UB->GM | DataCopy (UB -> GM - high-dimensional slicing data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting contiguous and non-contiguous data movement. |
| GM->UB | DataCopy (GM -> UB - sliced data movement) | This API is a software simulation API that supports sliced data movement, extracting a subset of multi-dimensional Tensor data for movement. |
| UB->GM | DataCopy (UB -> GM - sliced data movement) | This API is a software simulation API that supports sliced data movement, extracting a subset of multi-dimensional Tensor data for movement. |
| GM->UB | DataCopy (GM -> UB - inline conversion ND2NZ movement) | This API is a software simulation API that supports ND-to-NZ format conversion during data movement from Global Memory to Unified Buffer. |
| UB->GM | DataCopy (UB -> GM - inline conversion NZ2ND movement) | This API is a software simulation API that supports NZ-to-ND format conversion during data movement from Unified Buffer to Global Memory. |
| GM->UB | DataCopy (GM -> UB - multi-dimensional data movement NDDMA) | A multi-dimensional data movement API. Compared with the basic data movement API, it allows more flexible configuration of the dimension information to be moved in and the corresponding Stride. |
| GM->UB | DataCopyPad (GM -> UB - unaligned data movement) | Supports unaligned data movement, allowing data to be padded when moved to UB. |
| UB->GM | DataCopyPad (UB -> GM - unaligned data movement) | Supports unaligned data movement. |

## DataCopy (GM and UB - Contiguous Data Movement)<a name="ZH-CN_TOPIC_0000002382908021"></a>

Supports contiguous data movement between Global Memory and Unified Buffer, with the data retaining its original format and content during transmission.

The specifically supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id1 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific APIs, see [DataCopy (GM and UB - Contiguous Data Movement)](../memory_vector_compute/data_move/DataCopy_GMAndUB_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved contiguously.

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

## DataCopy (GM and UB - High-Dimensional Slicing Data Movement)<a name="ZH-CN_TOPIC_00000023829080211"></a>

Supports high-dimensional slicing data movement between Global Memory and Unified Buffer. The data retains its original format and content during transmission.

High-dimensional slicing data movement supports both non-contiguous and contiguous data movement by configuring movement parameters such as the number of data blocks, the length of a single data block, and address offsets.

The specific supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id2 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id2 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, see [DataCopy (GM and UB - High-Dimensional Slicing Data Movement)](../memory_vector_compute/data_move/DataCopy_GMAndUB_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively. **repeatParams** is the movement parameters of the **DataCopyParams** type, through which you can configure the size, count, and interval of the data blocks to be moved, supporting both non-contiguous and contiguous data movement.

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

## DataCopy (GM and UB - Sliced Data Movement)<a name="ZH-CN_TOPIC_0000002568770166"></a>

This API is a software simulation API designed from the perspective of ease of use. It supports sliced data movement, extracting a subset of a multi-dimensional Tensor for movement.

The supported data paths are as follows (represented by [logical position TPosition](../auxiliary data structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id3 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id3 -->

The moved data is used for [vector computation](../memory_vector_compute/Memory vector computation.md). For the specific API, see [DataCopy (GM and UB - Sliced Data Movement)](../memory_vector_compute/data movement/DataCopy_GMAndUB_slice.md).

**src** and **dst** are the source operand and destination operand, respectively. **dstSliceInfo** and **srcSliceInfo** are the slice information of the destination and source operands, respectively, of the type **SliceInfo**. **dimValue** is the operand dimension information.

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T> &dst, const LocalTensor<T> &src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

## DataCopy (GM -> UB - inline conversion ND2NZ movement)<a name="ZH-CN_TOPIC_0000002349187356"></a>

This API is a software simulation API designed from the perspective of ease of use. It supports ND-to-NZ format conversion during data movement from Global Memory to Unified Buffer.

The supported data paths are as follows (represented by [logical position TPosition](../auxiliary data structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN

The moved data is used for [vector computation](../memory_vector_compute/Memory vector computation.md). For details about the API, see [DataCopy (GM -> UB - inline conversion ND2NZ movement)](../memory_vector_compute/data movement/DataCopy_GMToUB_ND2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively. **intriParams** is the movement parameters of the **Nd2NzParams** type, which configures the ND-to-NZ format conversion information.

- The **enableSmallC0** mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id4 -->
- The **enableSmallC0** mode is supported (only Ascend 950PR/Ascend 950DT support it).

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id4 -->

## DataCopy (UB -> GM - inline conversion NZ2ND movement)<a name="ZH-CN_TOPIC_0000002391805265"></a>

This API is a software simulation API designed from the perspective of ease of use. It supports conversion from NZ to ND format during data movement from Unified Buffer to Global Memory.

The supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id5 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id5 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, see [DataCopy (UB -> GM - inline conversion NZ2ND movement)](../memory_vector_compute/data_move/DataCopy_UBToGM_NZ2ND.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameters of the **Nz2NdParamsFull** type, which configures the NZ-to-ND format conversion information.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams)
```

## DataCopy (GM -> UB - Multi-dimensional Data Movement NDDMA)<a name="ZH-CN_TOPIC_0000002544407954"></a>

The multi-dimensional data movement API, compared with the basic data movement API, allows more flexible configuration of the dimension information to be moved in and the corresponding Stride.

The specifically supported data paths are as follows (represented by [logical position TPosition](../auxiliary data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN

The moved data is used for [vector computation](../memory_vector_compute/Memory vector computation.md). For the specific API, see [DataCopy (GM -> UB - Multi-dimensional Data Movement NDDMA)](../memory_vector_compute/data movement/DataCopy_GMToUB_NDDMA.md).

**src** and **dst** are the source operand and destination operand, respectively; **params** is the movement parameters of the **NdDmaParams** type, which configures the multi-dimensional movement information.

- Global Memory -> Unified Buffer, supporting multi-dimensional data movement

    ```cpp
    template <typename T, uint8_t dim, const NdDmaConfig& config = kDefaultNdDmaConfig>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const NdDmaParams<T, dim>& params)
    ```

- NDDMA Cache refresh

    ```cpp
    __aicore__ inline void NdDmaDci()
    ```

## DataCopyPad (GM -> UB - Unaligned Data Movement)<a name="ZH-CN_TOPIC_0000001894460401"></a>

This API moves data from Global Memory to Unified Buffer in an unaligned manner, allowing developers to pad data as needed. When the length of each moved data block (**blockLen**) is not 32-byte aligned, each data block must be padded to 32-byte alignment.

<!-- npu="950" id6 -->
In particular, for Ascend 950PR/Ascend 950DT, the Compact mode is supported. This mode allows a single data block to be non-32-byte aligned, merges all data blocks into one contiguous data block, and pads data on the right side of that block to 32-byte alignment.
<!-- end id6 -->

The specifically supported data paths are as follows (expressed in terms of [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
    - GM -> VECOUT
    <!-- npu="950" id7 -->
    - GM -> VECCALC (supported only on Ascend 950PR/Ascend 950DT)
    <!-- end id7 -->

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, see [DataCopyPad (GM -> UB - Unaligned Data Movement)](../memory_vector_compute/data_move/DataCopyPad_GMToUB.md).

**src** and **dst** are the source operand and destination operand, respectively; **dataCopyParams** is the movement parameters of the **DataCopyExtParams** type; **padParams** is the padding control parameters of the **DataCopyPadExtParams** type.

- Configuring the data movement mode **mode** is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```

<!-- npu="950" id8 -->
- Configuring the data movement mode **mode** is supported (only Ascend 950PR/Ascend 950DT).

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```
<!-- end id8 -->

## DataCopyPad (UB -> GM - Unaligned Data Movement)<a name="ZH-CN_TOPIC_0000001894460502"></a>

This API moves data from the Unified Buffer to Global Memory in an unaligned manner.

For scenarios that are not 32-byte aligned, dummy data is filled in to align to 32B when reading Unified Buffer data, and the dummy empty data is discarded when moving into Global Memory, thereby implementing unaligned data movement from the Unified Buffer to Global Memory.

The supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> Global Memory
    - VECIN -> GM
    - VECOUT -> GM

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, see [DataCopyPad (UB -> GM - Unaligned Data Movement)](../memory_vector_compute/data_move/DataCopyPad_UBToGM.md).

**src** and **dst** are the source operand and destination operand, respectively; **dataCopyParams** is the movement parameters of the **DataCopyExtParams** type.

- Configuring the data movement mode **mode** is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- npu="950" id9 -->
- Supports configuring the data movement mode **mode** (supported only on Ascend 950PR/Ascend 950DT).

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```
<!-- end id9 -->
